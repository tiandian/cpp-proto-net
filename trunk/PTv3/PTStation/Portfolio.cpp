#include "StdAfx.h"
#include "Portfolio.h"
#include "ArbitrageStrategy.h"
#include "ChangePositionStrategy.h"
#include "ScalperStrategy.h"
#include "globalmembers.h"
#include "QuoteFetcher.h"
#include "QuoteRepositry.h"
#include "AvatarClient.h"
#include "PortfolioScalperOrderPlacer.h"
#include "charsetconvert.h"


CPortfolio::CPortfolio(CAvatarClient* client, const entity::PortfolioItem& srcPortfolioItem)
	: m_avatar(client)
	, m_pQuoteRepo(NULL)
	, m_strategyType(entity::ARBITRAGE)
	, m_serialOrderId(0)
	, m_openTimes(0)
	, m_totalOpenTimes(0)
	, m_totalCloseTimes(0)
	, m_currentPosition(0)
	, m_cancelTimes(0)
	, m_profit(0)
	, m_avgCost(0)
	, m_maxOpenPerStart(30)
	, m_maxCancel(100)
	, m_totalOpenLimit(80)
{
	// Backup created portfolio item
	m_portfolioItem.CopyFrom(srcPortfolioItem);

	InitOpenCancelLimit(srcPortfolioItem);


	// Initialize portfolio update item
	m_portfolioUpdate.set_id(m_portfolioItem.id());
	m_portfolioUpdate.set_totalopentimes(0);
	m_portfolioUpdate.set_totalclosetimes(0);
	m_portfolioUpdate.set_currentposition(0);
	m_portfolioUpdate.set_canceltimes(0);

	// Create legs, as well as leg update item in portfolio update item
	for(int i = 0; i < srcPortfolioItem.legs_size(); ++i)
	{
		AddLeg(srcPortfolioItem.legs(i));
	}

	// Initialize strategy
	m_strategy = CreateStrategy(srcPortfolioItem.strategy());
	assert(m_orderPlacer.get() != NULL);
	m_orderPlacer->Initialize(this, &(client->OrderProcessor()));

	// Initialize strategy type in update item
	m_portfolioUpdate.set_strategy(m_strategyType);
	// Initialize trigger status in update item
	PrepareTriggerUpdate();
}


CPortfolio::~CPortfolio(void)
{
	Cleanup();
}

void CPortfolio::Cleanup()
{
	if(m_pQuoteRepo != NULL)
	{
		BOOST_FOREACH(CQuoteFetcher* pFetcher, m_quoteFetcherVec)
		{
			m_pQuoteRepo->DestoryFetcher(pFetcher);
		}
		m_quoteFetcherVec.clear();
	}
}

void CPortfolio::AddLeg( const entity::LegItem& legItem )
{
	// prepare legs in portfolio update item
	entity::LegUpdateItem* legUpdate = m_portfolioUpdate.add_legs();
	legUpdate->set_symbol(legItem.symbol());

	int legCount = (int)m_legs.size();
	// leg id is 1 based
	LegPtr leg(new CLeg(legCount + 1, legItem, legUpdate));
	m_legs.push_back(leg);
}

CLeg* CPortfolio::GetLeg( int legId )
{
	if(legId <= (int)m_legs.size())
		return m_legs[legId - 1].get();

	return NULL;
}

CLeg* CPortfolio::GetLeg(const string& symbol)
{
	// update last
	for(vector<LegPtr>::iterator iter = m_legs.begin(); iter != m_legs.end(); ++iter)
	{
		if((*iter)->Symbol() == symbol)
		{
			return iter->get();
		}
	}

	return NULL;
}

StrategyPtr CPortfolio::CreateStrategy( const entity::StrategyItem& strategyItem )
{
	m_strategyType = strategyItem.type();
	StrategyPtr created;
	switch(m_strategyType)
	{
	case entity::ARBITRAGE:
		created = StrategyPtr(new CArbitrageStrategy(strategyItem));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioOrderPlacer);
		break;
	case entity::CHANGE_POSITION:
		created = StrategyPtr(new CChangePositionStrategy(strategyItem));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioOrderPlacer);
		break;
	case entity::SCALPER:
		created = StrategyPtr(new CScalperStrategy(strategyItem));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioScalperOrderPlacer);
		break;
	}
	
	return created;
}

void CPortfolio::PrepareTriggerUpdate()
{
	const vector<TriggerPtr>& triggers = m_strategy->Triggers();
	for (vector<TriggerPtr>::const_iterator iter = triggers.begin(); iter != triggers.end(); ++iter)
	{
		entity::TriggerStatus* triggerStatus = m_portfolioUpdate.add_triggers();
		triggerStatus->set_strategy((*iter)->Strategy());
		triggerStatus->set_name((*iter)->Name());
		triggerStatus->set_enabled((*iter)->IsEnabled());
	}
}

void CPortfolio::SubscribeQuotes( CQuoteRepositry* pQuoteRepo )
{
	assert(pQuoteRepo != NULL);
	m_pQuoteRepo = pQuoteRepo;

	BOOST_FOREACH(const LegPtr& leg, m_legs)
	{
		const string& symbol = leg->Symbol();
		CQuoteFetcher* pFetcher = m_pQuoteRepo->CreateFetcher(symbol);
		m_quoteFetcherVec.push_back(pFetcher);
		pFetcher->Run(boost::bind(&CPortfolio::OnQuoteRecevied, this, _1, _2));
	}
}

