#include "SQL_Test.h"
#include <string>
#include <string.h>
#include <time.h>


long long GetTick(const string& year, const string& month, const string& day, const string& hour, const string& min, const string& second)
{
	struct tm stm;
	int iY, iM, iD, iH, iMin, iS;

	memset(&stm, 0, sizeof(stm));

	iY = atoi(year.c_str());
	iM = atoi(month.c_str());
	iD = atoi(day.c_str());
    iH = atoi(hour.c_str());
    iMin = atoi(min.c_str());
    iS = atoi(second.c_str());

	stm.tm_year = iY - 1900;
	stm.tm_mon = iM - 1;
	stm.tm_mday = iD;
	stm.tm_hour = iH;
	stm.tm_min = iMin;
	stm.tm_sec = iS;

	return mktime(&stm);
}

bool ParkSystemDatabase::connectWithDatabase(const string& ip, const string& port, const string& username,
	const string& passwd, const string& dbname) {
	if (ip == "" || port == "" || username == "" || dbname == "")
		return false;

	mysql_init(&mysqlConnection_);

	unsigned int port_num = static_cast<unsigned int>(atoi(port.c_str()));

	if (mysql_real_connect(&mysqlConnection_, ip.c_str(), username.c_str(),
		passwd.c_str(), dbname.c_str(), port_num, nullptr, 0)) {
		//OutputDebugStringA("数据库连接成功!");
		return true;
	}
	else {
		//OutputDebugStringA("数据库连接失败!");
		return false;
	}
}

void ParkSystemDatabase::releaseConnection() {
	if (mysqlRes_)
		mysql_free_result(mysqlRes_);
	
	mysql_close(&mysqlConnection_);
}

bool ParkSystemDatabase::verifyUandP(const string& usertype, const string& username, const string& passwd) {
	string query = "";
	if (usertype == "admin") {
		query = "select password from admin where adminid = '" + username + "'";
	}
	else if (usertype == "worker") {
		query = "select password from worker where workerid = '" + username + "'";
	}

	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("查询用户名失败！");
		return false;
	}
	else {
		//OutputDebugStringA("查询用户名成功！");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_)))
		{
			//OutputDebugStringA("密码为空");
			return false;
		}
		else {
			//OutputDebugStringA("密码不为空");

			MYSQL_ROW column;
			if (column = mysql_fetch_row(mysqlRes_)) {
				if (passwd == column[0])
					return true;
				else
					return false;
			}
			else
				return false;
		}
	}
}

bool ParkSystemDatabase::modifyParkingPrice(float parkPrice) {
	string query = "update price set parkingprice = '" + std::to_string(parkPrice) + "'";//update的SQL语句，用于更新数据表的parkPrice的字段
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("更新停车费用失败！");
		return false;
	}
	else {
		//OutputDebugStringA("更新停车费用成功！");
		return true;
	}
}

bool ParkSystemDatabase::modifyVIPPrice(float vipPrice) {
	string query = "update price set vipcost = '" + std::to_string(vipPrice) + "'";//update的SQL语句，用于更新数据表的vipPrice的字段
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("更新会员费失败！");
		return false;
	}
	else {
		//OutputDebugStringA("更新会员费成功！");
		return true;
	}
}

bool ParkSystemDatabase::modifyVIPDiscount(float vipDiscount) {
	string query = "update price set vipdiscount = '" + std::to_string(vipDiscount) + "'";//update的SQL语句，用于更新数据表的vipDiscount的字段
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("更新会员折扣失败！");
		return false;
	}
	else {
		//OutputDebugStringA("更新会员折扣成功！");
		return true;
	}
}

bool ParkSystemDatabase::insertParkingInfo(const string& plateNumber, const string& inworkerID, const string& outworkerID,
		const string& outTime, const string& inTime, const string& lastTime, float parkingFee,
		const string& year, const string& month, const string& day) {
	//INSERT的SQL语句, 用于向收费表插入一条记录
    string query = "insert into allinfo values(0, '" + plateNumber + "'," + inTime + "," + outTime + ","
                    +lastTime + ",'" + inworkerID + "','" + outworkerID + "'," + std::to_string(parkingFee) + ",'"
					+year + "','" + month + "','" + day + "')";
    //mysql_query(&mysqlConnection_, "set names gb2312");
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("插入车辆收费信息失败！");
		return false;
	}
	else {
		//OutputDebugStringA("插入车辆收费信息成功！");
		return true;
	}
}

