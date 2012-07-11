#pragma once

#include "../../APInvoke/APInvokeNative/APInvokeNative.h"
#include "ClientAgent.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

using namespace std;

typedef boost::shared_ptr<CClientAgent> ClientPtr;
typedef map<string, ClientPtr> ClientMap;
typedef ClientMap::iterator ClientMapIter;
typedef boost::function<void(CClientAgent*, const string&, string&)> ReqTransFunc;
typedef map<string, ReqTransFunc> ReqTranslatorMap;
typedef ReqTranslatorMap::iterator ReqTranslatorMapIter;

class CClientManager : public SessionManagerHandler
{
public:
	CClientManager(void);
	~CClientManager(void);

	virtual void OnConnected(Session* session);

	virtual void OnDisconnected(Session* session);

	virtual void OnError(Session* session, const string& errorMsg);

	virtual void DispatchPacket(const string& sessionId, 
		const string& method, const string& in_data, string& out_data);

	CClientAgent* GetClient(const string& sessionId);

private:
	void InitializeReqTranslators();

	void QuoteConnect(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void QuoteDisconnect(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void QuoteLogin(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void QuoteLogout(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void TradeConnect(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void TradeDisconnect(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void TradeLogin(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void TradeLogout(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	ReqTranslatorMap m_reqTransMap;
	ClientMap m_clients;
};