void CPortfolio::OnQuoteRecevied( boost::chrono::steady_clock::time_point& timestamp, entity::Quote* pQuote )
{
	cout << "Quote incoming: " << pQuote->symbol() << ", " << pQuote->last() << ", " << pQuote->update_time() << endl; 
	// Update leg's last with income quote in CStrategy::Test
	m_strategy->Test(pQuote, this, timestamp);

	// Update PortfolioUpdateItem
	// 1. for LegUpdateItem
	GetLegUpdate();
	// 2. for different Strategy Update
	m_strategy->GetStrategyUpdate(&m_portfolioUpdate);
	// 3. for statistics
	GetStatisticsUpdate();
	// 4. finally publish PortfolioUpdateItem to client
	PushUpdate();
}

void CPortfolio::GetLegUpdate()
{
	// update last
	for(vector<LegPtr>::iterator iter = m_legs.begin(); iter != m_legs.end(); ++iter)
	{
		(*iter)->GetUpdated();
	}
}

int CPortfolio::NewOrderId( string& newId )
{
	boost::mutex::scoped_lock lock(m_mutStat);
	newId = boost::str(boost::format("%s-%d") % ID() % ++m_serialOrderId);
	return m_serialOrderId;
}

void CPortfolio::GetStatisticsUpdate()
{
	m_portfolioUpdate.set_totalopentimes(m_totalOpenTimes);
	m_portfolioUpdate.set_totalclosetimes(m_totalOpenTimes);
	m_portfolioUpdate.set_currentposition(m_currentPosition);
	m_portfolioUpdate.set_canceltimes(m_cancelTimes);
}

void CPortfolio::AddPosition( const trade::MultiLegOrder& openOrder )
{
	int openTimes = m_strategy->OnPortfolioAddPosition(this, openOrder);
}

void CPortfolio::RemovePosition( const trade::MultiLegOrder& closeOrder )
{
	int closeTimes = m_strategy->OnPortfolioRemovePosition(this, closeOrder);
}

void CPortfolio::PushUpdate()
{
	m_avatar->PublishPortfolioUpdate(m_portfolioUpdate);
}

void CPortfolio::StartStrategy(int lastOrderId)
{
	logger.Info(boost::str(boost::format("[%s] Portfolio (%s) START strategy >>>") % InvestorId() % ID()));
	m_openTimes = 0;
	m_serialOrderId = lastOrderId;
	m_orderPlacer->Prepare();
	m_strategy->Start();
}

void CPortfolio::StopStrategy()
{
	logger.Info(boost::str(boost::format("[%s] Portfolio (%s) STOP strategy <<<") % InvestorId() % ID()));
	m_strategy->Stop();
	m_orderPlacer->Cleanup();
}

void CPortfolio::EnableTrigger( int triggerIdx, bool enabled )
{
	assert(triggerIdx < (int)m_strategy->Triggers().size());
	logger.Info(boost::str(boost::format("[%s] %s trigger %d of Portfolio (%s)") 
		% InvestorId() % (enabled ? "ENABLE" : "DISABLE") % (triggerIdx + 1) % ID()));
	m_strategy->Triggers().at(triggerIdx)->Enable(enabled);
}

const string& CPortfolio::InvestorId()
{
	return m_avatar->Pseudo();
}

void CPortfolio::InitOpenCancelLimit( const entity::PortfolioItem &srcPortfolioItem )
{
	m_maxOpenPerStart = srcPortfolioItem.maxopenperstart();
	m_maxCancel = srcPortfolioItem.maxcancel();
	m_totalOpenLimit = srcPortfolioItem.totalopenlimit();
}

void CPortfolio::CheckOpenCancelLimit()
{
	string msg;
	if(m_openTimes >= m_maxOpenPerStart)
	{
		msg = boost::str(boost::format("本次策略开仓%d手已达到上限%d") % m_openTimes % m_maxOpenPerStart);
		StopStrategy();
	}
	else if(m_cancelTimes >= m_maxCancel)
	{
		msg = boost::str(boost::format("撤单%d手已达到上限%d") % m_cancelTimes % m_maxCancel);
		StopStrategy();
	}
	else if(m_totalOpenTimes >= m_totalOpenLimit)
	{
		msg = boost::str(boost::format("总开仓%d手已达到上限%d") % m_totalOpenTimes % m_totalOpenLimit);
		StopStrategy();
	}

	if(msg.length() > 0)
	{
		m_portfolioUpdate.set_running(false);
		string utf8Msg;
		GB2312ToUTF_8(utf8Msg, msg.c_str());
		m_portfolioUpdate.set_message(utf8Msg);
		PushUpdate();
		m_portfolioUpdate.clear_message();
	}
}

void CPortfolio::UpdatePosition()
{
	int posiQty = m_totalOpenTimes - m_totalCloseTimes;
	if(posiQty < 0)
		posiQty = 0;
	m_currentPosition = posiQty;
}
