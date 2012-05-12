#pragma once

#define SYMBOL_LEN 31
#define TRADINDG_DAY_LEN 9
#define UPDATE_TIME_LEN 9

struct QuoteData
{
	char caSymbol[SYMBOL_LEN];
	char caTradingDay[TRADINDG_DAY_LEN];
	double dLast;
	double dPrevClose;
	double dOpen;
	double dHigh;
	double dLow;
	int iVolume;
	double dTurnover;
	double dClose;
	char caUpdateTime[UPDATE_TIME_LEN];
	int iUpdateMillisec;

	double dBid;
	int iBidSize;
	double dAsk;
	int iAskSize;
};

struct BreakOutStrategy
{
	// Entry condition
	int iRectPeriod;
	double dRectRange;
	bool bUpBreak;
	bool bDownBreak;
	double dBreakoutCriterion;
	int iBreakoutTimespan;

	// Exit condition
	double dGainLimit;

	// StopLoss condition
	double dLossLimit;
};

// for every definition
#define UNKNOWN 0

// iDirection
#define SHORT_BREAKOUT 1
#define LONG_BREAKOUT 2

// iEntryType and iExitType
#define SHORT_OPEN 1
#define LONG_OPEN 2
#define SHORT_CLOSE 3
#define LONG_CLOSE 4

// iEntryStatus and iExitStatus
#define UNOPEN 1
#define ORDER_SUBMIT 2
#define PENDING 3
#define PARTIALLY_FILLED 4
#define FULL_FILLED 5
#define CANCELED 6
#define REJECTED 7
#define CLOSED UNOPEN
#define SENDING_ORDER 8

// iEntryReason
#define CONDITION_TRIGGER 1
#define MANUAL_OPEN 2

// iExitReason
#define STOP_GAIN 1
#define STOP_LOSS 2
#define MANUAL_CLOSE 3

struct OperationRecord
{
	int iRecordId;
	char caSymbol[SYMBOL_LEN];
	char caRectPeriodBegin[UPDATE_TIME_LEN];
	char caRectPeriodEnd[UPDATE_TIME_LEN];

	double dUpperBoundary;
	double dLowerBoundary;
	double dRange;
	int iDirection;

	double dEntryPoint;
	char caEntryTime[UPDATE_TIME_LEN];
	int iEntryReason;
	int iEntryType;
	int iEntryQuantity;
	int iEntryStatus;

	double dExitPoint;
	char caExitTime[UPDATE_TIME_LEN];
	int iExitReason;
	int iExitType;
	int iExitQuantity;
	int iExitStatus;

	double ProfitLoss;
};

struct TimeNSalesData
{
	char caSymbol[SYMBOL_LEN];

	char caTimeStamp[UPDATE_TIME_LEN];
	
	double dPrice;
	int iQuantity;
	int iDirection;
};

