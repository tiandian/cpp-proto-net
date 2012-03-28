#include "StdAfx.h"
#include "TransactionDB.h"

#include <string>

#define TRANSACTION_DB_FILE "transaction.db"
#define PORTFOLIO_TABLE "Portfolio"
#define LEG_TABLE "Leg"


CTransactionDB::CTransactionDB(void):
CDBAccess(TRANSACTION_DB_FILE)
{
	EnsurePortfolioTable();
}

CTransactionDB::~CTransactionDB(void)
{
}

void CTransactionDB::EnsurePortfolioTable()
{
	std::string sql;
	if(!CheckTableExist(PORTFOLIO_TABLE))
	{
		// Create Portfolio table
		sql = "CREATE TABLE [Portfolio] (					\n\
				[PID] TEXT NOT NULL,						\n\
				[Quantity] INT NOT NULL,					\n\
				CONSTRAINT [sqlite_autoindex_Portfolio_1] PRIMARY KEY ([PID]));";
		ExecuteNonQuery(sql);
	}

	if(!CheckTableExist(LEG_TABLE))
	{
		// Create leg table
		sql = "CREATE TABLE [Leg] (							\n\
				[PID] TEXT NOT NULL,						\n\
				[Symbol] TEXT NOT NULL,						\n\
				[Side] INT NOT NULL,						\n\
				[Ratio] INT NOT NULL,						\n\
				[Status] INT NOT NULL,						\n\
				[OrderStatus] INT NOT NULL,					\n\
				[OrderRef] TEXT,							\n\
				[OrderSubmitStatus] INT,					\n\
				[OpenOrderPriceType] INT,					\n\
				[OpenLimitPrice] DOUBLE DEFAULT 0,			\n\
				[CloseOrderPriceType] INT,					\n\
				[CloseLimitPrice] DOUBLE DEFAULT 0,			\n\
				[Cost] DOUBLE,								\n\
				CONSTRAINT [sqlite_autoindex_Leg_1] PRIMARY KEY ([PID], [Symbol]));";
		ExecuteNonQuery(sql);
	}
}
