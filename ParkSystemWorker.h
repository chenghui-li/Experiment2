#pragma once
#include "SQL_Test.h"
#include "include/Pipeline.h"
static string workerID_ = "";
class ParkSystemWorker
{
public:
	ParkSystemWorker();
	bool init();
	bool login(const string& workerID, const string& passwd);
	string recognizePlate(const string &path);
    std::vector<std::string> submit(const string& plateNumber,bool &in);

	float getParkingPrice();
    bool registerMembership(const string& plateNumber, const string& lasttime, const string& vipPrice);//办理会员的函数
	float getVIPPrice();
	float getVIPDiscount(const string& plateNumber);

    float getCurrentDiscount();
private:
    std::vector<std::string> parking(const string& plateNumber);//获取时间戳
    std::vector<std::string> leaving(const string& plateNumber);//获取时间戳
	bool isVIP(const string& plateNumber);


private:

	ParkSystemDatabase db_;
	pr::PipelinePR prc_;
};

