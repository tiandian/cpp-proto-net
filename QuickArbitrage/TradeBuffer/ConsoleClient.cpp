#include "StdAfx.h"
#include "ConsoleClient.h"
#include "LogManager.h"
#include "OrderManager.h"
#include "protobuf_gen/trade.pb.h"

#include <boost/uuid/uuid_io.hpp>
#include <boost/foreach.hpp>

using namespace std;

extern CLogManager logger;
extern COrderManager g_orderMgr;

CConsoleClient::CConsoleClient(void):
	m_streamOut(std::stringstream::out), m_pRemoteClient(NULL)
{
}


CConsoleClient::~CConsoleClient(void)
{
}

void CConsoleClient::ProcessQuote( CTP::Quote* pQuote )
{
	m_streamOut.str("");
	m_streamOut << pQuote->symbol() << ", ";
	m_streamOut << pQuote->last() << ", ";
	m_streamOut << pQuote->update_time() << endl;

	logger.Info(m_streamOut.str());
}

void CConsoleClient::OnRegisterResult( bool succ, std::string& errMsg )
{
	ClientBase::OnRegisterResult(succ, errMsg);

	// forward the response to real remote client if existing, but don't need to forward login result
	//if(m_pRemoteClient != NULL)
	//	m_pRemoteClient->OnRegisterResult(succ, errMsg);

	m_streamOut.str("");
	m_streamOut << "Logged in to trading front: " << succ ? "Succeeded" : "Failed";
	m_streamOut << " (" << errMsg << ")" << endl;

	logger.Info(m_streamOut.str());
}

bool CConsoleClient::Login( const std::string& brokerId, const std::string& userId, const std::string& password )
{
	ClientBase* pRemoteClient = g_orderMgr.GetCurrentClient();
	if(pRemoteClient != this)
	{
		m_pRemoteClient = pRemoteClient;
		return ClientBase::Login(brokerId, userId, password);
	}
	else
	{
		return false;
	}
}

void CConsoleClient::Logout()
{
	ClientBase* pRemoteClient = g_orderMgr.GetCurrentClient();
	if(pRemoteClient != this)
	{
		cout << "Didn't I log in?" << endl;
	}
	else if(m_pRemoteClient == NULL)
	{
		cout << "I did REALLY log in trading front, now I wanna log out" << endl;
		ClientBase::Logout();
	}
	else 
	{
		cout << "I did just JOIN remote client, now I wanna UNJOIN" << endl;
		g_orderMgr.SetCurrentClient(m_pRemoteClient);
		m_pRemoteClient = NULL;
		m_tradeLoggedin = false;
	}
}

void CConsoleClient::Buy()
{
	g_orderMgr.Buy();
}

void CConsoleClient::AddPortfolio(double longPrice, double shortPrice)
{
	CPortfolio* pPortfolio = new CPortfolio();
	pPortfolio->SetQuantity(1);

	// long cu1206
	CLeg* pLeg = pPortfolio->AddLeg();
	pLeg->SetSymbol(std::string("cu1206"));
	pLeg->SetSide(protoc::SHORT);
	pLeg->SetRatio(1);
	pLeg->SetOpenOrderPriceType(longPrice > 0 ? protoc::LIMIT_PRICE : protoc::ANY_PRICE);
	pLeg->SetOpenLimitPrice(longPrice);

	// short cu1207
	pLeg = pPortfolio->AddLeg();
	pLeg->SetSymbol(std::string("cu1207"));
	pLeg->SetSide(protoc::LONG);
	pLeg->SetRatio(1);
	pLeg->SetOpenOrderPriceType(shortPrice > 0 ? protoc::LIMIT_PRICE : protoc::ANY_PRICE);
	pLeg->SetOpenLimitPrice(shortPrice);

	g_orderMgr.AddPortfolio(pPortfolio);
}

void CConsoleClient::OpenPosition(int portIdx, int legIdx)
{
	boost::uuids::uuid uidPortfolio;
	if(GetPortfolioIDByIndex(portIdx, &uidPortfolio))
	{
		cout << "Open position for portfolio :" << uidPortfolio << endl;
		g_orderMgr.Portfolio_OpenPosition(uidPortfolio);
	}
}

void CConsoleClient::ShowPortfolio()
{
	const PortfolioVector& portfolios = g_orderMgr.GetPortfolios();
	// print all portfolios with legs
	int idx = 0;
	BOOST_FOREACH(const boost::shared_ptr< CPortfolio >& portfo, portfolios)
	{
		cout << "Idx:" << setw(2) << ++idx << " Portfolio ID: " << portfo->GetID();
		cout << setw(7) << "Qty: " << portfo->GetQuantity() << "  Diff: " << portfo->GetDiff() << endl;
		cout << "IsAuto: " << (portfo->GetIsAuto() ? "Yes" : "No") << "  Entry Trigger Params: ";
		TriggerParamMap params;
		portfo->GetEntryTrigger()->GetParameters(params);
		for (TriggerParamMapIter it = params.begin(); it != params.end(); ++it)
		{
			cout << it->first << "=" << it->second << " ";
		}
		cout << endl;

		const LegVector& legs = portfo->GetLegs();
		int i = 0;
		BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, legs)
		{
			cout << "leg " << ++i << ": " ;
			cout << setw(10) << leg->GetSymbol() << setw(10) << leg->GetQuantity();
			cout << setw(10) << leg->GetSideText() << setw(15) << leg->GetStatusText();
			cout << setw(20) << leg->GetOrderSubmitStatusText() << setw(30) << leg->GetOrderStatusText();
			cout << setw(10) << leg->GetOpenLimitPrice() << setw(10) << leg->GetCost() << setw(10) << leg->GetCloseLimitPrice(); 
			cout << "    " << leg->GetStatusMessage();
			cout << endl;
		}
	}
}

