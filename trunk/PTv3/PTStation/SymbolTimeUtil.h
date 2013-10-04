#pragma once

boost::chrono::seconds ParseTimeString(const char* time);
boost::chrono::seconds ParseTimeString(const string& time);

bool isSymbolIF(const string& symbol);
int GetIndexFromTime(const boost::chrono::seconds& baseTp, const boost::chrono::seconds& timePoint, int precision);
string GetISOTimeString(const boost::chrono::seconds& timepoint);

