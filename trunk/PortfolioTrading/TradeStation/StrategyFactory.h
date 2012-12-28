#pragma once
#include <string>

class CDiffStrategy;
class CPortfolio;

using namespace std;

CDiffStrategy* CreateStrategy(CPortfolio* pPortf, const string& name, const string& data);





