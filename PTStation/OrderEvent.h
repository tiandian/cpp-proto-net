#pragma once

#include "entity/trade.pb.h"

struct evtOrder
{
	evtOrder(boost::shared_ptr<trade::Order>& pOrd):m_pOrd(pOrd){}

	boost::shared_ptr<trade::Order> m_pOrd;
};

struct evtSubmit : public evtOrder 
{
	evtSubmit(boost::shared_ptr<trade::Order>& pOrd):evtOrder(pOrd){}
};

struct evtPending : public evtOrder 
{
	evtPending(boost::shared_ptr<trade::Order>& pOrd):evtOrder(pOrd){}
};

struct evtReject: public evtOrder 
{
	evtReject(boost::shared_ptr<trade::Order>& pOrd):evtOrder(pOrd){}
};

struct evtFilled : public evtOrder 
{
	evtFilled(boost::shared_ptr<trade::Order>& pOrd):evtOrder(pOrd){}
};

struct evtPartiallyFilled : public evtOrder 
{
	evtPartiallyFilled(boost::shared_ptr<trade::Order>& pOrd):evtOrder(pOrd){}
};

struct evtAllFilled {};
struct evtNextLeg {};

struct evtCancelSuccess : public evtOrder 
{
	evtCancelSuccess(boost::shared_ptr<trade::Order>& pOrd):evtOrder(pOrd){}
};

struct evtRetry {};
struct evtFirstCanceled {};
struct evtPendingTimeUp {};

struct evtNextQuoteIn {};

struct evtSubmitFailure 
{
	evtSubmitFailure(const string& errMsg):m_ErrorMsg(errMsg){}

	string m_ErrorMsg;
};

struct evtCancelFailure
{
	evtCancelFailure(int errorId, const string& errMsg):m_ErrorId(errorId),m_ErrorMsg(errMsg){}
	int m_ErrorId;
	string m_ErrorMsg;
};

struct evtFilledCanceled
{
	evtFilledCanceled(const string& errMsg):m_ErrorMsg(errMsg){}

	string m_ErrorMsg;
};

struct evtErrorFound 
{
	evtErrorFound(const string& errMsg):m_ErrorMsg(errMsg){}

	string m_ErrorMsg;
};