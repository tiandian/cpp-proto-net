#include "StdAfx.h"
#include "MarketAgent.h"
#include "LogManager.h"
#include "Configuration.h"

#include <sstream>
#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

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
		string& s = *iter;
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


bool CMarketAgent::ChangeQuotes( QuoteListener* pQuoteListener, vector<string>& changeSymbols )
{
	boost::uuids::uuid& token = pQuoteListener->GetUuid();

	QuoteListenerMap::iterator foundIter = m_mapQuoteListeners.find(token);
	if(foundIter != m_mapQuoteListeners.end())
	{
		// found this token
		vector<string>& origSymbols = (foundIter->second)->GetSymbols();

		// Find out newly added symbols in newSymbols but not in origSymbols
		vector<string> added(changeSymbols.size());
		vector<string>::iterator addedIter = set_difference(changeSymbols.begin(), changeSymbols.end(),
															origSymbols.begin(), origSymbols.end(),
															added.begin());
		added.shrink_to_fit();

		for (vector<string>::iterator adIt = added.begin(); adIt != added.end(); ++adIt)
		{
			string& s = *adIt;
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

		// Find out those in origSymbols but not in newSymbols, remove them
		vector<string> toRemoved(origSymbols.size());
		vector<string>::iterator removedIter = set_difference(origSymbols.begin(), origSymbols.end(),
																changeSymbols.begin(), changeSymbols.end(),
																toRemoved.begin());
		toRemoved.shrink_to_fit();

		for (vector<string>::iterator delIt = toRemoved.begin(); delIt != toRemoved.end(); ++delIt)
		{
			string& s = *delIt;
			SymbolListenerMap::iterator foundSymbol = m_mapSymbolListeners.find(s);
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

		// The last, update to changed symbols set
		(foundIter->second)->SetSymbols(changeSymbols);

		// Finally, update to server
		bool sumbit = SubmitToServer();
	}

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
		// erase this quote listener
		m_mapQuoteListeners.erase(foundIter);

		bool sumbit = SubmitToServer();
	}
}

// Really subscribe symbols on market
bool CMarketAgent::SubmitSubscribes( std::vector<std::string>& subcribeArr )
{
	bool retVal = false;

	// know symbols's count
	int symbCount = subcribeArr.size();

	if(symbCount > 0)
	{
		stringstream info(std::stringstream::out);
		info <<	"Subscribe quote for symbols as following " << endl;

		// new string array
		char** symbols = new char*[symbCount];

		// loop set for copying each symbol to array
		int idx = 0;
		for (vector<string>::iterator iter = subcribeArr.begin(); 
			iter != subcribeArr.end(); ++iter, ++idx)
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

// Really unsubscribe symbols on market
bool CMarketAgent::SubmitUnsubscribes( std::vector<std::string>& unsubcribeArr )
{
	bool retVal = false;

	// know symbols's count
	int symbCount = unsubcribeArr.size();

	if(symbCount > 0)
	{
		stringstream info(std::stringstream::out);
		info <<	"Unsubscribe following symbols' quote" << endl;

		// new string array
		char** symbols = new char*[symbCount];

		// loop set for copying each symbol to array
		int idx = 0;
		for (vector<string>::iterator iter = unsubcribeArr.begin(); 
			iter !=unsubcribeArr.end(); ++iter, ++idx)
		{
			symbols[idx] = new char[SYMBOL_MAX_LENGTH];
			strcpy(symbols[idx], iter->c_str());
			info << iter->c_str() << " ";
		}

		try	
		{
			logger.Trace(info.str());
			// 'REALLY' unsubscribe symbols to server side
			int iResult = m_pUserApi->UnSubscribeMarketData(symbols, symbCount);

			if(iResult == 0)
			{
				retVal = true;
				logger.Info("Unsubscribe quotes successfully.");
			}
			else
			{
				retVal = false;
				logger.Error("Failed to unsubscribe quotes");
			}
		}
		catch(...)
		{
			logger.Error("Unknown error happent while Unsubscribe quoting");
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

	// update m_subscribingSymbols to latest and get subscribe array and unsubscribe array
	vector<string> subscribeArr, unsubscribeArr;
	bool needSubmit = GetUpdateSymbolSet(subscribeArr, unsubscribeArr);
	if(needSubmit)
	{
		bool succ = SubmitSubscribes(subscribeArr);

		succ = SubmitUnsubscribes(unsubscribeArr);
	}
	else
	{
		logger.Trace("Subscribed symbols didn't change, no need to submit.");
		retVal = true;
	}

	return retVal;
}

bool CMarketAgent::GetUpdateSymbolSet(std::vector<std::string>& subscribeArr, std::vector<std::string>& unsubscribeArr)
{
	bool needSubmit = true;

	// collect symbols to be subscribed
	set<string> currentSymbols;
	for (SymbolListenerMap::iterator iter = m_mapSymbolListeners.begin(); iter != m_mapSymbolListeners.end(); ++iter)
	{
		currentSymbols.insert(iter->first);
	}
	
	// the largest possible size of subscribe array will be currentSymbols' size
	subscribeArr.resize(currentSymbols.size());
	vector<string>::iterator subscribeDiff = set_difference(currentSymbols.begin(), currentSymbols.end(),
															m_subscribingSymbols.begin(), m_subscribingSymbols.end(),
															subscribeArr.begin());
	subscribeArr.shrink_to_fit();

	// the same thing with unsubscribeArr and m_subscribingSymbols which is last time of subscribing symbols
	unsubscribeArr.resize(m_subscribingSymbols.size());
	vector<string>::iterator unsubDiff = set_difference(m_subscribingSymbols.begin(), m_subscribingSymbols.end(),
														currentSymbols.begin(), currentSymbols.end(),
														unsubscribeArr.begin());
	unsubscribeArr.shrink_to_fit();

	if(subscribeArr.size() == 0 && unsubscribeArr.size() == 0)
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
