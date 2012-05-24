#pragma once

#include "Message.h"
#include "BufferRunner.h"

class CQuote;
class COperationRecordData;
class CTimeNSalePacket;
class CAccountInfoMsg;
class CPositionDetailMsg;
class COrderMsg;

typedef boost::function<void(CQuote*)> QuoteUpdateFunc;
typedef boost::function<void(COperationRecordData*)> OperationRecordUpdateFunc;
typedef boost::function<void(CTimeNSalePacket*)> TimeNSalesUpdateFunc;
typedef boost::function<void(CAccountInfoMsg*)> AccountInfoUpdateFunc;
typedef boost::function<void(CPositionDetailMsg*)> PositionDetailUpdateFunc;
typedef boost::function<void(COrderMsg*)> OrderUpdateFunc;

class CClientAgent
{
public:
	CClientAgent(void);
	~CClientAgent(void);


	void SetQuoteCallback(QuoteUpdateFunc callback);
	void SetOperationRecordCallback(OperationRecordUpdateFunc callback);
	void SetTimeNSalesCallback(TimeNSalesUpdateFunc callback);
	void SetAccountInfoCallback(AccountInfoUpdateFunc callback);
	void SetPositionDetailCallback(PositionDetailUpdateFunc callback);
	void SetOrderCallback(OrderUpdateFunc callback);

	void Publish(boost::shared_ptr<CMessage>& msgPack);

private:
	void DispatchMsgPack(boost::shared_ptr<CMessage>& package);


	QuoteUpdateFunc m_quoteCallback;
	OperationRecordUpdateFunc m_recordCallback;
	TimeNSalesUpdateFunc m_tnsUpdateCallback;
	AccountInfoUpdateFunc m_acctUpdateCallback;
	PositionDetailUpdateFunc m_positionDetailUpdateCallback;
	OrderUpdateFunc m_orderUpdateCallback;

	CBufferRunner< boost::shared_ptr<CMessage> > m_bufferRunner;
};

