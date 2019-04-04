#pragma once

#include "MessageDecoder.h"
#include "Protocol.h"

#include <vector>

class CServerLogic {
  enum class EComputeResult {
      eAllRight
    , eOverflow
    , eDivByZero
  };
public:
  CServerLogic(CProtocol& p);

  void execute();

private:
  void clearState();

  EComputeResult compute(CMessageDecoder& dec, int& res);

private:
  std::vector< int > opVec;
  CProtocol& proto;
};
