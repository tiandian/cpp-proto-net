#pragma once

#include "RtnOrderWrapper.h"

struct evtOrder
{
	evtOrder(RtnOrderWrapperPtr& pOrd):m_pOrd(pOrd){}

	RtnOrderWrapperPtr m_pOrd;
};

struct evtSubmit : public evtOrder 
{
	evtSubmit(RtnOrderWrapperPtr& pOrd):evtOrder(pOrd){}
};

struct evtPending : public evtOrder 
{
	evtPending(RtnOrderWrapperPtr& pOrd):evtOrder(pOrd){}
};

struct evtReject: public evtOrder 
{
	evtReject(RtnOrderWrapperPtr& pOrd):evtOrder(pOrd){}
};

struct evtFilled : public evtOrder 
{
	evtFilled(RtnOrderWrapperPtr& pOrd):evtOrder(pOrd){}
};

struct evtPartiallyFilled : public evtOrder 
{
	evtPartiallyFilled(RtnOrderWrapperPtr& pOrd):evtOrder(pOrd){}
};

struct evtAllFilled {};
struct evtNextLeg {};

struct evtCancelSuccess : public evtOrder 
{
	evtCancelSuccess(RtnOrderWrapperPtr& pOrd):evtOrder(pOrd){}
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
	evtErrorFound(const char* errMsg):m_ErrorMsg(errMsg){}

	string m_ErrorMsg;
};