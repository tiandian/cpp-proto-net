#pragma once

#include "../Entity/gen/cpp/trade.pb.h"

#include <string>
#include <boost/function.hpp>

typedef boost::function<void(trade::MultiLegOrder*)> PushMultiLegOrderFunc;
typedef boost::function<void(const std::string&, const std::string&, trade::Order* legOrd)> PushLegOrderFunc;
typedef boost::function<void(trade::Trade*)> PushTradeFunc;
typedef boost::function<void(trade::PositionDetailInfo*)> PushPositionDetailFunc; 
