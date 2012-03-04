#pragma once

// Here we define our application severity levels.
enum severity_level
{
	trace,
	debug,
	info,
	warning,
	error,
	fatal
};

// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
	std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
{
	static const char* const str[] =
	{
		"trace",
		"debug",
		"info",
		"warning",
		"error",
		"fatal"
	};
	if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
		strm << str[lvl];
	else
		strm << static_cast< int >(lvl);
	return strm;
}

template< typename CharT, typename TraitsT >
inline std::basic_istream< CharT, TraitsT >& operator>> (
	std::basic_istream< CharT, TraitsT >& strm, severity_level &lvl)
{
	static const char* const str[] =
	{
		"trace",
		"debug",
		"info",
		"warning",
		"error",
		"fatal"
	};

	lvl = trace;

	std::string tmp;
	strm >> tmp;

	int len = (sizeof(str) / sizeof(*str));
	for(int i = 0; i < len; i++)
	{
		if(tmp == str[i])
		{
			lvl = static_cast<severity_level>(i);
			break;
		}
	}

	return strm;
}