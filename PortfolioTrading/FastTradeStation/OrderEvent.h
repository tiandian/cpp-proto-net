#pragma once

#include "entity/trade.pb.h"

#include <string>

using namespace std;

struct evtOrder
{
	evtOrder(trade::Order* pOrd):m_pOrd(pOrd){}

	trade::Order* m_pOrd;
};

struct evtSubmit : public evtOrder 
{
	evtSubmit(trade::Order* pOrd):evtOrder(pOrd){}
};

struct evtPending : public evtOrder 
{
	evtPending(trade::Order* pOrd):evtOrder(pOrd){}
};

struct evtReject: public evtOrder 
{
	evtReject(trade::Order* pOrd):evtOrder(pOrd){}
};

struct evtFilled : public evtOrder 
{
	evtFilled(trade::Order* pOrd):evtOrder(pOrd){}
};

struct evtPartiallyFilled : public evtOrder 
{
	evtPartiallyFilled(trade::Order* pOrd):evtOrder(pOrd){}
};

struct evtAllFilled {};
struct evtNextLeg {};

struct evtCancelSuccess : public evtOrder 
{
	evtCancelSuccess(trade::Order* pOrd):evtOrder(pOrd){}
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
	evtCancelFailure(const string& errMsg):m_ErrorMsg(errMsg){}

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