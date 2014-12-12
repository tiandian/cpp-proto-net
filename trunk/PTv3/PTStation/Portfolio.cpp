#include "StdAfx.h"
#include "Portfolio.h"
#include "ArbitrageStrategy.h"
#include "ChangePositionStrategy.h"
#include "ScalperStrategy.h"
#include "HistSlopeStrategy.h"
#include "WMATrendStrategy.h"
#include "LinerRegressionStrategy.h"
#include "ASCTrendStrategy.h"
#include "RangeTrendStrategy.h"
#include "globalmembers.h"
#include "QuoteFetcher.h"
#include "QuoteRepositry.h"
#include "AvatarClient.h"
#include "PortfolioScalperOrderPlacer.h"
#include "PortfolioTrendOrderPlacer.h"
#include "PortfolioArbitrageOrderPlacer.h"
#include "charsetconvert.h"
#include "SymbolTimeUtil.h"

#include <algorithm>
#include <boost/algorithm/string.hpp>

double CalcDiff(vector<LegPtr>& legs, DIFF_TYPE diffType)
{
	// calculate the diff
	double diff = 0;
	assert(legs.size() > 1);

	if(diffType == LONG_DIFF)
	{
		double long_cost = legs[0]->Ask();
		double short_cost = legs[1]->Bid();
		diff = long_cost - short_cost;
	}
	else if(diffType == SHORT_DIFF)
	{
		double short_cost = legs[0]->Bid();
		double long_cost = legs[1]->Ask();
		diff = short_cost - long_cost;
	}
	else
	{
		diff = legs[0]->Last() - legs[1]->Last();
	}


	return diff;
}

int CalcSize(vector<LegPtr>& legs, DIFF_TYPE diffType)
{
	// calculate the diff
	int diffSize = 0;
	assert(legs.size() > 1);

	if(diffType == LONG_DIFF)
	{
		int long_size = legs[0]->AskSize();
		int short_size = legs[1]->BidSize();
		diffSize = long_size < short_size ? long_size : short_size;
	}
	else if(diffType == SHORT_DIFF)
	{
		int short_size = legs[0]->BidSize();
		int long_size = legs[1]->AskSize();
		diffSize = short_size < long_size ? short_size : long_size;
	}

	return diffSize;
}

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
	, m_maxOpenPerStart(100)
	, m_maxCancel(450)
	, m_totalOpenLimit(450)
	, m_hedgeFlag(trade::SPECULATION)
{
	// Backup created portfolio item
	m_portfolioItem.CopyFrom(srcPortfolioItem);

	InitOpenCancelLimit(srcPortfolioItem);
	InitEndTimePoints(srcPortfolioItem);
	
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

	if(m_strategy.get() != NULL)
	{
		m_strategy->Cleanup();
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
		created = StrategyPtr(new CArbitrageStrategy(strategyItem, m_avatar, this));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioArbitrageOrderPlacer);
		break;
	case entity::CHANGE_POSITION:
		created = StrategyPtr(new CChangePositionStrategy(strategyItem, m_avatar, this));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioOrderPlacer);
		break;
	case entity::SCALPER:
		created = StrategyPtr(new CScalperStrategy(strategyItem));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioScalperOrderPlacer);
		break;
	case entity::HIST_SLOPE:
		created = StrategyPtr(new CHistSlopeStrategy(strategyItem, m_avatar));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioTrendOrderPlacer);
		break;
	case entity::WMA_TREND:
		created = StrategyPtr(new CWMATrendStrategy(strategyItem, m_avatar));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioTrendOrderPlacer);
		break;
	case entity::LINER_REGRESSION:
		created = StrategyPtr(new CLinerRegressionStrategy(strategyItem, m_avatar));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioTrendOrderPlacer);
		break;
	case entity::ASC_TREND:
		created = StrategyPtr(new CASCTrendStrategy(strategyItem, m_avatar));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioTrendOrderPlacer);
		break;
	case entity::RANGE_TREND:
		created = StrategyPtr(new CRangeTrendStrategy(strategyItem, m_avatar));
		m_orderPlacer = OrderPlacerPtr(new CPortfolioTrendOrderPlacer);
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
#ifdef _DEBUG
	cout << "Quote incoming: " << pQuote->symbol() << ", " << pQuote->last() << ", " << pQuote->update_time() << endl; 
