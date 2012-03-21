#pragma once

#include "protobuf_gen/trade.pb.h"

#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>

class CPortfolio;

typedef std::vector< boost::shared_ptr<protoc::InputOrder> > InputOrderVec;
typedef InputOrderVec::iterator InputOrderVecIter;

typedef boost::function<const char*(void)> AssignOrderRefCallback;

boost::shared_ptr<InputOrderVec> CreateOpenPositionOrders(CPortfolio* pPortfolio, AssignOrderRefCallback orderRefCallback);


