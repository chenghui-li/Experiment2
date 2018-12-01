#pragma once
#include "SQL_Test.h"

using std::string;

class ParkSystemManager
{
public:
	bool init();
	bool login(const string& username, const string& passwd);
	vector<vector<string>> checkSummaryInfo(const string& st_year, const string& ed_year,
		const string& st_month, const string& ed_month,
		const string& st_day, const string& ed_day);
	bool modifyParkingPrice(float parkPrice);
	bool modifyVIPPrice(float vipPrice);
	bool modifyVIPDiscount(float vipDiscount);
    float getCurrentDiscount();
    float getCurrentPrice();
    float getParkingPrice();
private:
	ParkSystemDatabase db_;
};

