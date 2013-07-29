#pragma once

#include "Enums.h"
#include "entity/message.pb.h"
#include "charsetconvert.h"

using namespace System;
using namespace System::Text;
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
			_errorMsg = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->error_msg().c_str());
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

		property String^ Address
		{
			String^ get()
			{
				return _address;
			}
		}

		property String^ BrokerId
		{
			String^ get()
			{
				return _brokerId;
			}
		}

		property String^ InvestorId
		{
			String^ get()
			{
				return _investorId;
			}
		}

		property ServerType SeverType
		{
			ServerType get()
			{
				return _serverType;
			}
		}

		void From(entity::ServerLoginResponse* pEntity)
		{
			_success = pEntity->success();
			
			wchar_t* uniStr = UTF8ToUnicode(pEntity->errormessage().c_str());
			//String ^utfStr = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->errormessage().c_str());
			_errorMsg = Marshal::PtrToStringAuto((IntPtr)uniStr);
			delete[] uniStr;

			_serverType = static_cast<ServerType>(pEntity->type());
			_address = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->address().c_str());
			_brokerId = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->brokerid().c_str());
			_investorId = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->userid().c_str());
		}

	private:
		bool _success;
		String ^_errorMsg;
		ServerType _serverType;
		String ^_address;
		String ^_brokerId;
		String ^_investorId;
	};
}