package edu.umn.congestion;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Environment;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Collections;
import java.util.List;

import eu.chainfire.libsuperuser.Shell;

public class MainActivity extends AppCompatActivity {

    String tgtIp = "";
    String tgtPort = "";
    String filename = "tmp";
    String duration = "10";

    String rtcpCmd;
    Button run_bt;
    TextView tv;
    Shell.Interactive shell;
    String tcpdump_command;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tv = findViewById(R.id.textView);
        String ip = getIPAddress(true);
        tv.setText(ip);
        run_bt = (Button) findViewById(R.id.run);
        run_bt.setOnClickListener(view -> {
            Toast.makeText(MainActivity.this, "starting rtcp", Toast.LENGTH_SHORT).show();

            new Thread(() -> {

                /* Run TCPDump if Root */
                if (checkRooted()) {

                    tcpdump_command = "tcpdump -i any -s 96 -w " +
                            GetTCPDumpFullPath(filename);

                    // Prepare shell command
                    // Callback to report whether the shell was successfully started up
                    shell = new Shell.Builder().
                            useSU().
                            setWantSTDERR(true).
                            setMinimalLogging(true).
                            open((success, reason) -> {
                                if (!success) {
                                    Log.e("!CC!TCPDUMP", "Error opening root shell.");
                                } else {
                                    // Shell is up: send command;
                                    sendTCPDumpCommand();
                                    Log.d("!CC!TCPDUMP", "TcpDump Running");
                                }
                            });
                } else {
                    Log.e("!CC!TCPDUMP", "No root permissions");
                }

                if (checkRooted()) {
                    /* start raw tcp socket on receiver */
                    rtcpCmd = "./data/local/tmp/receiver " + getIPAddress(true) + " " + tgtIp + " " + tgtPort + " " + filename + " " + duration;
                    try {
                        Process p = Runtime.getRuntime().exec("su");
                        DataOutputStream os = new DataOutputStream(p.getOutputStream());
                        os.writeBytes(rtcpCmd + "\n");
                        os.writeBytes("exit\n");
                        os.flush();
                    }
                    catch (IOException e) {
                        tv.setText(e.toString());
                    }
                }
                else {
                    Log.e("!CC!rtcp", "No root permissions");
                }


                /* end the tcpdump session */
                tcpdump_command = null;
                if (shell != null) {
                    try{
                        List<String> out = Shell.SU.run("ps -A | grep tcpdump");
                        for(String x : out) {
                            String[] temp = x.split("\\s+");
                            Integer pid =  Integer.valueOf(temp[1]);
                            List<String> exitOutput =  Shell.SU.run("kill -9 " + pid.toString());
                            Log.d("!AN!TCPDUMP", "Killed pid:" + pid.toString());
                        }
                    }
                    catch(Exception ex) {
                        ex.printStackTrace();
                        throw ex;
                    }
                    Log.d("!CC!TCPDUMP", "Closed TCPDUMP shell.");
                }
            }).start();
        });
    }

    public static String GetTCPDumpFullPath(String filename) {
        try {
            Files.createDirectories(Paths.get(Environment.getExternalStorageDirectory() + "/TcpDump/"));
        } catch (IOException e) {
            e.printStackTrace();
        }

        return "/sdcard/" + "/TcpDump/" + filename + "-td.pcap";
    }

    /* TCPDUMP COMMAND */
    private void sendTCPDumpCommand() {
        shell.addCommand(new String[] {tcpdump_command}, 0,
                new Shell.OnCommandResultListener() {
                    @Override
                    public void onCommandResult(int commandCode, int exitCode, @NonNull List<String> output) {
                        if (exitCode < 0) {
                            Log.e("!CC!TCPDUMP", "Error executing commands: exitCode." + exitCode);
                        }
                    }
                });
        Log.i("!CC!TCPDUMP", "Started TCPDUMP shell.");
        Toast.makeText(this, "TCPDUMP Started", Toast.LENGTH_SHORT).show();
    }


    public static String getIPAddress(boolean useIPv4) {
        try {
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface intf : interfaces) {
                List<InetAddress> addrs = Collections.list(intf.getInetAddresses());
                for (InetAddress addr : addrs) {
                    Log.d("congestion", addr.getHostAddress());
                    if (!addr.isLoopbackAddress()) {
                        String sAddr = addr.getHostAddress();
                        boolean isIPv4 = sAddr.indexOf(':')<0;

                        if (useIPv4) {
                            if (isIPv4)
                                return sAddr;
                        } else {
                            if (!isIPv4) {
                                int delim = sAddr.indexOf('%'); // drop ip6 zone suffix
                                return delim<0 ? sAddr.toUpperCase() : sAddr.substring(0, delim).toUpperCase();
                            }
                        }
                    }
                }
            }
        } catch (Exception ignored) { } // for now eat exceptions
        return "";
    }

    // Ref. http://www.stealthcopter.com/blog/2010/01/android-requesting-root-access-in-your-app/
    public static boolean checkRooted()
    {
        try
        {
            Process p = Runtime.getRuntime().exec("su", null, new File("/"));
            DataOutputStream os = new DataOutputStream( p.getOutputStream());
            os.writeBytes("pwd\n");
            os.writeBytes("exit\n");
            os.flush();
            p.waitFor();
            p.destroy();
        }
        catch (Exception e)
        {
            return false;
        }

        return true;
    }
}