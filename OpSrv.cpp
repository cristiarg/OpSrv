
#include "WSAInitializer.h"
#include "SocketListen.h"

#include "OpSrv.h"
#include <Ws2tcpip.h>

void sp_GenericEndlessTCPStreamedSocketClient()
{
  static const int PROCDEC_DEFAULT_BUFLEN = 20000;
  static const char PROCDEC_DEFAULT_ADDRESS[] = "146.122.148.69"; // "146.122.109.159";
  static const char PROCDEC_DEFAULT_PORT[] = "9991";

  WSADATA wsaData;
  SOCKET ConnectSocket = INVALID_SOCKET;
  struct addrinfo *result = NULL,
                  *ptr = NULL,
                  hints;
  char *sendbuf = "this is a test";
  char recvbuf[PROCDEC_DEFAULT_BUFLEN];
  int iResult;
  int recvbuflen = PROCDEC_DEFAULT_BUFLEN;

  //// Validate the parameters
  //if (argc != 2) {
  //    printf("usage: %s server-name\n", argv[0]);
  //    return 1;
  //}

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
      printf("WSAStartup failed with error: %d\n", iResult);
      return;
  }

  ZeroMemory( &hints, sizeof(hints) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  iResult = getaddrinfo(PROCDEC_DEFAULT_ADDRESS, PROCDEC_DEFAULT_PORT, &hints, &result);
  if ( iResult != 0 ) {
      printf("getaddrinfo failed with error: %d\n", iResult);
      WSACleanup();
      return;
  }

  // Attempt to connect to an address until one succeeds
  for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
      // Create a SOCKET for connecting to server
      ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
          ptr->ai_protocol);
      if (ConnectSocket == INVALID_SOCKET) {
          printf("socket failed with error: %ld\n", WSAGetLastError());
          WSACleanup();
          return;
      }

      // Connect to server.
      iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
      if (iResult == SOCKET_ERROR) {
          closesocket(ConnectSocket);
          ConnectSocket = INVALID_SOCKET;
          continue;
      }
      break;
  }

  freeaddrinfo(result);

  if (ConnectSocket == INVALID_SOCKET) {
      printf("Unable to connect to server!\n");
      WSACleanup();
      return;
  }

  //// Send an initial buffer
  //iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
  //if (iResult == SOCKET_ERROR) {
  //    printf("send failed with error: %d\n", WSAGetLastError());
  //    closesocket(ConnectSocket);
  //    WSACleanup();
  //    return 1;
  //}
  //printf("Bytes Sent: %ld\n", iResult);

  // shutdown the connection since no more data will be sent
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
      printf("shutdown failed with error: %d\n", WSAGetLastError());
      closesocket(ConnectSocket);
      WSACleanup();
      return;
  }

  // Receive until the peer closes the connection
  do {
      iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
      if ( iResult > 0 )
          printf("Bytes received: %d\n", iResult);
      else if ( iResult == 0 )
          printf("Connection closed\n");
      else
          printf("recv failed with error: %d\n", WSAGetLastError());
  } while( iResult > 0 );

  // cleanup
  closesocket(ConnectSocket);
  WSACleanup();
}

int main()
{
  CWSAInitializer wsaInitializer;

  CSocketListen socketListen { 5001 };

  if (socketListen.openBindListen()) {
    // blocking
    auto socket_message = socketListen.accept();

    const std::string buff { "Sending back data" };
    socket_message->sendMessage(buff);
  }

  //sp_GenericEndlessTCPStreamedSocketClient();

	return 0;
}
