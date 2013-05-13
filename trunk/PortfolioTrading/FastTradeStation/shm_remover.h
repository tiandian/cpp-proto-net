#pragma once

#include <string>
#include <boost/interprocess/shared_memory_object.hpp>

using namespace std;
using namespace boost::interprocess;

struct shm_remove  
{  
	shm_remove(const string& name) : _name(name)
	{ shared_memory_object::remove(_name.c_str()); }  
	~shm_remove()
	{ shared_memory_object::remove(_name.c_str()); }  

	string _name;
};
