#pragma once

#include <string>
#include "sqlite/sqlite3.h"

using namespace std;

class CDBAccess
{
public:
	CDBAccess(const char* dbFilePath, bool shareConnection = false);
	CDBAccess();
	~CDBAccess(void);

	bool Open();
	void Close();

	bool ExecuteNonQuery(const string& sqltext);

	bool ExecuteScalar(const string& sqltext, string* outData);

	bool ExecuteSQL(const string& sqltext, int (*callback)(void*,int,char**,char**), void * pState );

	bool CheckTableExist(const string& tableName);
	bool CheckTableExist(const char* tableName);

	const string& GetDBFile() { return m_dbFile; }
	void SetDBFile(const string& dbFile) { m_dbFile = dbFile; }

	bool GetIsSharingConnection() { return m_shareConnection; }
	void SetIsSharingConnection(bool val) { m_shareConnection = val; }

protected:

	int Exec(const string& sql, char** szErr);
	int Exec(const char* sql, char** szErr);

	sqlite3* m_db;
	string m_dbFile;
	bool m_shareConnection;
};

