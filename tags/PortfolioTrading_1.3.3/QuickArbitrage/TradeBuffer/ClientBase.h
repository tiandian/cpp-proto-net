#pragma once

#include "QuoteListener.h"
#include "BufferRunner.h"
#include "protobuf_gen/quote.pb.h"
#include "MsgPack.h"

#include <vector>
#include <string>

using namespace std;

class ClientBase : public QuoteListener
{
public:
	ClientBase(void);
	virtual ~ClientBase(void);

	void Subscribe(vector<string>& symbols);
	void UnSubscribe();

	virtual bool Login(const std::string& brokerId, const std::string& userId, const std::string& password);
	virtual void Logout();

	void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);

	virtual void OnRegisterResult(bool succ, std::string& errMsg)
	{
		m_tradeLoggedin = succ;
	}

protected:
	
	template<typename T>
	void EnqueueMessage(MSG_TYPE type, boost::shared_ptr<T>& pT)
	{
		boost::shared_ptr<MsgPack> pack(new MsgPackT<T>(type, pT));
		m_pbfRunner->Enqueue(pack);
	}

	virtual void ProcessMessage(MSG_TYPE type, void* pData){}

	virtual void ProcessQuote(CTP::Quote* pQuote){}

protected:

	std::string& GetBrokerId(){return m_brokerId;}
	std::string& GetUserId(){return m_userId;}

	std::string m_brokerId;
	std::string m_userId;
	bool m_tradeLoggedin;

private:
	void ProcessMsgPack(boost::shared_ptr<MsgPack>& pPack);

	CBufferRunner< boost::shared_ptr<MsgPack> >* m_pbfRunner;
};

