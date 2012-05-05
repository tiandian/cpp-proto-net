#pragma once

#include "Message.h"
#include "BufferRunner.h"

class CQuote;
class COperationRecordData;
class CTimeNSalePacket;

typedef boost::function<void(CQuote*)> QuoteUpdateFunc;
typedef boost::function<void(COperationRecordData*)> OperationRecordUpdateFunc;
typedef boost::function<void(CTimeNSalePacket*)> TimeNSalesUpdateFunc;

class CClientAgent
{
public:
	CClientAgent(void);
	~CClientAgent(void);


	void SetQuoteCallback(QuoteUpdateFunc callback);
	void SetOperationRecordCallback(OperationRecordUpdateFunc callback);
	void SetTimeNSalesCallback(TimeNSalesUpdateFunc callback);

	void Publish(boost::shared_ptr<CMessage>& msgPack);

private:
	void DispatchMsgPack(boost::shared_ptr<CMessage>& package);


	QuoteUpdateFunc m_quoteCallback;
	OperationRecordUpdateFunc m_recordCallback;
	TimeNSalesUpdateFunc m_tnsUpdateCallback;

	CBufferRunner< boost::shared_ptr<CMessage> > m_bufferRunner;
};

