#pragma once

using namespace System;

namespace PTCommunication {

public ref class OperationResult
{
public:
	OperationResult(void){}

	property bool Success
	{
		bool get()
		{
			return _success;
		}
		void set(bool succ) 
		{
			_success = succ;
		}
	}

	property String^ ErrorMessage
	{
		String^ get()
		{
			return _errorMsg;
		}
		void set(String^ errMsg) 
		{
			_errorMsg = errMsg;
		}
	}

private:
	bool _success;
	String ^_errorMsg;
};

}

