#include "StdAfx.h"
#include "MarketAgent.h"
#include "LogManager.h"
#include "Configuration.h"

#include <sstream>
#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/uuid/uuid_generators.hpp>

#define SYMBOL_MAX_LENGTH 10

using namespace std;

extern CConfiguration config;
extern CLogManager	logger;

CMarketAgent::CMarketAgent(void):
	m_pUserApi(NULL),
	m_iRequestID(0)
{
}

CMarketAgent::~CMarketAgent(void)
{
	if(m_pUserApi != NULL)
		m_pUserApi->Release();
}

bool CMarketAgent::Connect()
{
	try{
		// 初始化UserApi
		m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
		//CThostFtdcMdSpi* pUserSpi = new CMdSpi();
		m_pUserApi->RegisterSpi(&m_mktResponse);						// 注册事件类
		
		// duplicate address string and use boost.smart_ptr to manage its lifecycle
		boost::scoped_array<char> front_addr(strdup(config.GetMarketDataAddr()));
		
		m_pUserApi->RegisterFront(front_addr.get());					// connect
		m_pUserApi->Init();

		std::stringstream ss(std::stringstream::out);
		ss << "Market connected (" << front_addr << ")";
		
		logger.Info(ss.str());
		return true;
	}
	catch(std::exception& ex)
	{
		logger.Error("Failed to connect to market for quote, details is following");
		logger.Error(ex.what());
	}
	catch(...)
	{
		logger.Error("Unknown error encounted while connecting market for quote");
	}

	return false;
}

void CMarketAgent::DispatchQuotes()
{
	// Create a circular buffer with a capacity for 3 integers.
	boost::circular_buffer<int> cb(3);

	// Insert some elements into the buffer.
	cb.push_back(1);
	cb.push_back(2);
	cb.push_back(3);

	int a = cb[0];  // a == 1
	int b = cb[1];  // b == 2
	int c = cb[2];  // c == 3

	// The buffer is full now, pushing subsequent
	// elements will overwrite the front-most elements.

	cb.push_back(4);  // Overwrite 1 with 4.
	cb.push_back(5);  // Overwrite 2 with 5.

	// The buffer now contains 3, 4 and 5.

	a = cb[0];  // a == 3
	b = cb[1];  // b == 4
	c = cb[2];  // c == 5

	// Elements can be popped from either the front or the back.

	cb.pop_back();  // 5 is removed.
	cb.pop_front(); // 3 is removed.

	int d = cb[0];  // d == 4
}

bool CMarketAgent::Login( const char* brokerID, const char* userID, const char* password )
{

	std::stringstream ss(std::stringstream::out);
	ss << "Log in market (" << brokerID << " ," << userID << " ," << password << ")";
	logger.Trace(ss.str());

	int iResult = -1;

	try
	{
		CThostFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, brokerID);
		strcpy(req.UserID, userID);
		strcpy(req.Password, password);
		iResult = m_pUserApi->ReqUserLogin(&req, RequestIDIncrement());
	}
	catch (...)
	{
		logger.Error("Encouter error while logging in market for quote");
	}

	ss.str("");
	ss << "Login " << ((iResult == 0) ? "Succeeded" : "Failed");
	logger.Info(ss.str());
	return (iResult == 0);
}

void CMarketAgent::Logout( const char* brokerID, const char* userID )
{
	logger.Trace("Logging out");

	int nResult = -1;
	try{
		CThostFtdcUserLogoutField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, brokerID);
		strcpy(req.UserID, userID);

		nResult = m_pUserApi->ReqUserLogout(&req, RequestIDIncrement());

		if(nResult == 0)
		{
			logger.Info("Log out successfully");
		}
		else
		{
			logger.Error("Logout failed");
		}
	}
	catch(...)
	{
		logger.Error("Unknown error happent while logging out");
	}
}

