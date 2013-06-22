// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef WIN32
#include "targetver.h"
#endif

// TODO: reference additional headers your program requires here

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

#ifdef USE_NET_LIBS 

#include <PushFrameworkInc.h>
using namespace PushFramework;

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
using namespace google;

#endif


