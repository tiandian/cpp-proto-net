#pragma once

#include "globalmembers.h"

#include <string>
#include <boost/format.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

using namespace std;
using namespace boost::interprocess;

struct shm_remove  
{  
	shm_remove(const string& name) : _name(name)
	{ 
		bool succ = shared_memory_object::remove(_name.c_str());
#ifdef IN_QUOTE_AGENT
		logger.Debug(boost::str(boost::format("Remove SHM %s before using: %s")
			% _name % (succ ? "Succeeded" : "Failed") ));
#endif
	}  
	~shm_remove()
	{ 
		bool succ = shared_memory_object::remove(_name.c_str());
#ifdef IN_QUOTE_AGENT
		logger.Debug(boost::str(boost::format("Remove SHM %s after using: %s")
			% _name % (succ ? "Succeeded" : "Failed") ));
#endif
	}  

	string _name;
};
