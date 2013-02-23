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

	virtual bool OnConnected(Session* session, const string& clientId, bool attach);

	virtual void OnDisconnected(Session* session);

	virtual void OnError(Session* session, const string& errorMsg);

	virtual void DispatchPacket(const string& sessionId, 
		const string& method, const string& in_data, string& out_data);

	virtual bool VerifyClient(const string& username, const string& password, bool* clientExisting);

	CClientAgent* GetClient(const string& sessionId);
	CClientAgent* GetClientById(const string& clientId);

private:
	void InitializeReqTranslators();
	void AttachSession(const string& clientId, Session* session);

	void QuoteConnect(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void QuoteDisconnect(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void QuoteLogin(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void QuoteLogout(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void RegQuote(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void TradeConnect(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void TradeDisconnect(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void TradeLogin(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void TradeLogout(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void AddPorf(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void AddPortfCollection(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void RemovePorf(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void PorfOpenPosition(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void PorfClosePosition(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void SwitchPosition(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void ScalperOpenPosition(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void ClosePosition(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void CancelOrder(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void ManualCloseOrder(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void QuerySymbolInfo(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void PortfChgQuantity(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void PortfEnableStrategy(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void PortfSetPreferredLeg(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void PortfTurnSwitches(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void ApplyStrategySettings(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	void QueryAccountInfo(CClientAgent* pClientAgent, const string& in_data, string& out_data);
	void QueryPositionDetails(CClientAgent* pClientAgent, const string& in_data, string& out_data);

	ReqTranslatorMap m_reqTransMap;
	ClientMap m_clients;
};

