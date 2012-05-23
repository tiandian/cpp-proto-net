#pragma once
#include "message.h"
#include "EntityStructs.h"
#include "ThostFtdcUserApiStruct.h"

class CPositionDetailMsg :
	public CMessage
{
public:
	CPositionDetailMsg(void);
	~CPositionDetailMsg(void);

	const PositionDetail* InnerStruct() { return &m_innerData; }

	void SetData(CThostFtdcInvestorPositionDetailField* pPosition)
	{
		SafeStringCopy(m_innerData.caSymbol, pPosition->InstrumentID, SYMBOL_LEN);
		SafeStringCopy(m_innerData.caTradeID, pPosition->TradeID, TRADEID_LEN);
		m_innerData.cHedgeFlag = pPosition->HedgeFlag;
		
		if(pPosition->Direction == THOST_FTDC_D_Buy)
		{
			m_innerData.iDirection = LONG_OPEN;
		}
		else if(pPosition->Direction == THOST_FTDC_D_Sell)
		{
			m_innerData.iDirection = SHORT_OPEN;
		
		}
		
		SafeStringCopy(m_innerData.caOpenDate, pPosition->OpenDate, TRADINDG_DAY_LEN);
		m_innerData.iVolume = pPosition->Volume;
		m_innerData.dOpenPrice = pPosition->OpenPrice;
		SafeStringCopy(m_innerData.caTradingDay, pPosition->TradingDay, TRADINDG_DAY_LEN);
		SafeStringCopy(m_innerData.caExchangeID, pPosition->ExchangeID, EXCHANGEID_LEN);
		m_innerData.dCloseProfit = pPosition->CloseProfitByDate;
		m_innerData.dPositionProfit = pPosition->PositionProfitByDate;
		m_innerData.dMargin = pPosition->Margin;
		m_innerData.dMarginRateByMoney = pPosition->MarginRateByMoney;
		m_innerData.CloseVolume = pPosition->CloseVolume;
		m_innerData.CloseAmount = pPosition->CloseAmount;
	}

private:
	PositionDetail m_innerData;
};

