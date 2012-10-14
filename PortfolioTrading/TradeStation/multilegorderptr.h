#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<trade::MultiLegOrder> MultiLegOrderPtr;