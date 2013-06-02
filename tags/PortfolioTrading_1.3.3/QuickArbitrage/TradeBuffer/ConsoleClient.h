#pragma once
#include "clientbase.h"
#include "protobuf_gen/trade.pb.h"

#include <sstream>
#include <boost/uuid/uuid.hpp>

class CConsoleClient :
	public ClientBase
{
public:
	CConsoleClient(void);
	~CConsoleClient(void);

	bool Login(const std::string& brokerId, const std::string& userId, const std::string& password);
	void Logout();

	void OnRegisterResult(bool succ, std::string& errMsg);

	void Buy();

	void AddPortfolio(double longPrice = 0, double shortPrice = 0);
	void OpenPosition(int portIdx, int legIdx = -1);
	void ClosePosition(int portIdx, int legIdx = -1);
	void ShowPortfolio();
	void SetLegDirection(int portIdx, int legIdx, protoc::PosiDirectionType side);
	void SetLegOrdPriceType(int portIdx, int legIdx, protoc::OrderPriceTypeType priceType);
	void SetLegLmtPrice(int portIdx, int legIdx, double limitPrice);
	void CancelPortfolio(int portIdx, int legIdx = -1);
	void SetPortfolioAuto(int portIdx, bool isAuto);
	void SetPortfolioParams(int portIdx, const string& key, const string& value);
protected:
	virtual void ProcessQuote(CTP::Quote* pQuote);

private:
	bool GetPortfolioIDByIndex(int idx, boost::uuids::uuid* outPID);

	std::stringstream m_streamOut;

	ClientBase* m_pRemoteClient;
};

