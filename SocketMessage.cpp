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

void CSocketMessage::sendMessage(const std::int32_t v) const
{
  assert( isValid() );

  if (isValid()) {

  }
  // TODO: return
}

void CSocketMessage::sendMessage(const std::string s) const
{
  assert( isValid() );

  if (isValid()) {
    rawSend(s.c_str(), (int)s.size());
  }
  // TODO: return
}

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
