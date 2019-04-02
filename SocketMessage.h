#pragma once

#include <memory>
#include <string>
#include <Ws2tcpip.h>

class CSocketMessage final {
public:
  CSocketMessage( SOCKET s );
  ~CSocketMessage();

  bool isValid() const;

  void sendMessage( const std::int32_t v ) const;
  void sendMessage( const std::string s ) const;

private:
  int rawSend(const char* const buff, const int buffLen) const;

private:
  SOCKET socket_message;
};

using UXSocketMessage = std::unique_ptr< CSocketMessage >;