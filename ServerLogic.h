#pragma once

#include "MessageDecoder.h"
#include "Protocol.h"

#include <vector>

class CServerLogic {
public:
  CServerLogic(CProtocol& p);

  void execute();

private:
  void clearState();

  bool compute(CMessageDecoder& dec, int& res);

private:
  std::vector< int > opVec;
  CProtocol& proto;
};
