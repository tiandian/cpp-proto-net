#include "StdAfx.h"
#include "NatvieClient.h"
#include "ProtocolIDs.h"
#include "ResponseHandlers.h"
#include "ClientProtocol.h"
#include "ServerLoginReturn.h"
#include "ProtobufPacket.h"
#include "MultiLegOrder.h"
#include "TradeUpdate.h"
#include "Order.h"

using namespace System;
using namespace System::Runtime::InteropServices;

CNatvieClient::CNatvieClient(PTCommunication::IClientRequestHandler ^reqHandler)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	setProtocol(new CClientProtocol);

	registerHandler(LoginResponseID, new LoginResponseHandler(this));
	registerHandler(LoginPuzzleResponseID, new LoginPuzzleResponseHandler(this));
	registerHandler(ServerLoginResponseID, new ServerLoginResponseHandler(this));
	registerHandler(PortfolioUpdateResponseID, new PortfolioUpdateResponseHandler(this));
	registerHandler(MultilegOrderResponseID, new MultilegOrderUpdateResponseHandler(this));
	registerHandler(LegOrderResponseID, new LegOrderUpdateResponseHandler(this));
	registerHandler(TradeResponseID, new TradeUpdateResponseHandler(this));
	registerHandler(HeartbeatResponseID, new HeartbeatResponseHandler(this));

	m_clr = reqHandler;
}


CNatvieClient::~CNatvieClient(void)
{
}

void CNatvieClient::onConnected()
{

}

void CNatvieClient::onConnectionClosed()
{
	// for now, don't consider reconnect, just clear session id in case connection closed
	m_sessionId.clear();
	m_clr->OnDisconnected();
}

void CNatvieClient::OnLoginResponse( entity::LoginResponse& resp )
{
	if(resp.accepted())
	{
		m_sessionId = resp.session_id();
	}

	msclr::auto_gcroot<LoginReturn^> loginRet = gcnew LoginReturn(&resp);
	m_clr->OnLoginReturned(loginRet.get());
}

void CNatvieClient::OnLoginPuzzleResponse( entity::LoginPuzzleResponse& resp )
{
	tryLogin(resp.session_id());
}

void CNatvieClient::OnServerLoginResponse( entity::ServerLoginResponse& resp )
{
	 msclr::auto_gcroot<ServerLoginReturn^> loginRet = gcnew ServerLoginReturn();
	 loginRet->From(&resp);
	 m_clr->OnServerLoginReturned(loginRet.get());
}

bool CNatvieClient::tryLogin(const string& sessionId)
{
	if (getStatus() == Disconnected)
	{
		return false;
	}

	if (getStatus() != Connected)
	{
		return false;
	}

	ProtobufPacket<entity::LoginRequest> request(LoginRequestID);
	
	request.getData().set_is_new(true);
	request.getData().set_session_id(sessionId);
	request.getData().set_previous_session_id("");
	request.getData().set_pseudo(m_pseudo);

	return sendRequest(&request) > 0;
}

bool CNatvieClient::logged()
{
	return getStatus() == Connected && m_sessionId.length() > 0;
}

void CNatvieClient::Logout()
{
	m_sessionId.clear();
	disconnect(true);
}

bool CNatvieClient::ServerLogin(entity::ServerType svrType, const char* address, const char* brokerId, const char* investorId, const char* userId, const char* password)
{
	ProtobufPacket<entity::ServerLoginRequest> request(ServerLoginRequestID);
	request.getData().set_type(svrType);
	request.getData().set_address(address);
	request.getData().set_brokerid(brokerId);
	request.getData().set_investorid(investorId);
	request.getData().set_userid(userId);
	request.getData().set_password(password);

	return sendRequest(&request) > 0;
}

void CNatvieClient::ServerLogout( entity::ServerType svrType )
{
	ProtobufPacket<entity::ServerLogoutRequest> request(ServerLogoutRequestID);
	request.getData().set_type(svrType);

	sendRequest(&request);
}

void CNatvieClient::AddPortfolio( PTEntity::PortfolioItem ^portfolioItem )
{
	ProtobufPacket<entity::AddPortfolioRequest> request(AddPortfolioRequestID);
	entity::PortfolioItem* pAddedPortfItem = request.getData().add_portfolios();
	portfolioItem->To(pAddedPortfItem);
	
	sendRequest(&request);
}

void CNatvieClient::AddPortfolios( array<PTEntity::PortfolioItem^> ^portfolioItems )
{
	ProtobufPacket<entity::AddPortfolioRequest> request(AddPortfolioRequestID);
	for each(PTEntity::PortfolioItem ^portf in portfolioItems)
	{
		entity::PortfolioItem* pAddedPortfItem = request.getData().add_portfolios();
		portf->To(pAddedPortfItem);
	}

	sendRequest(&request);
}

void CNatvieClient::PortfEnableStrategy( const char* portfId, bool isEnabled, int lastOrderId )
{
	ProtobufPacket<entity::SwitchPortfolioRequest> request(PortfolioSwitchRequestID);
	request.getData().set_pid(portfId);
	request.getData().set_switchtype(entity::STRATEGY_SWITCH);
	request.getData().set_startstrategy(isEnabled);
	if(isEnabled)
		request.getData().set_lastorderid(lastOrderId);

	sendRequest(&request);
}

