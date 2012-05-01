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