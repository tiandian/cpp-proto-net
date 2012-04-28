#pragma once

#include <string>

using namespace std;

class CQuote
{
public:
	CQuote(void);
	~CQuote(void);

	void set_symbol(char* v){ _symbol = v; }
	const string& get_symbol(){ return _symbol; }

	void set_trading_day(char* v){ _trading_day = v; }
	const string& get_trading_day(){ return _trading_day; }

	void set_last(double v){ _last = v; }
	double get_last(){ return _last; }

	void set_prev_close(double v){ _prev_close = v; }
	double get_prev_close(){ return _prev_close; }

	void set_open(double v){ _open = v; }
	double get_open(){ return _open; }

	void set_high(double v){ _high = v; }
	double get_high(){ return _high; }

	void set_low(double v){ _low = v; }
	double get_low(){ return _low; }

	void set_volume(int v){ _volume = v; }
	int get_volume(){ return _volume; }

	void set_turnover(double v){ _turnover = v; }
	double get_turnover(){ return _turnover; }

	void set_close(double v){ _close = v; }
	double get_close(){ return _close; }

	void set_update_time(char* v){ _update_time = v; }
	const string& get_update_time(){ return _update_time; }
	
	void set_update_millisec(int v){ _update_millisec = v; }
	int get_update_millisec(){ return _update_millisec; }

	void set_bid(double v){ _bid = v; }
	double get_bid(){ return _bid; }

	void set_bid_size(int v){ _bid_size = v; }
	int get_bid_size(){ return _bid_size; }

	void set_ask(double v){ _ask = v; }
	double get_ask(){ return _ask; }

	void set_ask_size(int v){ _ask_size = v; }
	int get_ask_size(){ return _ask_size; }

private:
	string _symbol;
	string _trading_day;
	double _last;
	double _prev_close;
	double _open;
	double _high;
	double _low;
	int _volume;
	double _turnover;
	double _close;
	string _update_time;
	int _update_millisec;

	double _bid;
	int _bid_size;
	double _ask;
	int _ask_size;
};