bool ParkSystemDatabase::insertPlateNumer(const string& plateNumber, const string& workerID, const string& time) {
	string query = "insert into carinfo values(0, '" + plateNumber + "','" + time + "','" + workerID + "')";//INSERT的SQL语句，用于向临时停车表插入车辆的停车信息
    //mysql_query(&mysqlConnection_, "set names gb2312");
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("停车失败！");
		return false;
	}
	else {
		//OutputDebugStringA("停车成功！");
		return true;
	}
}

bool ParkSystemDatabase::insertVIPInfo(const string& plateNumber, const string& lastTime,
                                       const string& vipDiscount, const string& workerid, const string& vipPrice) {
    time_t curtime;
    time(&curtime);
    tm curYMD;
    localtime_r(&curtime,&curYMD);
    long long lasttime = GetTick(std::to_string(curYMD.tm_year + std::stoi(lastTime) + 1900),
                                 std::to_string(curYMD.tm_mon + 1),
                                 std::to_string(curYMD.tm_mday),
                                 std::to_string(curYMD.tm_hour),
                                 std::to_string(curYMD.tm_min),
                                 std::to_string(curYMD.tm_sec));
    float vipprice = std::stof(vipPrice);
    long long bgtime = (long long)curtime;
    string query = "insert into vipinfo values('" +
            plateNumber + "'," +
            std::to_string(bgtime) + ",'" +
            std::to_string(lasttime) + "'," +
            std::to_string(vipprice) + ",'" +
            vipDiscount + "','" +
            workerid + "')";//INSERT的SQL语句，插入会员信息
    //mysql_query(&mysqlConnection_, "set names gb2312");
	if (mysql_query(&mysqlConnection_, query.c_str())) {
        //OutputDebugStringA("开通会员失败！");
		return false;
	}
	else {
		//OutputDebugStringA("开通会员成功！");
		return true;
	}
}

bool ParkSystemDatabase::deletePlateNumber(const string& plateNumber) {
	string query = "delete from carinfo where platenumber = '" + plateNumber + "'";//DELETE的SQL语句，用于从临时停车表删除车辆的信息
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("车辆出库失败！");
		return false;
	}
	else {
		//OutputDebugStringA("车辆出库成功！");
		return true;
	}
}

bool ParkSystemDatabase::searchPlateNumber(const string& plateNumber) {
	string query = "select * from carinfo where platenumber = '" + plateNumber + "'";//QUERY查询的SQL语句，用于查询停车数据表中是否有该车牌
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("查询车牌失败！");
		return false;
	}
	else {
		//OutputDebugStringA("查询车牌成功！");

		if (mysqlRes_ = mysql_store_result(&mysqlConnection_))
		{
			MYSQL_ROW column;
			if (column = mysql_fetch_row(mysqlRes_)) {
			//	OutputDebugStringA("表内有车牌");
				return true;
			}
			else {
			//	OutputDebugStringA("表内无车牌");
				return false;
			}
		}
	}
}

bool ParkSystemDatabase::isVIP(const string& plateNumber) {
	string query = "select * from vipinfo where platenumber = '" + plateNumber + "'";//QUERY的SQL语句，用于查询会员表内是否有此号码
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("获取会员车牌失败！");
		return false;
	}
	else {
		//OutputDebugStringA("获取会员车牌成功！");

		if (mysqlRes_ = mysql_store_result(&mysqlConnection_)) {

			MYSQL_ROW column;
			if (column = mysql_fetch_row(mysqlRes_)) {
				//OutputDebugStringA("该车牌是会员!");
				return true;
			}
			else {
				//OutputDebugStringA("该车牌不是会员！");
				return false;
			}
		}
	}
}

float ParkSystemDatabase::getParkingPrice() {
	string query = "select parkingprice from price";//QUERY的SQL语句，用于查询停车费
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("获取停车费失败！");
	}
	else {
		//OutputDebugStringA("获取停车费成功！");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
		//	OutputDebugStringA("停车费为空!");
		}
		else {
		//	OutputDebugStringA("停车费不为空！");
			MYSQL_ROW column;
			float parkPrice = 0.0f;
			if (column = mysql_fetch_row(mysqlRes_)) {
				parkPrice = atof(column[0]);
			}
			
			return parkPrice;
		}
	}
}

