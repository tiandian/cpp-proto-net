// PTCommunication.h

#pragma once

#include "NatvieClient.h"

using namespace System;

namespace PTCommunication {

	public ref class ClientBase
	{
	private:
		CNatvieClient *_navtiveClient;
	
	public:
		ClientBase()
		{
			_navtiveClient = new CNatvieClient();
		}
		~ClientBase()
		{
			this->!ClientBase();
		}
		!ClientBase()
		{
			if(_navtiveClient != 0)
			{
				delete _navtiveClient;
				_navtiveClient = 0;
			}
		}
		
		property String ^Name
		{
			String ^get()
			{
				return gcnew String("Hello, C++/cli");
			}
		}
		property double Gpa
		{
			double get()
			{
				return 100.0;
			}
		}
	};
}
