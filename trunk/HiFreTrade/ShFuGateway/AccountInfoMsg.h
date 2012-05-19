#pragma once
#include "message.h"
#include "EntityStructs.h"
#include "AccountInfo.h"

class CAccountInfoMsg :
	public CMessage
{
public:
	CAccountInfoMsg(void);
	~CAccountInfoMsg(void);

	const AccountInfoData* InnerStruct() { return &m_innerData; }
	void SetData(CAccountInfo* pAccount)
	{
		if(pAccount != NULL)
		{
			SafeStringCopy(m_innerData.caBrokerId, pAccount->brokerid().c_str(), BROKERID_LEN);
			SafeStringCopy(m_innerData.caAccountId, pAccount->accountid().c_str(), ACCOUNTID_LEN);
			SafeStringCopy(m_innerData.caTradingDay, pAccount->tradingday().c_str(), TRADINDG_DAY_LEN);
			
			m_innerData.dPremortgage = pAccount->premortgage();
			m_innerData.dPreCredit = pAccount->precredit();
			m_innerData.dPreDeposit = pAccount->predeposit();
			m_innerData.dPreBalance = pAccount->prebalance();
			m_innerData.dPreMargin = pAccount->premargin();
			m_innerData.dInterestBase = pAccount->interestbase();
			m_innerData.dInterest = pAccount->interest();
			m_innerData.dDeposit = pAccount->deposit();
			m_innerData.dWithdraw = pAccount->withdraw();
			m_innerData.dFrozenMargin = pAccount->frozenmargin();
			m_innerData.dFrozenCash = pAccount->frozencash();
			m_innerData.dFrozenCommission = pAccount->frozencommission();
			m_innerData.dCurrMargin = pAccount->currmargin();
			m_innerData.dCashIn = pAccount->cashin();
			m_innerData.dCommission = pAccount->commission();
			m_innerData.dCloseProfit = pAccount->closeprofit();
			m_innerData.dPositionProfit = pAccount->positionprofit();
			m_innerData.dBalance = pAccount->balance();
			m_innerData.dAvailable = pAccount->available();
			m_innerData.dWithdrawQuota = pAccount->withdrawquota();
			m_innerData.dReserve = pAccount->reserve();
			
			m_innerData.iSettlementId = pAccount->settlementid();
			m_innerData.dCredit = pAccount->credit();
			m_innerData.dMortgage = pAccount->mortgage();
			m_innerData.dExchangeMargin = pAccount->exchangemargin();
			m_innerData.dDeliveryMargin = pAccount->deliverymargin();
			m_innerData.dExchangeDeliveryMargin = pAccount->exchangedeliverymargin();
		}
	}

private:

	AccountInfoData m_innerData;
};

