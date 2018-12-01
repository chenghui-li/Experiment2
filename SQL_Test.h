#pragma once
#include <iostream>
#include <vector>
#include <mysql.h>
using std::string;
using std::vector;

class ParkSystemDatabase {
public:
	bool connectWithDatabase(const string& ip, const string& port, const string& username,
		const string& passwd, const string& dbname);
	void releaseConnection();

	bool verifyUandP(const string& usertype, const string& username, const string& passwd);//��֤�û�������
	bool modifyVIPPrice(float vipPrice);
	bool modifyParkingPrice(float parkPrice);
	bool modifyVIPDiscount(float vipDiscount);
	bool searchPlateNumber(const string& plateNumber);//�����ݿ��������Ƿ��г��ƣ����û�У������parking���������leaving
	bool insertPlateNumer(const string& plateNumber, const string& workerID, const string& time);
    bool insertVIPInfo(const string& plateNumber, const string& lastTime, const string& discount, const string& workerid, const string& vipPrice);
	bool deletePlateNumber(const string& plateNumber);
	bool insertParkingInfo(const string& plateNumber, const string& inworkerID, const string& outworkerID,
		const string& outTime, const string& inTime, const string& lastTime, float parkingFee,
		const string& year, const string& month, const string& day);
	bool isVIP(const string& plateNumber);

	vector<vector<string>> getSummaryInfo(const string& st_year, const string& ed_year,
		const string& st_month, const string& ed_month,
		const string& st_day, const string& ed_day);
	string getInworkerID(const string& plateNumber);//ȡ����Ӧ���Ƶ�ͣ��Ա��ID
	string getInTime(const string& plateNumber);//ȡ�ó��Ƶ�ͣ��ʱ��
	float getParkingPrice();
	float getVIPDiscount(const string& plateNumber);
	float getCurDiscount();
	float getVIPPrice();
    string getVIPLasttime(const string& plateNumber);
	bool updateVIPTime();//���ڸ��»�Ա��ʣ��ʱ��

private:
	MYSQL mysqlConnection_;
	MYSQL_RES *mysqlRes_;
};
