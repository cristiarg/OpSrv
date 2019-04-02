#include "SocketMessage.h"

#include <cassert>

CSocketMessage::CSocketMessage( SOCKET s )
    : socket_message { s }
{
}

CSocketMessage::~CSocketMessage()
{
  if ( isValid() ) {
   closesocket(socket_message); 
  }
}

bool CSocketMessage::isValid() const
{
  return ( socket_message != INVALID_SOCKET );
}

//void CSocketMessage::sendMessage(const std::int32_t v) const
//{
//  assert( isValid() );
//
//  if (isValid()) {
//
//  }
//  // TODO: return
//}

void CSocketMessage::sendMessage(const std::string& s) const
{
  assert( isValid() );

  if (isValid()) {
    const std::int32_t messLen = s.size();
    const auto res1 = rawSend( reinterpret_cast<const char*>( &messLen ), sizeof( std::int32_t ) );
    assert(res1 == sizeof( std::int32_t ) );
    if (res1 > 0) {
      const auto res2 = rawSend(s.c_str(), (int)s.size());
      assert(res2 == s.size());
    }
  }
  // TODO: return error codes
}

//int CSocketMessage::recvMessage(char& c)
//{
//  return rawRecv(&c, 1);
//}

int CSocketMessage::rawSend(const char* const buff, const int buffLen) const
{
  assert( isValid() );
  if (isValid()) {
    const char* locBuff = buff;
    int locBuffLenSent { 0 };
    int locBuffLenStillToSend = buffLen;
    while (locBuffLenStillToSend > 0) {
      const int sendRes = ::send(socket_message, buff, buffLen, 0); // no flags

      if (sendRes == SOCKET_ERROR) {
        fprintf(stderr,"send() failed with error %d\n",WSAGetLastError());
        locBuffLenStillToSend = 0;
      }
      else {
        locBuffLenSent += sendRes;
        locBuffLenStillToSend -= sendRes;
        locBuff += sendRes;

        assert(locBuffLenStillToSend >= 0);
      }
    }
    return locBuffLenSent;
  }
  else {
    fprintf(stderr, "failed to send message; socket is invalid\n");
    return -1;
  }
}

int CSocketMessage::rawRecv(char* const buff, const int buffLen)
{
  char* pBuffer = buff;
  int nRecvCount = 0;
  int nStillToRecvCount = buffLen;
  while (nStillToRecvCount > 0 ) {
    const int c_nResult = recv(socket_message, pBuffer, nStillToRecvCount, 0);
      // from the docs: If no error occurs, recv returns the number of bytes received and the buffer pointed to by the
      // buf parameter will contain this data received.
      // If the connection has been gracefully closed, the return value is zero.
    if (c_nResult > 0)
    {
      nRecvCount += c_nResult;
      pBuffer += c_nResult;
      nStillToRecvCount -= c_nResult;
      assert(nStillToRecvCount >= 0);
    }
    else if (c_nResult == 0)
    {
      const int c_eWSALastError = WSAGetLastError();
      assert( c_eWSALastError == 0 );
      printf("INFO connection gracefully closed by remote (apparently)\n");
      return nRecvCount;
      //return std::make_tuple( nRecvCount  , mc_eConnectionGracefullyClosed );
    }
    else //(c_nResult < 0)
    {
      const int c_eWSALastError = WSAGetLastError();
      if (c_eWSALastError == WSAETIMEDOUT)
      {
        fprintf(stderr, "ERROR recv: timeout\n");
        return nRecvCount;
      }
      else if ( c_eWSALastError == WSAECONNRESET )
      {
        fprintf(stderr, "ERROR recv: connection reset by peer\n");
        closesocket(socket_message);
        socket_message = INVALID_SOCKET;
        return nRecvCount;
      }
      else
      {
        fprintf(stderr, "ERROR recv: undistinguished error (%d)\n", c_eWSALastError);
        closesocket(socket_message);
        socket_message = INVALID_SOCKET;
        return nRecvCount;
      }
    }
  }
  return nRecvCount;
}

