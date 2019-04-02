
#include "WSAInitializer.h"
#include "SocketListen.h"

#include <iostream>
#include <sstream>

int main()
{
  CWSAInitializer wsaInitializer;

  CSocketListen socketListen { 5001 };

  if (socketListen.openBindListen()) {
    // blocking
    auto socket_message = socketListen.accept();

    //int recvLen { 1 };
    //while (recvLen > 0) {
    //  std::cout << "Server: waiting char from client ..." << std::endl;
    //  char ch { 0 };
    //  recvLen = socket_message->recvMessage(ch);
    //  if (recvLen > 0) {
    //    std::cout << "Server: received '" << ch << "'" << std::endl;
    //    std::string str;
    //    str += ch;
    //    socket_message->sendMessage(str);
    //  }
    //}

    int messageIndex { 1 };
    while( true ) {
      std::stringstream ss;
      ss << "Message " << messageIndex << " from server";
      socket_message->sendMessage(ss.str());
      messageIndex++;
    }
  }

  //sp_GenericEndlessTCPStreamedSocketClient();

	return 0;
}