#endif
	CheckForStart(pQuote->update_time());

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
	// 5. detect whether to stop
	CheckForStop(pQuote->update_time());
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
	m_portfolioUpdate.set_profit(m_profit);
}

void CPortfolio::AddPosition(const trade::MultiLegOrder& openOrder, int actualTradedVol)
{
	int openTimes = m_strategy->OnPortfolioAddPosition(this, openOrder, actualTradedVol);
}

void CPortfolio::RemovePosition(const trade::MultiLegOrder& closeOrder, int actualTradedVol)
{
	int closeTimes = m_strategy->OnPortfolioRemovePosition(this, closeOrder, actualTradedVol);
}

void CPortfolio::PushUpdate()
{
	m_avatar->PublishPortfolioUpdate(m_portfolioUpdate);
}

void CPortfolio::StartStrategy(int lastOrderId)
{
	logger.Info(boost::str(boost::format("[%s] Portfolio (%s) START strategy >>>") % InvestorId() % ID()));
	if(m_strategy->IsRunning())
	{
		logger.Warning(boost::str(boost::format("[%s] Portfolio (%s) Already Started !") % InvestorId() % ID()));
		return;
	}
	m_openTimes = 0;
	m_targetEnd.clear();
	if(lastOrderId >= m_serialOrderId)
		m_serialOrderId = lastOrderId;
	m_orderPlacer->Prepare();
	m_strategy->Start();
}

void CPortfolio::StopStrategy()
{
	logger.Info(boost::str(boost::format("[%s] Portfolio (%s) STOP strategy <<<") % InvestorId() % ID()));
	if (m_strategy->IsRunning())
	{
		m_strategy->Stop();
		m_orderPlacer->Cleanup();
	}
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
	m_hedgeFlag = srcPortfolioItem.hedgeflag();
}

void CPortfolio::CheckOpenCancelLimit()
{
	string msg;
	if(m_openTimes >= m_maxOpenPerStart)
	{
		msg = boost::str(boost::format("本次策略开仓%d手已达到上限%d") % m_openTimes % m_maxOpenPerStart);
		StopStrategyDueTo(msg);
	}
	else if(m_cancelTimes >= m_maxCancel)
	{
		msg = boost::str(boost::format("撤单%d次已达到上限%d") % m_cancelTimes % m_maxCancel);
		StopStrategyDueTo(msg);
	}
	else if(m_totalOpenTimes >= m_totalOpenLimit)
	{
		msg = boost::str(boost::format("总开仓%d手已达到上限%d") % m_totalOpenTimes % m_totalOpenLimit);
		StopStrategyDueTo(msg);
	}
}

void CPortfolio::UpdatePosition()
{
	int posiQty = m_totalOpenTimes - m_totalCloseTimes;
	if(posiQty < 0)
		posiQty = 0;
	m_currentPosition = posiQty;
}

void CPortfolio::SetQuantity( int perOpenQty, int perStartQty, int totalOpenLimit, int maxCancelQty )
{
	m_portfolioItem.set_quantity(perOpenQty);
	m_portfolioItem.set_maxopenperstart(perStartQty);
	m_portfolioItem.set_totalopenlimit(totalOpenLimit);
	m_portfolioItem.set_maxcancel(maxCancelQty);
	
	InitOpenCancelLimit(m_portfolioItem);

	logger.Info(boost::str(boost::format("[%s] Portfolio (%s) Modify Quantity: PerOpen = %d, PerStart = %d, MaxOpen = %d, MaxCancel = %d") 
		% InvestorId() % ID() % perOpenQty % perStartQty % totalOpenLimit % maxCancelQty));
}

bool compareTimePoint(const string& tp1, const string& tp2)
{
	return tp1.compare(tp2) < 0;
}

