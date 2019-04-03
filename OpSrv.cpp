
#include "WSAInitializer.h"
#include "SocketListen.h"

#include <iostream>
#include <sstream>
#include <cassert>

int main()
{
  CWSAInitializer wsaInitializer;

  CSocketListen socketListen { 5001 };

  if (socketListen.openBindListen()) {
    // blocking
    auto socket_message = socketListen.accept();
    assert( socket_message->isValid() );

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

    const std::string enter{ "\r\n" };

    int messageIndex { 100 };
    EStatus res { EStatus::eOK };
    while( res == EStatus::eOK ) {
      std::cout << "Server: sending " << messageIndex << std::endl;
      //std::stringstream ss;
      //ss << "Message " << messageIndex << " from server";
      res = socket_message->sendOk();
      res = socket_message->sendString(enter);
      res = socket_message->sendValue(messageIndex);
      res = socket_message->sendString(enter);
      res = socket_message->sendOperator('*');
      res = socket_message->sendString(enter);
      res = socket_message->sendResult(messageIndex);
      res = socket_message->sendString(enter);
      res = socket_message->sendError("Some random error");
      res = socket_message->sendString(enter);
      messageIndex--;
      getchar();
    }
  }

	return 0;
}
