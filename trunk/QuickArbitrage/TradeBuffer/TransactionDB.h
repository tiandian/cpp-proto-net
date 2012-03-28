#pragma once

#include "dbaccess.h"
#include "Portfolio.h"
#include "protobuf_gen/trade.pb.h"


class CTransactionDB :
	public CDBAccess
{
public:
	CTransactionDB(void);
	~CTransactionDB(void);

	void AddPortfolio(CPortfolio* po){}
	void ModifyPortfolio(CPortfolio* po){}
	void RemovePortfolio(CPortfolio* po){}

	void AddTrade(protoc::Trade* trade){}

private:
	void EnsurePortfolioTable();

};

