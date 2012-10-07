#pragma once
#include <string>

class CDiffStrategy;

using namespace std;

CDiffStrategy* CreateStrategy(const string& name, const string& data);





