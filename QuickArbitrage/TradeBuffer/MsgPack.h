#pragma once

#include "MsgType.h"
#include "protobuf_gen/quote.pb.h"
#include "protobuf_gen/login.pb.h"
#include "protobuf_gen/subscribe.pb.h"

#include "boost/smart_ptr.hpp"

class MsgPack
{
public:
	MsgPack(){}
	virtual ~MsgPack(){}

	virtual MSG_TYPE GetType() = 0;
	virtual void* GetMsg() = 0;
};

template<typename T>
class MsgPackT : public MsgPack
{
public:
	MsgPackT(MSG_TYPE type, boost::shared_ptr<T>& msg)
		:m_msgType(type), m_spMsg(msg)
	{}
	~MsgPackT(){}
	
	MSG_TYPE GetType(){ return m_msgType; }
	void* GetMsg() { return m_spMsg.get(); }
	T* GetActualMsg() { return m_spMsg.get(); }

private:
	boost::shared_ptr<T> m_spMsg;
	MSG_TYPE m_msgType;
};