void CPortfolio::InitEndTimePoints( const entity::PortfolioItem& srcPortfolioItem )
{
	int count = srcPortfolioItem.endtimepoints_size();
	if(count > 0)
	{
		for(int i = 0; i < count; ++i)
		{
			const string& timeScope = srcPortfolioItem.endtimepoints(i);
			vector<string> SplitVec;
			boost::split( SplitVec, timeScope, boost::is_any_of("-"));
			if(SplitVec.size() > 1)
			{
				m_beginTimePoints.push_back(SplitVec[0]);
				m_endTimePoints.push_back(SplitVec[1]);
			}
			else if(SplitVec.size() == 1)
			{
				m_endTimePoints.push_back(SplitVec[0]);
			}
		}

		sort(m_endTimePoints.begin(), m_endTimePoints.end(), compareTimePoint);
		sort(m_beginTimePoints.begin(), m_beginTimePoints.end(), compareTimePoint);
	}
}

void CPortfolio::SetEndTimePoints(vector<string>& timepoints)
{
	boost::mutex::scoped_lock l(m_endTimeMutex);

	m_targetEnd.clear();
	m_beginTimePoints.clear();
	m_endTimePoints.clear();

	for(int i = 0; i < timepoints.size(); ++i)
	{
		const string& timeScope = timepoints[i];
		vector<string> SplitVec;
		boost::split( SplitVec, timeScope, boost::is_any_of("-"));
		if(SplitVec.size() > 1)
		{
			m_beginTimePoints.push_back(SplitVec[0]);
			m_endTimePoints.push_back(SplitVec[1]);
		}
		else if(SplitVec.size() == 1)
		{
			m_endTimePoints.push_back(SplitVec[0]);
		}
	}
	
	sort(m_endTimePoints.begin(), m_endTimePoints.end(), compareTimePoint);
	sort(m_beginTimePoints.begin(), m_beginTimePoints.end(), compareTimePoint);
}

void CPortfolio::CheckForStart( const string& quoteUpdateTime )
{
	boost::mutex::scoped_lock l(m_endTimeMutex);

	if(m_strategy->IsRunning() || m_beginTimePoints.size() == 0)
		return;

	for(vector<string>::iterator iter = m_beginTimePoints.begin(); iter !=m_beginTimePoints.end(); ++iter)
	{
		boost::chrono::seconds tpTarget = ParseTimeString(*iter);
		boost::chrono::seconds tpQuote = ParseTimeString(quoteUpdateTime);
		if(tpQuote >= tpTarget && tpQuote - tpTarget <= boost::chrono::seconds(2))
		{
			string msg = boost::str(boost::format("策略从%s起启动") % quoteUpdateTime);
			StartStrategyDueTo(msg);
			logger.Info(boost::str(boost::format("[%s] Portfolio (%s) Auto Start at %s") 
				% InvestorId() % ID() % quoteUpdateTime));
		}
	}
}

void CPortfolio::CheckForStop(const string& quoteUpdateTime)
{
	boost::mutex::scoped_lock l(m_endTimeMutex);

	if(m_endTimePoints.size() == 0 || !m_strategy->IsRunning())
	{
		return;
	}

	if(m_targetEnd.empty())
	{
		for(vector<string>::iterator iter = m_endTimePoints.begin(); iter !=m_endTimePoints.end(); ++iter)
		{
			if(iter->compare(quoteUpdateTime) > 0)
			{
				m_targetEnd = *iter;
				break;
			}
		}
	}
	else
	{
		if(quoteUpdateTime.compare(m_targetEnd) >= 0)
		{
			string msg = boost::str(boost::format("策略已自动停止于%s") % m_targetEnd);
			StopStrategyDueTo(msg);
			logger.Info(boost::str(boost::format("[%s] Portfolio (%s) Auto Stop at %s") 
				% InvestorId() % ID() % m_targetEnd));
		}
	}
}



void CPortfolio::StopStrategyDueTo( const string& stopReason )
{
	StopStrategy();
	m_portfolioUpdate.set_running(false);

	if(stopReason.length() > 0)
	{
		PushMessage(stopReason);
	}
}

