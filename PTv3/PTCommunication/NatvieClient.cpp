#include "StdAfx.h"
#include "NatvieClient.h"
#include "ProtocolIDs.h"
#include "ResponseHandlers.h"
#include "ClientProtocol.h"
#include "ServerLoginReturn.h"
#include "ProtobufPacket.h"


CNatvieClient::CNatvieClient(PTCommunication::IClientRequestHandler ^reqHandler)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	setProtocol(new CClientProtocol);

	registerHandler(LoginResponseID, new LoginResponseHandler(this));
	registerHandler(LoginPuzzleResponseID, new LoginPuzzleResponseHandler(this));
	registerHandler(ServerLoginResponseID, new ServerLoginResponseHandler(this));
	registerHandler(PortfolioUpdateResponseID, new PortfolioUpdateResponseHandler(this));

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

bool CNatvieClient::ServerLogin( entity::ServerType svrType, const char* address, const char* brokerId, const char* investorId, const char* password )
{
	ProtobufPacket<entity::ServerLoginRequest> request(ServerLoginRequestID);
	request.getData().set_type(svrType);
	request.getData().set_address(address);
	request.getData().set_brokerid(brokerId);
	request.getData().set_userid(investorId);
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

void CNatvieClient::PortfEnableStrategy( const char* portfId, bool isEnabled )
{
	ProtobufPacket<entity::SwitchPortfolioRequest> request(PortfolioSwitchRequestID);
	request.getData().set_pid(portfId);
	request.getData().set_switchtype(entity::STRATEGY_SWITCH);
	request.getData().set_startstrategy(isEnabled);

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

void CNatvieClient::ApplyStrategySettings( const char* portfId, PTEntity::StrategyItem ^strategyItem )
{
	ProtobufPacket<entity::ApplyStrategySettingsRequest> request(ApplyStrategySetttingRequestID);
	request.getData().set_pid(portfId);
	entity::StrategyItem* pNativeItem = request.getData().mutable_strategy();
	strategyItem->To(pNativeItem);

	sendRequest(&request);
}

void CNatvieClient::OnPortfolioUpdateResponse( entity::PortfolioUpdateItem& resp )
{
	msclr::auto_gcroot<PortfolioUpdateItem^> updateItem = gcnew PortfolioUpdateItem(&resp);
	m_clr->OnPortfolioUpdate(updateItem.get());
}



