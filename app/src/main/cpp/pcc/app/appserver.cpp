#include "appserver.h"

int AppServer::run()
{

   if (0 != getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &res))
   {
      std::cout << "illegal port number or port is busy.\n" << std::endl;
      return 0;
   }

   UDTSOCKET serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

   if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
   {
      std::cout << "bind: " << UDT::getlasterror().getErrorMessage() << std::endl;
      return 0;
   }

   freeaddrinfo(res);

   std::cout << "server is ready at port: " << std::to_string(port).c_str() << std::endl;

   if (UDT::ERROR == UDT::listen(serv, 10))
   {
      std::cout << "listen: " << UDT::getlasterror().getErrorMessage() << std::endl;
      return 0;
   }

   sockaddr_storage clientaddr{};
   int addrlen = sizeof(clientaddr);

   UDTSOCKET recver;

   while (true)
   {
      if (UDT::INVALID_SOCK == (recver = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
      {
         std::cout << "accept: " << UDT::getlasterror().getErrorMessage() << std::endl;
         return 0;
      }

      char clienthost[NI_MAXHOST];
      char clientservice[NI_MAXSERV];
      getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
      std::cout << "new connection: " << clienthost << ":" << clientservice << std::endl;

         pthread_t rcvthread;
         pthread_create(&rcvthread, NULL, recvdata, new UDTSOCKET(recver));
         pthread_detach(rcvthread);
   }

   UDT::close(serv);

   return 0;
}

void* AppServer::recvdata(void* usocket)
{
   UDTSOCKET recver = *(UDTSOCKET*)usocket;
   delete (UDTSOCKET*)usocket;
   pthread_create(new pthread_t, nullptr, monitor, &recver);
   char* data;
   int size = 100000000;
   data = new char[size];

   while (true)
   {
      int rsize = 0;
      int rs;
      while (rsize < size)
      {
         int rcv_size;
         int var_size = sizeof(int);
         UDT::getsockopt(recver, 0, UDT_RCVDATA, &rcv_size, &var_size);
         if (UDT::ERROR == (rs = UDT::recv(recver, data + rsize, size - rsize, 0)))
         {
            std::cout << "recv:" << UDT::getlasterror().getErrorMessage() << std::endl;
            break;
         }

         rsize += rs;
      }

      if (rsize < size)
         break;
   }

   delete [] data;

   UDT::close(recver);

      return nullptr;
}
void* AppServer::monitor(void* s)
{
   UDTSOCKET u = *(UDTSOCKET*)s;

   UDT::TRACEINFO perf;

   std::cout << "SendRate(Mb/s)\tRTT(ms)\tCWnd\tPktSndPeriod(us)\tRecvACK\tRecvNAK" << std::endl;

   while (true)
   {
         sleep(1);

      if (UDT::ERROR == UDT::perfmon(u, &perf))
      {
         std::cout << "perfmon: " << UDT::getlasterror().getErrorMessage() << std::endl;
         break;
      }

      //cout << perf.pktRecvTotal << "\t\t"
      //     << perf.pktRecvNAKTotal << endl;
   }

      return nullptr;
}
