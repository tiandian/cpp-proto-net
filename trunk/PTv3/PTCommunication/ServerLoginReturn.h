#pragma once

#include "entity/message.pb.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

	public ref class LoginReturn
	{
	public:
		LoginReturn(){}
		LoginReturn(entity::LoginResponse* pEntity)
		{
			From(pEntity);
		}

		property bool Success
		{
			bool get()
			{
				return _success;
			}
		}

		property String^ ErrorMessage
		{
			String^ get()
			{
				return _errorMsg;
			}
		}

		void From(entity::LoginResponse* pEntity)
		{
			_success = pEntity->accepted();
			_errorMsg = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->pseudo().c_str());
		}

	private:
		bool _success;
		String ^_errorMsg;
	};

	public ref class ServerLoginReturn
	{
	public:
		ServerLoginReturn(){}
		ServerLoginReturn(entity::ServerLoginResponse* pEntity)
		{
			From(pEntity);
		}

		property bool Success
		{
			bool get()
			{
				return _success;
			}
		}

		property String^ ErrorMessage
		{
			String^ get()
			{
				return _errorMsg;
			}
		}

		void From(entity::ServerLoginResponse* pEntity)
		{
			_success = pEntity->success();
			_errorMsg = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->errormessage().c_str());
		}

	private:
		bool _success;
		String ^_errorMsg;
	};
}