void CConsoleClient::ClosePosition(int portIdx, int legIdx)
{
	boost::uuids::uuid uidPortfolio;
	if(GetPortfolioIDByIndex(portIdx, &uidPortfolio))
	{
		cout << "Close position for portfolio :" << uidPortfolio << endl;
		g_orderMgr.Portfolio_ClosePosition(uidPortfolio);
	}
}

void CConsoleClient::SetLegDirection( int portIdx, int legIdx, protoc::PosiDirectionType side )
{
	boost::uuids::uuid uidPortfolio;
	if(GetPortfolioIDByIndex(portIdx, &uidPortfolio))
	{
		CPortfolio* port = g_orderMgr.GetPortfolio(uidPortfolio);
		const LegVector& legs = port->GetLegs();
		if( legIdx <= legs.size() && legIdx > 0)
		{
			const boost::shared_ptr<CLeg>& l = legs[legIdx - 1];
			l->SetSide(side);
		}
	}
}

void CConsoleClient::SetLegOrdPriceType( int portIdx, int legIdx, protoc::OrderPriceTypeType priceType )
{
	boost::uuids::uuid uidPortfolio;
	if(GetPortfolioIDByIndex(portIdx, &uidPortfolio))
	{
		CPortfolio* port = g_orderMgr.GetPortfolio(uidPortfolio);
		const LegVector& legs = port->GetLegs();
		if( legIdx <= legs.size() && legIdx > 0 )
		{
			const boost::shared_ptr<CLeg>& l = legs[legIdx - 1];
			if (l->GetStatus() == UNOPENED)
			{
				l->SetOpenOrderPriceType(priceType);
			}
			else if(l->GetStatus() == OPENED)
			{
				l->SetCloseOrderPriceType(priceType);
			}
		}
	}
}

void CConsoleClient::SetLegLmtPrice( int portIdx, int legIdx, double limitPrice )
{
	boost::uuids::uuid uidPortfolio;
	if(GetPortfolioIDByIndex(portIdx, &uidPortfolio))
	{
		CPortfolio* port = g_orderMgr.GetPortfolio(uidPortfolio);
		const LegVector& legs = port->GetLegs();
		if( legIdx <= legs.size() && legIdx > 0 )
		{
			const boost::shared_ptr<CLeg>& l = legs[legIdx - 1];
			if (l->GetStatus() == UNOPENED)
			{
				l->SetOpenLimitPrice(limitPrice);
			}
			else if(l->GetStatus() == OPENED)
			{
				l->SetCloseLimitPrice(limitPrice);
			}
		}
	}
}

void CConsoleClient::CancelPortfolio(int portIdx, int legIdx)
{
	boost::uuids::uuid uidPortfolio;
	if(GetPortfolioIDByIndex(portIdx, &uidPortfolio))
	{
		cout << "Close position for portfolio :" << uidPortfolio << endl;
		g_orderMgr.Portfolio_CancelLegOrder(uidPortfolio, legIdx);
	}
}

bool CConsoleClient::GetPortfolioIDByIndex( int idx, boost::uuids::uuid* outPID )
{
	CPortfolio* pPort = g_orderMgr.GetPortfolio(idx - 1);
	if(pPort != NULL)
	{
		*outPID = pPort->GetID();
		return true;
	}
	else
	{
		cout << "Input portfolio index(" << idx << ") beyond boundary." << endl;
		return false;
	}
}

void CConsoleClient::SetPortfolioAuto( int portIdx, bool isAuto )
{
	boost::uuids::uuid uidPortfolio;
	if(GetPortfolioIDByIndex(portIdx, &uidPortfolio))
	{
		CPortfolio* port = g_orderMgr.GetPortfolio(uidPortfolio);
		port->SetAuto(isAuto);
	}
}

void CConsoleClient::SetPortfolioParams( int portIdx, const string& key, const string& value )
{
	boost::uuids::uuid uidPortfolio;
	if(GetPortfolioIDByIndex(portIdx, &uidPortfolio))
	{
		CPortfolio* port = g_orderMgr.GetPortfolio(uidPortfolio);
		TriggerParamMap params;
		params[key] = value;
		CTrigger* trigger = port->GetEntryTrigger();
		if(trigger != NULL)
		{
			trigger->SetParameters(params);
		}
	}
}