bool CMarketAgent::SubscribeQuotes( QuoteListener* pQuoteListener )
{
	assert(pQuoteListener != NULL);
	assert(pQuoteListener->GetUuid().is_nil());

	// generate token for quote listener
	boost::uuids::random_generator gen;
	boost::uuids::uuid token = gen();

	pQuoteListener->SetUuid(token);

	// add it to listener map
	m_mapQuoteListeners.insert( pair<boost::uuids::uuid, QuoteListener*>(token, pQuoteListener));
	
	// know which symbols to subscribe
	vector<string>& regSymbols = pQuoteListener->GetSymbols();
	for(vector<string>::iterator iter = regSymbols.begin(); iter != regSymbols.end(); ++iter)
	{
		string s = *iter;
		// see if the symbol already registered
		SymbolListenerMap::iterator mapIter = m_mapSymbolListeners.find(s);
		if(mapIter == m_mapSymbolListeners.end())
		{
			// not registered yet, need to create a new vector for token;
			TokenSet tokenSet;
			tokenSet.insert(token);
			m_mapSymbolListeners.insert(pair<std::string, TokenSet>(s, tokenSet));
		}
		else
		{
			// already registered, just add the token to existing token vector
			(mapIter->second).insert(token);
		}
	}

	stringstream info(stringstream::out);
	info << "A new quote listener subscribing with uuid(' " << token << ")'";
	logger.Trace(info.str());

	bool sumbit = SubmitToServer();
	
	return true;
}


bool CMarketAgent::ChangeQuotes( QuoteListener* pQuoteListener )
{


	return true;
}

void CMarketAgent::UnsubscribeQuotes( boost::uuids::uuid& token )
{
	QuoteListenerMap::iterator foundIter = m_mapQuoteListeners.find(token);
	if(foundIter != m_mapQuoteListeners.end())
	{
		// found this token
		vector<string>& symbols = (foundIter->second)->GetSymbols();
		boost::uuids::uuid token = (foundIter->second)->GetUuid();

		for(vector<string>::iterator it = symbols.begin(); it != symbols.end(); ++it)
		{
			SymbolListenerMap::iterator foundSymbol = m_mapSymbolListeners.find(*it);
			if(foundSymbol != m_mapSymbolListeners.end())
			{
				TokenSet& tokens = foundSymbol->second;
				tokens.erase(token);

				if(tokens.size() == 0)
				{
					// if there is no token for the symbol, erase this from map
					m_mapSymbolListeners.erase(foundSymbol);
				}
			}
		}

		m_mapQuoteListeners.erase(foundIter);

		bool sumbit = SubmitToServer();
	}
}

bool CMarketAgent::SubscribeQuotes( std::set<std::string>& subcribeSet )
{
	bool retVal = false;

	// know symbols's count
	int symbCount = subcribeSet.size();

	if(symbCount > 0)
	{
		stringstream info(std::stringstream::out);
		info <<	"Subscribe quote for symbols as following " << endl;

		// new string array
		char** symbols = new char*[symbCount];

		// loop set for copying each symbol to array
		int idx = 0;
		for (set<string>::iterator iter = subcribeSet.begin(); 
			iter !=subcribeSet.end(); ++iter, ++idx)
		{
			symbols[idx] = new char[SYMBOL_MAX_LENGTH];
			strcpy(symbols[idx], iter->c_str());
			info << iter->c_str() << " ";
		}

		try	
		{
			logger.Trace(info.str());
			// 'REALLY' submit symbols to server side
			int iResult = m_pUserApi->SubscribeMarketData(symbols, symbCount);

			if(iResult == 0)
			{
				retVal = true;
				logger.Info("Subscribe quotes successfully.");
			}
			else
			{
				retVal = false;
				logger.Error("Failed to subscribe quotes");
			}
		}
		catch(...)
		{
			logger.Error("Unknown error happent while subscribe market data for quoting");
		}

		for(int i = 0; i < symbCount; ++i)
		{
			delete[] symbols[i];
		}
		delete[] symbols; 
	}

	return retVal;
}

