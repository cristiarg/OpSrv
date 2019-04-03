
#include "WSAInitializer.h"
#include "SocketListen.h"
#include "MessageDecoder.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

class CServerLogic {
public:
  CServerLogic(CProtocol& p)
    : proto{ p }
  {
  }

  void execute()
  {
    bool halt { false };
    EStatus st;
    std::string mess;
    while (!halt) {
      st = proto.recvMessage(mess);
      CMessageDecoder dec { mess };
      if (dec.isValid()) {
        switch (dec.getType()) {
          case EMessageType::eValue: {
            if (opVec.size() == 2) {
              // already too many operands in the stack
              proto.sendError("Too many operands. State flushed.");
              clearState();
            }
            else {
              opVec.push_back(dec.getValue());
              proto.sendOk();
            }
            break;
          }
          case EMessageType::eOperator: {
            if (opVec.size() < 2) {
              proto.sendError("Too few operands. State flushed.");
              clearState();
            }
            else if (opVec.size() > 2) {
              proto.sendError("Too many operands. State flushed.");
              clearState();
            }
            else {
              int res { 0 };
              const auto b = compute(dec, res);
              if (b) {
                proto.sendResult(res);
                clearState();
              }
              else {
                proto.sendError("Error performing computation. Overflow or division by zero. State flushed.");
                clearState();
              }
            }
            break;
          }
          case EMessageType::eResult: {
            assert( "EMessageType::eResult - not implemented on the server" == nullptr );
            break;
          }
          case EMessageType::eOK: {
            assert( "EMessageType::eOK - not implemented on the server" == nullptr );
            break;
          }
          case EMessageType::eError: {
            assert( "EMessageType::eError - not implemented on the server" == nullptr );
            break;
          }
          case EMessageType::eQuit: {
            proto.sendOk();
            halt = true;
            break;
          }
        }
      }
      else {
        proto.sendError("Previous message was invalid.");
        clearState();
      }
    }
  }

private:
  void clearState()
  {
    opVec.resize(0);
  }

  bool compute(CMessageDecoder& dec, int& res)
  {
    switch (dec.getOperation()) {
      case EOperation::eNone: {
        assert(false);
        break;
      }
      case EOperation::eAdd: {
        const long long resLongLong { opVec[0] + opVec[1] };
        const int resInt { static_cast< int >( resLongLong ) };
        if (resLongLong == resInt) {
          res = resInt;
          return true;
        }
        else {
          return false;
        }
        break;
      }
      case EOperation::eDiff: {
        res = opVec[0] - opVec[1];
        return true;
        break;
      }
      case EOperation::eMulti: {
        const long long resLongLong { opVec[0] * opVec[1] };
        const int resInt { static_cast< int >( resLongLong ) };
        if (resLongLong == resInt) {
          res = resInt;
          return true;
        }
        else {
          return false;
        }
        break;
      }
      case EOperation::eDiv: {
        if (opVec[1] == 0) {
          return false;
        }
        else {
          res = opVec[0] / opVec[1];
          return true;
        }
        break;
      }
      case EOperation::eExp: {
        const double resDouble { std::pow( opVec[0] , opVec[ 1 ] ) };
        const long long resLongLong { static_cast< long long >( resDouble ) };
        const int resInt { static_cast< int >( resLongLong ) };
        if (resDouble == resInt) {
          res = resInt;
          return true;
        }
        else {
          return false;
        }
        break;
      }
      case EOperation::eMod: {
        if (opVec[1] == 0) {
          return false;
        }
        else {
          res = opVec[0] % opVec[1];
          return true;
        }
        break;
      }
      default:
        assert(false);
    }
  }

private:
  std::vector< int > opVec;
  CProtocol& proto;
};

int main()
{
  CWSAInitializer wsaInitializer;

  CSocketListen socketListen { 5001 };

  if (socketListen.openBindListen()) {
    // blocking
    auto proto = socketListen.accept();
    assert( proto->isValid() );

    CServerLogic logic { *proto.get() };
    logic.execute();
  }

	return 0;
}
