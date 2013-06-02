#include "StdAfx.h"
#include "ClientManager.h"
#include "globalmembers.h"
#include "../Entity/gen/cpp/message.pb.h"
#include "charsetconvert.h"

#include <boost/format.hpp>
#include <boost/bind.hpp>

CClientManager::CClientManager(void)
{
	InitializeReqTranslators();
}


CClientManager::~CClientManager(void)
{
}

bool CClientManager::OnConnected( Session* session, const string& clientId, bool attach )
{
	boost::recursive_mutex::scoped_lock lock(m_mutClntMap);
	if(attach)
	{
		CClientAgent* pClientAgent = GetClientById(clientId);
		_ASSERT(pClientAgent != NULL);
		if(pClientAgent != NULL)
		{
			if(pClientAgent->Detached())
			{
				AttachSession(clientId, session);
				logger.Info(boost::str(boost::format("Client(%s) Attached.") % clientId ));
				return true;
			}
		}
	}
	else
	{
		std::string info = boost::str(boost::format("Client(%s) connected.") % session->SessionId().c_str());
		logger.Info(info);

		ClientPtr client(new CClientAgent(clientId));
		client->SetSession(session);
		m_clients.insert(std::make_pair(session->SessionId(), client));
		return true;
	}

	return false;
}

void CClientManager::OnDisconnected( Session* session )
{
	std::string info = boost::str(boost::format("Client(%s) disconnected.") % session->SessionId().c_str());
	logger.Info(info);
	
	const string& sessionId = session->SessionId();
	CClientAgent* pClntAgent = GetClient(sessionId);
	if(pClntAgent != NULL)
	{
		boost::recursive_mutex::scoped_lock lock(m_mutClntMap);
		pClntAgent->SetSession(NULL);
		if(!pClntAgent->IsConnected())
			m_clients.erase(sessionId);
	}
}

void CClientManager::OnError( Session* session, const string& errorMsg )
{
	std::string outputErr = boost::str(boost::format("Client(%s) encounter error - %s.") 
		% session->SessionId().c_str() % errorMsg.c_str());
	logger.Info(outputErr);
}

void CClientManager::DispatchPacket( const string& sessionId, const string& method, const string& in_data, string& out_data )
{
	CClientAgent* pClntAgent = GetClient(sessionId);
	if(pClntAgent != NULL)
	{
		ReqTranslatorMapIter transIter = m_reqTransMap.find(method);
		if(transIter != m_reqTransMap.end())
		{
			(transIter->second)(pClntAgent, in_data, out_data);
		}
	}
}

CClientAgent* CClientManager::GetClient( const string& sessionId )
{
	boost::recursive_mutex::scoped_lock lock(m_mutClntMap);
	ClientMapIter clntIter = m_clients.find(sessionId);
	if(clntIter != m_clients.end())
	{
		return (clntIter->second).get();
	}
	else
		return NULL;
}

CClientAgent* CClientManager::GetClientById( const string& clientId )
{
	for (ClientMapIter clntIter = m_clients.begin(); clntIter != m_clients.end(); ++clntIter)
	{
		const string& currClientId = (clntIter->second)->ClientId();
		if(currClientId == clientId)
		{
			return (clntIter->second).get();
		}
	}
	return NULL;
}

