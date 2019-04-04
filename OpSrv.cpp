
#include "WSAInitializer.h"
#include "SocketListen.h"
#include "ServerLogic.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

int main()
{
  CWSAInitializer wsaInitializer;

  CSocketListen socketListen { 5001 };

  if (socketListen.openBindListen()) {
    // blocking
    auto proto = socketListen.accept();
    assert( proto->isValid() );

    /*
      - at this stage, the client has already sent its first message and is awaiting response
      - execute a loop following the protocol
    */

    CServerLogic logic { *proto.get() };
    logic.execute();
  }

	return 0;
}
