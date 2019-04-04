
#include "ServerLogic.h"

#include <cassert>

CServerLogic::CServerLogic(CProtocol& p)
  : proto{ p }
{
}

void CServerLogic::execute()
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
            int computedValue { 0 };
            const auto res = compute(dec, computedValue);
            switch (res) {
              case EComputeResult::eAllRight : {
                proto.sendResult(computedValue);
                break;
              }
              case EComputeResult::eDivByZero : {
                proto.sendError("Error performing computation. Division by zero. State flushed.");
                break;
              }
              case EComputeResult::eOverflow : {
                proto.sendError("Error performing computation. Overflow or underflow. State flushed.");
                break;
              }
              
              default:
                assert("EComputeResult - other enum type is not implemented" == nullptr);
                break;
            }
            clearState();
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

void CServerLogic::clearState()
{
  opVec.resize(0);
}

CServerLogic::EComputeResult CServerLogic::compute(CMessageDecoder& dec, int& res)
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
        return EComputeResult::eAllRight;
      }
      else {
        return EComputeResult::eOverflow;
      }
      break;
    }
    case EOperation::eDiff: {
      res = opVec[0] - opVec[1];
      return EComputeResult::eAllRight;
      break;
    }
    case EOperation::eMulti: {
      const long long resLongLong { opVec[0] * opVec[1] };
      const int resInt { static_cast< int >( resLongLong ) };
      if (resLongLong == resInt) {
        res = resInt;
        return EComputeResult::eAllRight;
      }
      else {
        return EComputeResult::eOverflow;
      }
      break;
    }
    case EOperation::eDiv: {
      if (opVec[1] == 0) {
        return EComputeResult::eDivByZero;
      }
      else {
        res = opVec[0] / opVec[1];
        return EComputeResult::eAllRight;
      }
      break;
    }
    case EOperation::eExp: {
      const double resDouble { std::pow( opVec[0] , opVec[ 1 ] ) };
      const long long resLongLong { static_cast< long long >( resDouble ) };
      const int resInt { static_cast< int >( resLongLong ) };
      if (resDouble == resInt) {
        res = resInt;
        return EComputeResult::eAllRight;
      }
      else {
        return EComputeResult::eOverflow;
      }
      break;
    }
    case EOperation::eMod: {
      if (opVec[1] == 0) {
        return EComputeResult::eDivByZero;
      }
      else {
        res = opVec[0] % opVec[1];
        return EComputeResult::eAllRight;
      }
      break;
    }
    default:
      assert(false);
  }
}