float ParkSystemDatabase::getVIPPrice()
{
	string query = "select vipcost from price";//QUERY的SQL语句，用于查询会员费
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("获取会员费失败！");
	}
	else {
		//OutputDebugStringA("获取会员费成功！");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("会员费为空!");
		}
		else {
		//	OutputDebugStringA("会员费不为空！");
			MYSQL_ROW column;
			float vipPrice = 0.0f;
			if (column = mysql_fetch_row(mysqlRes_)) {
				vipPrice = atof(column[0]);
			}

			return vipPrice;
		}
	}
} 

float ParkSystemDatabase::getVIPDiscount(const string& plateNumber) {
	string query = "select discount from vipinfo where platenumber = '" + plateNumber + "'";//QUERY的SQL语句，用于查询会员折扣
	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("获取会员折扣失败！");
	}
	else {
	//	OutputDebugStringA("获取会员折扣成功！");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("会员折扣为空!");
		}
		else {
	//		OutputDebugStringA("会员折扣不为空！");
			MYSQL_ROW column;
			float vipDiscount = 0.0f;
			if (column = mysql_fetch_row(mysqlRes_)) {
				vipDiscount = atof(column[0]);
			}

			return vipDiscount;
		}
	}
}

std::string ParkSystemDatabase::getVIPLasttime(const string& plateNumber) {
    string query = "select lasttime from vipinfo where platenumber = '" + plateNumber + "'";
    if (mysql_query(&mysqlConnection_, query.c_str())) {
    //	OutputDebugStringA("获取会员lasttime失败！");
    }
    else {
    //	OutputDebugStringA("获取会员lasttime成功！");

        if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
    //		OutputDebugStringA("会员lasttime为空!");
        }
        else {
    //		OutputDebugStringA("会员lasttime不为空！");
            MYSQL_ROW column;
            std::string tmp_time;
            std::string viplasttime;
            if (column = mysql_fetch_row(mysqlRes_)) {
                tmp_time = column[0];
                if (tmp_time.size() != 0){
                    tm cur_YMD;
                    long long curtime = std::stoll(tmp_time);
                    time_t c = (time_t)curtime;
                    localtime_r(&c, &cur_YMD);
                    viplasttime = std::to_string(cur_YMD.tm_year + 1900) + "/" +
                            std::to_string(cur_YMD.tm_mon + 1) + "/" +
                            std::to_string(cur_YMD.tm_mday) + " " +
                            std::to_string(cur_YMD.tm_hour) + ":" +
                            std::to_string(cur_YMD.tm_min) + ":" +
                            std::to_string(cur_YMD.tm_sec);
                }
                else
                    viplasttime = tmp_time;
            }

            return viplasttime;
        }
    }
}

float ParkSystemDatabase::getCurDiscount() {
	string query = "select vipdiscount from price";//QUERY的SQL语句，用于查询会员折扣
	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("获取会员折扣失败！");
	}
	else {
	//	OutputDebugStringA("获取会员折扣成功！");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("会员折扣为空!");
		}
		else {
	//		OutputDebugStringA("会员折扣不为空！");
			MYSQL_ROW column;
			float vipDiscount = 0.0f;
			if (column = mysql_fetch_row(mysqlRes_)) {
				vipDiscount = atof(column[0]);
			}

			return vipDiscount;
		}
	}
}