void CPortfolio::StartStrategyDueTo( const string& startReason )
{
	StartStrategy(0);
	m_portfolioUpdate.set_running(true);

	if(startReason.length() > 0)
	{
		PushMessage(startReason);
	}
}

void CPortfolio::StrategyForceOpen()
{
	m_strategy->SetForceOpen();
}

void CPortfolio::StrategyForceClose()
{
	m_strategy->SetForceClose();
}

void CPortfolio::PushMessage( const string& msg )
{
	boost::mutex::scoped_lock l(m_mutPushingMsg);
	string utf8Msg;
	GB2312ToUTF_8(utf8Msg, msg.c_str());
	m_portfolioUpdate.set_message(utf8Msg);
	PushUpdate();
	m_portfolioUpdate.clear_message();
}

double CPortfolio::CalculateDiff( ARBI_DIFF_CALC* structArbiDiffCalc, CALC_DIFF_METHOD calcDiffMethod )
{
	// calculate the diff
	double diff = 0;
	assert(structArbiDiffCalc != NULL);
	int legCount = Count();
	DIFF_TYPE diffType = structArbiDiffCalc->DiffType;
	for(int i = 0; i < legCount; ++i)
	{
		LegPtr& leg = m_legs[i];
		entity::PosiDirectionType side = leg->Side();
		double minPxMv = leg->MinPriceChange();
		if(diffType == LONG_DIFF)
		{
			if(side == entity::LONG)
			{
				structArbiDiffCalc->BuyPrice = calcDiffMethod == BETTER_PRICE && leg->IsPreferred() ? leg->Bid() + minPxMv : leg->Ask();
				diff += structArbiDiffCalc->BuyPrice;
			}
			else if(side == entity::SHORT)
			{
				structArbiDiffCalc->SellPrice = calcDiffMethod == BETTER_PRICE && leg->IsPreferred() ? leg->Ask() - minPxMv : leg->Bid();
				diff -= structArbiDiffCalc->SellPrice;
			}
		}
		else if(diffType == SHORT_DIFF)
		{
			if(side == entity::LONG)
			{
				structArbiDiffCalc->SellPrice = calcDiffMethod == BETTER_PRICE && leg->IsPreferred() ? leg->Ask() - minPxMv : leg->Bid();
				diff += structArbiDiffCalc->SellPrice;
			}
			else if(side == entity::SHORT)
			{
				structArbiDiffCalc->BuyPrice = calcDiffMethod == BETTER_PRICE && leg->IsPreferred() ? leg->Bid() + minPxMv : leg->Ask();
				diff -= structArbiDiffCalc->BuyPrice;
			}
		}
		else
		{
			if(side == entity::LONG)
			{
				structArbiDiffCalc->BuyPrice = leg->Last();
				diff += structArbiDiffCalc->BuyPrice;
			}
			else if(side == entity::SHORT)
			{
				structArbiDiffCalc->SellPrice = leg->Last();
				diff -= structArbiDiffCalc->SellPrice;
			}
		}
	}
	structArbiDiffCalc->Diff = diff;
	return diff;
}

bool CPortfolio::LegsTimestampAligned()
{
	int legCount = Count();
	if (legCount < 1)
		return false;

	for (int i = 1; i < legCount; ++i)
	{
		if (m_legs[0]->Timestamp() != m_legs[i]->Timestamp())
			return false;
	}

	return true;
}

void CPortfolio::PrintLegsQuote()
{
	LOG_DEBUG(logger, boost::str(boost::format("Print Portfolio %s Legs Quotes") % ID()));

	int legCount = Count();
	for (int i = 0; i < legCount; ++i)
	{
		const LegPtr& l = m_legs[i];
		LOG_DEBUG(logger, boost::str(boost::format("Leg%d %s L:%.2f A:%.2f B:%.2f %s") 
			% (i + 1) % l->Symbol()
			% l->Last() % l->Ask() % l->Bid() % l->Timestamp()));
	}
}


