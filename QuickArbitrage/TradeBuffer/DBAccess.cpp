#include "StdAfx.h"
#include "DBAccess.h"
#include "LogManager.h"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

extern CLogManager logger;

CDBAccess::CDBAccess(const char* dbFilePath, bool shareConnection):
	m_db(NULL),
	m_dbFile(dbFilePath),
	m_shareConnection(shareConnection)
{
	
}


CDBAccess::~CDBAccess(void)
{
}

bool CDBAccess::Open()
{
	int rc = SQLITE_OK;
	if(m_db == NULL)
	{
		rc = sqlite3_open(m_dbFile.c_str(), &m_db);
		if(SQLITE_OK != rc)
		{
			logger.Error(boost::str(boost::format("Can't open database(%s): %s") % m_dbFile.c_str() % sqlite3_errmsg(m_db)));
			Close();
		}
	}
	return SQLITE_OK == rc;
}

void CDBAccess::Close()
{
	if(m_db != NULL)
	{
		sqlite3_close(m_db);
		m_db = NULL;
	}
}

bool CDBAccess::ExecuteNonQuery( const string& sqltext )
{
	return ExecuteSQL(sqltext, NULL, NULL);
}

int execute_scalar_callback(void* state, int nColNum, char** azColValue, char** azColName)
{
	if(nColNum > 0)
	{
		string* str = static_cast<string*>(state);
		*str = azColValue[0];
	}
	return 0;
}

bool CDBAccess::ExecuteScalar( const string& sqltext, string* outData )
{
	return ExecuteSQL(sqltext, execute_scalar_callback, outData);
}

bool CDBAccess::CheckTableExist( const string& tableName )
{
	return CheckTableExist(tableName.c_str());
}

bool CDBAccess::CheckTableExist( const char* tableName )
{
	string sql = boost::str(boost::format("SELECT count(name) FROM sqlite_master WHERE name='%s'") % tableName );
	string retVal;
	bool succ = ExecuteScalar(sql, &retVal);
	if(succ)
	{
		int count = boost::lexical_cast<int>(retVal);
		return count > 0;
	}
	else
		return false;
}

bool CDBAccess::ExecuteSQL( const string& sqltext , int (*callback)(void*,int,char**,char**), void * pState )
{
	if(!Open())
		return false;

	int rc = -1;

	try
	{
		char* zErrMsg = NULL;
		rc = sqlite3_exec(m_db, sqltext.c_str(), callback, pState, &zErrMsg);
		if( rc!= SQLITE_OK )
		{
			string err = boost::str(boost::format("SQL error: %s\n") % zErrMsg);
			logger.Error(err);
			sqlite3_free(zErrMsg);
		}
	}
	catch(std::exception ex)
	{
		boost::format fmt("Executing '%s' failed due to %s");
		fmt % sqltext.c_str() % ex.what();
		logger.Error(fmt.str());
	}
	catch (...)
	{
		logger.Error(boost::str(boost::format("Encounter unknown error while executing '%s'") % sqltext.c_str()));
	}

	if(!m_shareConnection)
		Close();

	return rc == SQLITE_OK;
}