vector<vector<string>> ParkSystemDatabase::getSummaryInfo(const string& st_year, const string& ed_year,
	const string& st_month, const string& ed_month,
	const string& st_day, const string& ed_day) {
	vector<vector<string>> query_result;

	string query_sum = "";//根据情况确定query的SQL语句,汇总成总车辆数和总金额两个字段
	string query_allInfo = "";//根据情况确定query的SQL语句，返回所有符合条件的条目
    string query_vipsum = "";

    long long low_time = GetTick(st_year, st_month, st_day, "0", "0", "0");
    long long high_time = GetTick(ed_year, ed_month, ed_day, "23", "59", "59");
	string start_time = std::to_string(low_time);
	string end_time = std::to_string(high_time);
    query_vipsum = "select SUM(vipprice) from vipinfo where begtime >= " + start_time + " and begtime <= " + end_time;
    std::cout << query_vipsum << std::endl;
    query_sum = "select SUM(price), COUNT(*) from allinfo where outtime >= " + start_time + " and outtime <= " + end_time;
    std::cout << query_sum << std::endl;
    query_allInfo = "select * from allinfo where outtime >= " + start_time + " and outtime <= " + end_time;
    std::cout << query_allInfo << std::endl;

    string vipIncome = "";
    if (mysql_query(&mysqlConnection_, query_vipsum.c_str())){
        //chaxunshibai
    }
    else{
        if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))){
            //huizongcuowu
        }
        else{
            MYSQL_ROW column;
            if (column = mysql_fetch_row(mysqlRes_)){
                if (column[0] != nullptr)
                    vipIncome = column[0];
                else
                    vipIncome = "0";
            }
        }
    }


	if (mysql_query(&mysqlConnection_, query_sum.c_str())) {
	//	OutputDebugStringA("获取汇总信息失败！");
	}
	else {
	//	OutputDebugStringA("获取汇总信息成功！");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("汇总信息为空!");
		}
		else {
	//		OutputDebugStringA("汇总信息不为空！");
			MYSQL_ROW column;
			vector<string> sum_info;
            if (column = mysql_fetch_row(mysqlRes_)) {
                    string fee;
                    if (column[0] != nullptr)
                        fee = column[0];
                    else
                        fee = "0";
                    unsigned int location = fee.find('.');
                    if (location <= fee.size())
                        fee = fee.substr(0, location + 2);
                    sum_info.push_back(fee);
                    sum_info.push_back(column[1]);
                    sum_info.push_back(vipIncome);
			}
			query_result.push_back(sum_info);

			//查询所有条目
			if (mysql_query(&mysqlConnection_, query_allInfo.c_str())) {
	//			OutputDebugStringA("获取所有信息失败！");
			}
			else {
	//			OutputDebugStringA("获取所有信息成功！");

				if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//				OutputDebugStringA("所有信息为空!");
				}
				else {
	//				OutputDebugStringA("所有信息不为空！");
                    MYSQL_ROW column;

					tm cur_YMD;
					while (column = mysql_fetch_row(mysqlRes_)) {
						vector<string> per_info;
						per_info.push_back(column[1]);
						
						time_t intime = std::stoll(column[2]);
						time_t outtime = std::stoll(column[3]);
						string tmp_time = "";
                        localtime_r( &intime,&cur_YMD);
						per_info.push_back(tmp_time + std::to_string(cur_YMD.tm_year+1900) + "/" + std::to_string(cur_YMD.tm_mon+1)+ "/" + std::to_string(cur_YMD.tm_mday));
                        localtime_r( &outtime,&cur_YMD);
						per_info.push_back(tmp_time + std::to_string(cur_YMD.tm_year+1900) + "/" + std::to_string(cur_YMD.tm_mon+1) + "/" + std::to_string(cur_YMD.tm_mday));

						per_info.push_back(column[4]);
						per_info.push_back(column[5]);
						per_info.push_back(column[6]);
						per_info.push_back(column[7]);

						query_result.push_back(per_info);
					}
				}
			}
		}
	}

	return query_result;
}

string ParkSystemDatabase::getInworkerID(const string& plateNumber) {
	string query = "select workerid from carinfo where platenumber = '" + plateNumber + "'";//QUERY的SQL语句，用于查询workerID

	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("获取停车员ID失败！");
	}
	else {
	//	OutputDebugStringA("获取停车员ID成功！");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("停车员ID为空!");
		}
		else {
	//		OutputDebugStringA("停车员ID不为空！");
			MYSQL_ROW column;
			string inworkerID = "";
			if (column = mysql_fetch_row(mysqlRes_)) {
				inworkerID = column[0];
			}

			return inworkerID;
		}
	}
}

string ParkSystemDatabase::getInTime(const string& plateNumber) {
	string query = "select intime from carinfo where platenumber = '" + plateNumber + "'";//QUERY的SQL语句，用于查询workerID
	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("获取停车时间失败！");
	}
	else {
	//	OutputDebugStringA("获取停车时间成功！");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("停车时间为空!");
		}
		else {
	//		OutputDebugStringA("停车时间不为空！");
			MYSQL_ROW column;
			string inTime = "";
			if (column = mysql_fetch_row(mysqlRes_)) {
				inTime = column[0];
			}

			return inTime;
		}
	}
}


bool ParkSystemDatabase::updateVIPTime() {
	string query = "";//UPDATE的SQL语句，用于更新会员剩余时间
	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("更新会员时间失败！");
		return false;
	}
	else {
	//	OutputDebugStringA("更新会员时间成功！");
		return true;
	}
}