bool CMarketAgent::UnsubscribeQuotes( std::set<std::string>& unsubcribeSet )
{
	bool retVal = false;

	// know symbols's count
	int symbCount = unsubcribeSet.size();

	if(symbCount > 0)
	{
		stringstream info(std::stringstream::out);
		info <<	"Unsubscribe following symbols' quote" << endl;

		// new string array
		char** symbols = new char*[symbCount];

		// loop set for copying each symbol to array
		int idx = 0;
		for (set<string>::iterator iter = unsubcribeSet.begin(); 
			iter !=unsubcribeSet.end(); ++iter, ++idx)
		{
			symbols[idx] = new char[SYMBOL_MAX_LENGTH];
			strcpy(symbols[idx], iter->c_str());
			info << iter->c_str() << " ";
		}

		try	
		{
			logger.Trace(info.str());
			// 'REALLY' submit symbols to server side
			int iResult = m_pUserApi->SubscribeMarketData(symbols, symbCount);

			if(iResult == 0)
			{
				retVal = true;
				logger.Info("Subscribe quotes successfully.");
			}
			else
			{
				retVal = false;
				logger.Error("Failed to subscribe quotes");
			}
		}
		catch(...)
		{
			logger.Error("Unknown error happent while subscribe market data for quoting");
		}

		for(int i = 0; i < symbCount; ++i)
		{
			delete[] symbols[i];
		}
		delete[] symbols; 
	}

	return retVal;
}

int CMarketAgent::RequestIDIncrement()
{
	return ++m_iRequestID;
}

bool CMarketAgent::SubmitToServer()
{
	bool retVal = false;

	// update m_subscribingSymbols to latest
	bool needSubmit = GetSubscribingSymbols();
	if(needSubmit)
	{
		stringstream info(std::stringstream::out);
		info <<	"Subscribe quote for symbols as following " << endl;

		// know symbols's count
		int symbCount = m_subscribingSymbols.size();

		if(symbCount > 0)
		{
			// new string array
			char** symbols = new char*[symbCount];

			// loop set for copying each symbol to array
			int idx = 0;
			for (set<string>::iterator iter = m_subscribingSymbols.begin(); 
				iter !=m_subscribingSymbols.end(); ++iter, ++idx)
			{
				symbols[idx] = new char[SYMBOL_MAX_LENGTH];
				strcpy(symbols[idx], iter->c_str());
				info << iter->c_str() << " ";
			}

			try	
			{
				logger.Trace(info.str());
				// 'REALLY' submit symbols to server side
				int iResult = m_pUserApi->SubscribeMarketData(symbols, symbCount);

				if(iResult == 0)
				{
					retVal = true;
					logger.Info("Subscribe quotes successfully.");
				}
				else
				{
					retVal = false;
					logger.Error("Failed to subscribe quotes");
				}
			}
			catch(...)
			{
				logger.Error("Unknown error happent while subscribe market data for quoting");
			}

			for(int i = 0; i < symbCount; ++i)
			{
				delete[] symbols[i];
			}
			delete[] symbols; 
		}
		else
		{
			// no symbols at all, just unsubcribe quotes
			// 'REALLY' submit symbols to server side
			int iResult = m_pUserApi->UnSubscribeMarketData();

			if(iResult == 0)
			{
				retVal = true;
				logger.Info("Subscribe quotes successfully.");
			}
			else
			{
				retVal = false;
				logger.Error("Failed to subscribe quotes");
			}
		}
	}
	else
	{
		logger.Trace("Subscribed symbols didn't change, no need to submit.");
		retVal = true;
	}

	return retVal;
}

bool CMarketAgent::GetSubscribingSymbols()
{
	bool needSubmit = true;

	// collect symbols to be subscribed
	set<string> currentSymbols;
	for (SymbolListenerMap::iterator iter = m_mapSymbolListeners.begin(); iter != m_mapSymbolListeners.end(); ++iter)
	{
		currentSymbols.insert(iter->first);
	}
	
	// assign vector size with larger between new one and old one
	int biggerSize = max(currentSymbols.size(), m_subscribingSymbols.size());
	vector<string> diff(biggerSize);
	vector<string>::iterator iterDiff = set_symmetric_difference(currentSymbols.begin(), currentSymbols.end(),
															m_subscribingSymbols.begin(), m_subscribingSymbols.end(),
															diff.begin());
	if(iterDiff == diff.begin())
	{
		// no difference at all, don't need to submit to server
		needSubmit = false;
	}
	else
	{
		// update subscribing symbols
		m_subscribingSymbols.swap(currentSymbols);
	}

	return needSubmit;
}
