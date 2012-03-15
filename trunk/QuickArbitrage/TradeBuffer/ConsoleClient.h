#pragma once
#include "clientbase.h"

#include <sstream>

class CConsoleClient :
	public ClientBase
{
public:
	CConsoleClient(void);
	~CConsoleClient(void);

protected:
	virtual void ProcessQuote(CTP::Quote* pQuote);

private:
	std::stringstream m_streamOut;
};