void CClientManager::InitializeReqTranslators()
{
	m_reqTransMap.insert(make_pair("QuoteConnect", boost::bind(&CClientManager::QuoteConnect, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("QuoteDisconnect", boost::bind(&CClientManager::QuoteDisconnect, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("QuoteLogin", boost::bind(&CClientManager::QuoteLogin, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("QuoteLogout", boost::bind(&CClientManager::QuoteLogout, this, _1, _2, _3)));

	m_reqTransMap.insert(make_pair("RegQuote", boost::bind(&CClientManager::RegQuote, this, _1, _2, _3)));

	m_reqTransMap.insert(make_pair("TradeConnect", boost::bind(&CClientManager::TradeConnect, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("TradeDisconnect", boost::bind(&CClientManager::TradeDisconnect, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("TradeLogin", boost::bind(&CClientManager::TradeLogin, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("TradeLogout", boost::bind(&CClientManager::TradeLogout, this, _1, _2, _3)));

	m_reqTransMap.insert(make_pair("AddPortf", boost::bind(&CClientManager::AddPorf, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("AddPortfCollection", boost::bind(&CClientManager::AddPortfCollection, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("RemovePortf", boost::bind(&CClientManager::RemovePorf, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("PorfOpenPosition", boost::bind(&CClientManager::PorfOpenPosition, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("PorfClosePosition", boost::bind(&CClientManager::PorfClosePosition, this, _1, _2, _3)));

	m_reqTransMap.insert(make_pair("SwitchPosition", boost::bind(&CClientManager::SwitchPosition, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("ScalperOpenPosition", boost::bind(&CClientManager::ScalperOpenPosition, this, _1, _2, _3)));
	
	m_reqTransMap.insert(make_pair("ClosePosition", boost::bind(&CClientManager::ClosePosition, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("CancelOrder", boost::bind(&CClientManager::CancelOrder, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("ManualCloseOrder", boost::bind(&CClientManager::ManualCloseOrder, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("QuerySymbolInfo", boost::bind(&CClientManager::QuerySymbolInfo, this, _1, _2, _3)));
	
	m_reqTransMap.insert(make_pair("PortfChgQuantity", boost::bind(&CClientManager::PortfChgQuantity, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("PortfEnableStrategy", boost::bind(&CClientManager::PortfEnableStrategy, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("PortfSetPreferredLeg", boost::bind(&CClientManager::PortfSetPreferredLeg, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("PortfTurnSwitches", boost::bind(&CClientManager::PortfTurnSwitches, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("ApplyStrategySettings", boost::bind(&CClientManager::ApplyStrategySettings, this, _1, _2, _3)));

	m_reqTransMap.insert(make_pair("QueryAccountInfo", boost::bind(&CClientManager::QueryAccountInfo, this, _1, _2, _3)));
	m_reqTransMap.insert(make_pair("QueryPositionDetails", boost::bind(&CClientManager::QueryPositionDetails, this, _1, _2, _3)));
}

void CClientManager::QuoteConnect( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ConnectParam connParam;
	connParam.ParseFromString(in_data);

	boost::tuple<bool, string> ret = pClientAgent->QuoteConnect(connParam.quoteaddress(), connParam.streamfolder());

	entity::OperationReturn operRet;
	operRet.set_success(boost::get<0>(ret));
	operRet.set_errormessage(boost::get<1>(ret));

	operRet.SerializeToString(&out_data);
}

void CClientManager::QuoteDisconnect( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	pClientAgent->QuoteDisconnect();
}

void CClientManager::QuoteLogin( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::LoginParam loginParam;
	loginParam.ParseFromString(in_data);

	boost::tuple<bool, string> ret = pClientAgent->QuoteLogin(
			loginParam.brokerid(), loginParam.userid(), loginParam.password());

	entity::OperationReturn operRet;
	operRet.set_success(boost::get<0>(ret));
	operRet.set_errormessage(boost::get<1>(ret));

	operRet.SerializeToString(&out_data);
}

void CClientManager::QuoteLogout( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	pClientAgent->QuoteLogout();
}

void CClientManager::TradeConnect( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ConnectParam connParam;
	connParam.ParseFromString(in_data);

	boost::tuple<bool, string> ret = pClientAgent->TradeConnect(connParam.quoteaddress(), connParam.streamfolder());

	entity::OperationReturn operRet;
	operRet.set_success(boost::get<0>(ret));
	operRet.set_errormessage(boost::get<1>(ret));

	operRet.SerializeToString(&out_data);
}

void CClientManager::TradeDisconnect( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	pClientAgent->TradeDisconnect();
}

void CClientManager::TradeLogin( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::LoginParam loginParam;
	loginParam.ParseFromString(in_data);

	const entity::AccountSettings* pSettings = NULL;
	if(loginParam.has_acctsettings())
	{
		pSettings = &(loginParam.acctsettings());
	}

	boost::tuple<bool, string> ret = pClientAgent->TradeLogin(
		loginParam.brokerid(), loginParam.userid(), loginParam.password(), pSettings);

	entity::OperationReturn operRet;
	operRet.set_success(boost::get<0>(ret));
	string utf8Msg;
	GB2312ToUTF_8(utf8Msg, boost::get<1>(ret).c_str());
	operRet.set_errormessage(utf8Msg);

	operRet.SerializeToString(&out_data);
}

void CClientManager::TradeLogout( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	pClientAgent->TradeLogout();
}

void CClientManager::RegQuote( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::RegQuoteParam regParam;
	regParam.ParseFromString(in_data);

	vector<string> regSymbols(regParam.symbols().begin(), regParam.symbols().end());
	pClientAgent->RegQuote(regSymbols);
}

void CClientManager::AddPorf( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::PortfolioItem* portfItem = new entity::PortfolioItem;
	portfItem->ParseFromString(in_data);

	pClientAgent->Add(portfItem);
}

void CClientManager::AddPortfCollection(CClientAgent* pClientAgent, const string& in_data, string& out_data)
{
	entity::AddPortfolioParam addPorfParam;
	addPorfParam.ParseFromString(in_data);

	pClientAgent->AddPortfolios(addPorfParam);
}

void CClientManager::RemovePorf( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::StringParam strParam;
	strParam.ParseFromString(in_data);
	pClientAgent->Remove(strParam.data());
}

void CClientManager::PorfOpenPosition( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::PorfOpenPosiParam opParam;
	opParam.ParseFromString(in_data);

	bool isVirtual = opParam.isvirtual();
	if(isVirtual)
		pClientAgent->VirtualOpenPosition(opParam.portfid(), opParam.quantity());
	else
		pClientAgent->OpenPosition(opParam.portfid(), opParam.quantity());
}

void CClientManager::SwitchPosition( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::PorfOpenPosiParam opParam;
	opParam.ParseFromString(in_data);
	//pClientAgent->ChangePosition(opParam.portfid(), opParam.quantity());
}

void CClientManager::ScalperOpenPosition( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::PorfOpenPosiParam opParam;
	opParam.ParseFromString(in_data);
	pClientAgent->QuickScalpe(opParam.portfid(), opParam.quantity());
}

void CClientManager::PorfClosePosition(CClientAgent* pClientAgent, const string& in_data, string& out_data)
{
	entity::PorfOpenPosiParam opParam;
	opParam.ParseFromString(in_data);
	const string& portfId = opParam.portfid();
	int qty = opParam.quantity();
	if(qty > 0)
	{
		bool isVirtual = opParam.isvirtual();
		if(isVirtual)
			pClientAgent->VirtualClosePosition(portfId, qty);
		else
			pClientAgent->ClosePosition(portfId, qty, trade::SR_Manual);
	}
	else
		pClientAgent->SimpleCloseOrderPosition(portfId, trade::SR_Manual);
}

void CClientManager::ClosePosition( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ClosePositionParam cpParam;
	cpParam.ParseFromString(in_data);

	const trade::MultiLegOrder& openMlOrder = cpParam.multilegorder();

	string message;
	pClientAgent->ClosePosition(openMlOrder, cpParam.legordref(), trade::SR_Manual, message);
	entity::StringParam retParam;
	retParam.set_data(message);
	retParam.SerializeToString(&out_data);
}

void CClientManager::QueryAccountInfo( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	pClientAgent->QueryAccountInfo(&out_data);
}

void CClientManager::ApplyStrategySettings( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ModifyStrategyParam modifyParam;
	modifyParam.ParseFromString(in_data);

	pClientAgent->ApplyStrategySetting(modifyParam);
}

void CClientManager::PortfSetPreferredLeg( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ModifyPortfolioPreferredLegParam legParam;
	legParam.ParseFromString(in_data);

	pClientAgent->SetPorfPreferredLeg(legParam);
}

void CClientManager::PortfTurnSwitches( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ModifyPortfolioSwitchParam switchesParam;
	switchesParam.ParseFromString(in_data);

	pClientAgent->TurnPortfSwitches(switchesParam);
}

void CClientManager::PortfEnableStrategy( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ModifyRunningStatusParam runningParam;
	runningParam.ParseFromString(in_data);

	pClientAgent->EnableStrategy(runningParam);
}

void CClientManager::CancelOrder( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::CancelOrderParam cancelParam;
	cancelParam.ParseFromString(in_data);

	pClientAgent->CancelOrder(cancelParam);
}

void CClientManager::QueryPositionDetails( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::StringParam stringParam;
	stringParam.ParseFromString(in_data);

	pClientAgent->QueryPositionDetails(stringParam.data());
}

void CClientManager::ManualCloseOrder( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ManualCloseOrderParam closeOrdParam;
	closeOrdParam.ParseFromString(in_data);

	boost::tuple<bool, string> ret = pClientAgent->ManualCloseOrder(
		closeOrdParam.symbol(), closeOrdParam.direction(), closeOrdParam.opendate(), closeOrdParam.quantity());

	entity::OperationReturn operRet;
	operRet.set_success(boost::get<0>(ret));
	operRet.set_errormessage(boost::get<1>(ret));

	operRet.SerializeToString(&out_data);
}

void CClientManager::QuerySymbolInfo( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::RegQuoteParam symbolParam;
	symbolParam.ParseFromString(in_data);
	int count = symbolParam.symbols_size();
	if(count > 0)
	{
		const string& symbol = symbolParam.symbols(0);
		entity::SymbolInfo* pRetSymbInfo = NULL;
		pClientAgent->QuerySymbolInfo(symbol, &pRetSymbInfo);
		if(pRetSymbInfo != NULL)
		{
			pRetSymbInfo->SerializeToString(&out_data);
		}
	}
}

void CClientManager::PortfChgQuantity( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ModifyPortfolioQtyParam qtyParam;
	qtyParam.ParseFromString(in_data);

	pClientAgent->SetPortfolioQuantity(qtyParam.portfid(), qtyParam.onceqty(), qtyParam.maxqty());
}

bool CClientManager::VerifyClient( const string& username, const string& password, bool* clientExisting )
{
	boost::recursive_mutex::scoped_lock lock(m_mutClntMap);
	*clientExisting = false;
	CClientAgent* pClient = GetClientById(username);
	if(pClient == NULL)
		return true;
	else
	{
		*clientExisting = true;

		if(pClient->Detached())
		{
			return true;
		}
		return false;
	}
}

void CClientManager::AttachSession( const string& clientId, Session* session )
{
	boost::recursive_mutex::scoped_lock lock(m_mutClntMap);
	ClientPtr targetClient;
	bool found = false;
	for (ClientMapIter clntIter = m_clients.begin(); clntIter != m_clients.end(); ++clntIter)
	{
		const string& currClientId = (clntIter->second)->ClientId();
		if(currClientId == clientId)
		{
			targetClient = (clntIter->second);
			found = true;
			m_clients.erase(clntIter);
			break;
		}
	}

	if(found)
	{
		targetClient->SetSession(session);
		m_clients.insert(std::make_pair(session->SessionId(), targetClient));
	}
}