void CNatvieClient::PortfTurnSwitches( const char* portfId, int triggerIndex, bool enabled )
{
	ProtobufPacket<entity::SwitchPortfolioRequest> request(PortfolioSwitchRequestID);
	request.getData().set_pid(portfId);
	request.getData().set_switchtype(entity::TRIGGER_SWITCH);

	request.getData().set_triggerindex(triggerIndex);
	request.getData().set_enabletrigger(enabled);

	sendRequest(&request);
}


void CNatvieClient::PortfChangeArbitrage(const char* portfId, bool isArbitrage)
{
	ProtobufPacket<entity::SwitchPortfolioRequest> request(PortfolioSwitchRequestID);
	request.getData().set_pid(portfId);
	request.getData().set_switchtype(entity::ARBITRAGE_SWITCH);

	request.getData().set_isarbitrage(isArbitrage);
	
	sendRequest(&request);
}

void CNatvieClient::ApplyStrategySettings( const char* portfId, PTEntity::StrategyItem ^strategyItem )
{
	ProtobufPacket<entity::ApplyStrategySettingsRequest> request(ApplyStrategySetttingRequestID);
	request.getData().set_pid(portfId);
	entity::StrategyItem* pNativeItem = request.getData().mutable_strategy();
	strategyItem->To(pNativeItem);

	sendRequest(&request);
}

void CNatvieClient::PortfChangePreferredLeg(const char* portfId, const char* legName)
{
	ProtobufPacket<entity::ModifyPortfolioPreferredLegParam> request(PortfolioModifyPreferredLegRequestID);
	request.getData().set_portfid(portfId);
	request.getData().set_legsymbol(legName);

	sendRequest(&request);
}

void CNatvieClient::PortfModifyQuantity(const char* portfId, int perOpenQty, int perStartQty, int totalOpenLimit, int maxCancelQty, vector<string>& endTimePointsVec)
{
	ProtobufPacket<entity::ModifyPortfolioQtyParam> request(PortfolioModifyQtyRequestID);
	request.getData().set_portfid(portfId);
	request.getData().set_peropenqty(perOpenQty);
	request.getData().set_perstartqty(perStartQty);
	request.getData().set_totalopenlimit(totalOpenLimit);
	request.getData().set_maxcancelqty(maxCancelQty);
	
	for(vector<string>::iterator iter = endTimePointsVec.begin(); iter != endTimePointsVec.end(); ++iter)
	{
		request.getData().add_endtimepoints(*iter);
	}

	sendRequest(&request);
}

void CNatvieClient::PortfOpenPosition(const char* portfId)
{
	ProtobufPacket<entity::PortfOpenPosiParam> request(PortfolioOpenPositionRequestID);
	request.getData().set_portfid(portfId);

	sendRequest(&request);
}
void CNatvieClient::PortfClosePosition(const char* portfId)
{
	ProtobufPacket<entity::ClosePositionParam> request(PortfolioClosePositionRequestID);
	request.getData().set_portfid(portfId);

	sendRequest(&request);
}

void CNatvieClient::OnPortfolioUpdateResponse( entity::PortfolioUpdateItem& resp )
{
	msclr::auto_gcroot<PortfolioUpdateItem^> updateItem = gcnew PortfolioUpdateItem(&resp);
	m_clr->OnPortfolioUpdate(updateItem.get());
}

void CNatvieClient::OnMultilegOrderUpdateResponse( trade::MultiLegOrder& resp )
{
	msclr::auto_gcroot<MultiLegOrder^> mlOrderItem = gcnew MultiLegOrder(&resp);
	m_clr->OnMultiLegOrderUpdate(mlOrderItem.get());
}

void CNatvieClient::OnLegOrderUpdateResponse( entity::LegOrderUpdateParam& legOrderUpdateParam )
{
	msclr::auto_gcroot<String^> portfId = Marshal::PtrToStringAnsi((IntPtr)(char*)legOrderUpdateParam.portfid().c_str());
	msclr::auto_gcroot<String^> mlOrderId = Marshal::PtrToStringAnsi((IntPtr)(char*)legOrderUpdateParam.multilegorderid().c_str());
	msclr::auto_gcroot<String^> legOrderRef = Marshal::PtrToStringAnsi((IntPtr)(char*)legOrderUpdateParam.legorderref().c_str());
	msclr::auto_gcroot<Order^> legOrder = gcnew Order(&(legOrderUpdateParam.legorder()));
	
	m_clr->OnLegOrderUpdate(portfId.get(), mlOrderId.get(), legOrderRef.get(), legOrder.get());
}

void CNatvieClient::OnTradeUpdateResponse( trade::Trade& resp )
{
	msclr::auto_gcroot<TradeUpdate^> tradeItem = gcnew TradeUpdate(&resp);
	m_clr->OnTradeUpdate(tradeItem.get());
}

void CNatvieClient::OnHeartbeatResponse( const char* timestamp )
{
	msclr::auto_gcroot<String^> timestampStr = Marshal::PtrToStringAnsi((IntPtr)(char*)timestamp);
	m_clr->OnHeartbeat(timestampStr.get());
}

void CNatvieClient::SendHeartbeat(const char* timestamp)
{
	ProtobufPacket<entity::HeartbeatRequest> request(HeartbeatRequestID);
	request.getData().set_timestamp(timestamp);
	sendRequest(&request);
}



