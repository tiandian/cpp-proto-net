#pragma once

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

struct SubscribeQuoteObj  
{  
	enum { LineSize = 10, SymbolSize = 10 };  

	SubscribeQuoteObj()  
		: subscribe(true)
		, running(false)
		, message_in(false)
	{
		clear();
	}

	void clear()
	{
		memset(items, 0, sizeof(items));
	}

	//Mutex to protect access to the object  
	boost::interprocess::interprocess_mutex      mutex;  

	//Condition to wait when the object is ready for subscribe or unsubscribe  
	boost::interprocess::interprocess_condition  cond_ready;  

	//Condition to wait when the object is to be submit  
	boost::interprocess::interprocess_condition  cond_submit;  

	//Items to fill  
	char   items[SymbolSize][LineSize];  

	bool subscribe;
	bool running;
	bool message_in;
};  