#include "StdAfx.h"
#include "TransactionDB.h"
#include "LogManager.h"
#include "Leg.h"
#include "protobuf_gen/trade.pb.h"

#include <string>
#include <sstream>
#include <boost/format.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define TRANSACTION_DB_DIRECTORY "transaction"
#define PORTFOLIO_TABLE "Portfolio"
#define LEG_TABLE "Leg"

using namespace std;

extern CLogManager logger;

CTransactionDB::CTransactionDB(void)
{
}

CTransactionDB::~CTransactionDB(void)
{
}

void CTransactionDB::EnsureValid( const char* dbFile )
{
	SetDBFile(boost::str(boost::format("%s\\%s.db") % TRANSACTION_DB_DIRECTORY % dbFile));
	EnsurePortfolioTable();
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

void CTransactionDB::AddPortfolio( CPortfolio* po )
{
	char* errMsg = NULL; 

	if(!Open()) return;

	int res = Exec("begin transaction;", &errMsg); 

	try
	{
		string pid = boost::uuids::to_string(po->GetID());
		boost::format sqlfmt("insert into [%s] ([PID], [Quantity]) values('%s', %d)"); 
		sqlfmt % PORTFOLIO_TABLE % pid.c_str() % po->GetQuantity();
		string portfolioInsert = boost::str(sqlfmt);

		res = Exec(portfolioInsert, &errMsg);
		if(res != SQLITE_OK) throw exception("insert portfolio failed");

		std::ostringstream ossSQL;
		BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, po->GetLegs())
		{
			ossSQL.str("");
			ossSQL << "insert into [" << LEG_TABLE << "] ([PID], [Symbol], [Side], [Ratio], [Status], [OrderStatus]) ";
			ossSQL << "values(";
			ossSQL << "'" << pid << "',";
			ossSQL << "'" << leg->GetSymbol() << "',";
			ossSQL << leg->GetSide() << ",";
			ossSQL << leg->GetRatio() << ",";
			ossSQL << leg->GetStatus() << ",";
			ossSQL << leg->GetOrderStatus();
			ossSQL << ");";
			string legInsertSql = ossSQL.str();

			res = Exec(legInsertSql, &errMsg);
			if(res != SQLITE_OK) throw exception("insert leg failed");
		}
		
		res = Exec("commit transaction;", &errMsg);
		if(res != SQLITE_OK) throw exception("commit add portfolio");
	}
	catch (exception ex)
	{
		string err = boost::str(boost::format("%s SQL error: %s\n") % ex.what() % (errMsg != NULL ? errMsg : "unknown"));
		logger.Error(err);
		sqlite3_free(errMsg);

		res = Exec("rollback transaction;", &errMsg);
	}

	if(!m_shareConnection) Close();
}

int fetch_portfolio_callback(void* state, int nColNum, char** azColValue, char** azColName)
{
	assert(nColNum = 2);
	if(nColNum > 0)
	{
		static boost::uuids::string_generator gen;

		std::vector< boost::shared_ptr< CPortfolio > >* portVec = static_cast< std::vector< boost::shared_ptr< CPortfolio > >* >(state);
		
		boost::uuids::uuid pid = gen(azColValue[0]);
		int quantity = boost::lexical_cast<int>(azColValue[1]);

		boost::shared_ptr<CPortfolio> port(new CPortfolio(pid));
		port->SetQuantity(quantity);

		portVec->push_back(port);
	}
	return 0;
}

int fetch_legs_callback(void* state, int nColNum, char** azColValue, char** azColName)
{
	if(nColNum > 0)
	{
		CPortfolio* portfolio = static_cast<CPortfolio*>(state);
		CLeg* leg = portfolio->AddLeg();
		
		string symbol = azColValue[1];
		leg->SetSymbol(symbol);

		protoc::PosiDirectionType side = static_cast<protoc::PosiDirectionType>(boost::lexical_cast<int>(azColValue[2]));
		leg->SetSide(side);

		int ratio = boost::lexical_cast<int>(azColValue[3]);
		leg->SetRatio(ratio);
		
		LEG_STATUS status = static_cast<LEG_STATUS>(boost::lexical_cast<int>(azColValue[4]));
		leg->SetStatus(status);

		protoc::OrderStatusType orderStatus = static_cast<protoc::OrderStatusType>(boost::lexical_cast<int>(azColValue[5]));
		leg->SetOrderStatus(orderStatus)	;

		if(azColValue[6] != NULL)
		{
			string orderRef = azColValue[6];
			leg->SetOrderRef(orderRef);
		}

		if(azColValue[7] != NULL && strlen(azColValue[7]) > 0)
		{
			protoc::OrderSubmitStatusType submitStatus = static_cast<protoc::OrderSubmitStatusType>(boost::lexical_cast<int>(azColValue[7]));
			leg->SetOrderSubmitStatus(submitStatus);
		}

		if(azColValue[8] != NULL && strlen(azColValue[8]) > 0)
		{
			protoc::OrderPriceTypeType openPriceType = static_cast<protoc::OrderPriceTypeType>(boost::lexical_cast<int>(azColValue[8]));
			leg->SetOpenOrderPriceType(openPriceType);
		}

		if(azColValue[9] != NULL && strlen(azColValue[9]) > 0)
		{
			double price = boost::lexical_cast<double>(azColValue[9]);
			leg->SetOpenLimitPrice(price);
		}

		if(azColValue[10] != NULL && strlen(azColValue[10]) > 0)
		{
			protoc::OrderPriceTypeType closePriceType = static_cast<protoc::OrderPriceTypeType>(boost::lexical_cast<int>(azColValue[10]));
			leg->SetCloseOrderPriceType(closePriceType);
		}

		if(azColValue[11] != NULL && strlen(azColValue[11]) > 0)
		{
			double price = boost::lexical_cast<double>(azColValue[11]);
			leg->SetCloseLimitPrice(price);
		}

		if(azColValue[12] != NULL && strlen(azColValue[12]) > 0)
		{
			double cost = boost::lexical_cast<double>(azColValue[12]);
			leg->SetCost(cost);
		}
	}
	return 0;
}

void CTransactionDB::FetchPortfolio( std::vector< boost::shared_ptr< CPortfolio > >& portfolioVec )
{
	if(!Open()) return;

	// fetch from portfolio table
	string selectPortfolio = boost::str(boost::format("select [PID], [Quantity] from [%s]") % PORTFOLIO_TABLE);
	int rc = ExecuteSQL(selectPortfolio, fetch_portfolio_callback, &portfolioVec);

	BOOST_FOREACH(const boost::shared_ptr< CPortfolio >& port, portfolioVec)
	{
		// fetch from leg table
		const string& pid = boost::uuids::to_string(port->GetID());
		string selectLeg = boost::str(boost::format("select * from [%s] where [PID]='%s'") % LEG_TABLE % pid.c_str());
		rc = ExecuteSQL(selectLeg, fetch_legs_callback, port.get());
	}

	if(!m_shareConnection) Close();
}


