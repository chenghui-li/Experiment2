
#include "ParkSystemManager.h"

bool ParkSystemManager::init() {
    return db_.connectWithDatabase("localhost", "3306", "root", "00000000", "mydb");
}

bool ParkSystemManager::login(const string& username, const string& passwd)
{
	return db_.verifyUandP("admin", username, passwd);
}

vector<vector<string>> ParkSystemManager::checkSummaryInfo(const string& st_year, const string& ed_year,
	const string& st_month, const string& ed_month,
	const string& st_day, const string& ed_day)
{
	return db_.getSummaryInfo(st_year, ed_year, st_month, ed_month, st_day, ed_day);
}

bool ParkSystemManager::modifyParkingPrice(float parkPrice) {
	return db_.modifyParkingPrice(parkPrice);
}

bool ParkSystemManager::modifyVIPPrice(float vipPrice) {
	return db_.modifyVIPPrice(vipPrice);
}

bool ParkSystemManager::modifyVIPDiscount(float vipDiscount) {
	return db_.modifyVIPDiscount(vipDiscount);
}
float ParkSystemManager::getCurrentDiscount(){
    return db_.getCurDiscount();
}
float ParkSystemManager::getCurrentPrice(){
    return db_.getVIPPrice();
}
float ParkSystemManager::getParkingPrice(){
    return db_.getParkingPrice();
}
