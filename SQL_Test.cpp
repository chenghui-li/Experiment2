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
		//OutputDebugStringA("���ݿ����ӳɹ�!");
		return true;
	}
	else {
		//OutputDebugStringA("���ݿ�����ʧ��!");
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
		//OutputDebugStringA("��ѯ�û���ʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("��ѯ�û����ɹ���");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_)))
		{
			//OutputDebugStringA("����Ϊ��");
			return false;
		}
		else {
			//OutputDebugStringA("���벻Ϊ��");

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
	string query = "update price set parkingprice = '" + std::to_string(parkPrice) + "'";//update��SQL��䣬���ڸ������ݱ��parkPrice���ֶ�
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("����ͣ������ʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("����ͣ�����óɹ���");
		return true;
	}
}

bool ParkSystemDatabase::modifyVIPPrice(float vipPrice) {
	string query = "update price set vipcost = '" + std::to_string(vipPrice) + "'";//update��SQL��䣬���ڸ������ݱ��vipPrice���ֶ�
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("���»�Ա��ʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("���»�Ա�ѳɹ���");
		return true;
	}
}

bool ParkSystemDatabase::modifyVIPDiscount(float vipDiscount) {
	string query = "update price set vipdiscount = '" + std::to_string(vipDiscount) + "'";//update��SQL��䣬���ڸ������ݱ��vipDiscount���ֶ�
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("���»�Ա�ۿ�ʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("���»�Ա�ۿ۳ɹ���");
		return true;
	}
}

bool ParkSystemDatabase::insertParkingInfo(const string& plateNumber, const string& inworkerID, const string& outworkerID,
		const string& outTime, const string& inTime, const string& lastTime, float parkingFee,
		const string& year, const string& month, const string& day) {
	//INSERT��SQL���, �������շѱ����һ����¼
    string query = "insert into allinfo values(0, '" + plateNumber + "'," + inTime + "," + outTime + ","
                    +lastTime + ",'" + inworkerID + "','" + outworkerID + "'," + std::to_string(parkingFee) + ",'"
					+year + "','" + month + "','" + day + "')";
    //mysql_query(&mysqlConnection_, "set names gb2312");
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("���복���շ���Ϣʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("���복���շ���Ϣ�ɹ���");
		return true;
	}
}

bool ParkSystemDatabase::insertPlateNumer(const string& plateNumber, const string& workerID, const string& time) {
	string query = "insert into carinfo values(0, '" + plateNumber + "','" + time + "','" + workerID + "')";//INSERT��SQL��䣬��������ʱͣ������복����ͣ����Ϣ
    //mysql_query(&mysqlConnection_, "set names gb2312");
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("ͣ��ʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("ͣ���ɹ���");
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
            workerid + "')";//INSERT��SQL��䣬�����Ա��Ϣ
    //mysql_query(&mysqlConnection_, "set names gb2312");
	if (mysql_query(&mysqlConnection_, query.c_str())) {
        //OutputDebugStringA("��ͨ��Աʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("��ͨ��Ա�ɹ���");
		return true;
	}
}

bool ParkSystemDatabase::deletePlateNumber(const string& plateNumber) {
	string query = "delete from carinfo where platenumber = '" + plateNumber + "'";//DELETE��SQL��䣬���ڴ���ʱͣ����ɾ����������Ϣ
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("��������ʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("��������ɹ���");
		return true;
	}
}

bool ParkSystemDatabase::searchPlateNumber(const string& plateNumber) {
	string query = "select * from carinfo where platenumber = '" + plateNumber + "'";//QUERY��ѯ��SQL��䣬���ڲ�ѯͣ�����ݱ����Ƿ��иó���
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("��ѯ����ʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("��ѯ���Ƴɹ���");

		if (mysqlRes_ = mysql_store_result(&mysqlConnection_))
		{
			MYSQL_ROW column;
			if (column = mysql_fetch_row(mysqlRes_)) {
			//	OutputDebugStringA("�����г���");
				return true;
			}
			else {
			//	OutputDebugStringA("�����޳���");
				return false;
			}
		}
	}
}

bool ParkSystemDatabase::isVIP(const string& plateNumber) {
	string query = "select * from vipinfo where platenumber = '" + plateNumber + "'";//QUERY��SQL��䣬���ڲ�ѯ��Ա�����Ƿ��д˺���
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("��ȡ��Ա����ʧ�ܣ�");
		return false;
	}
	else {
		//OutputDebugStringA("��ȡ��Ա���Ƴɹ���");

		if (mysqlRes_ = mysql_store_result(&mysqlConnection_)) {

			MYSQL_ROW column;
			if (column = mysql_fetch_row(mysqlRes_)) {
				//OutputDebugStringA("�ó����ǻ�Ա!");
				return true;
			}
			else {
				//OutputDebugStringA("�ó��Ʋ��ǻ�Ա��");
				return false;
			}
		}
	}
}

float ParkSystemDatabase::getParkingPrice() {
	string query = "select parkingprice from price";//QUERY��SQL��䣬���ڲ�ѯͣ����
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("��ȡͣ����ʧ�ܣ�");
	}
	else {
		//OutputDebugStringA("��ȡͣ���ѳɹ���");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
		//	OutputDebugStringA("ͣ����Ϊ��!");
		}
		else {
		//	OutputDebugStringA("ͣ���Ѳ�Ϊ�գ�");
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
	string query = "select vipcost from price";//QUERY��SQL��䣬���ڲ�ѯ��Ա��
	if (mysql_query(&mysqlConnection_, query.c_str())) {
		//OutputDebugStringA("��ȡ��Ա��ʧ�ܣ�");
	}
	else {
		//OutputDebugStringA("��ȡ��Ա�ѳɹ���");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("��Ա��Ϊ��!");
		}
		else {
		//	OutputDebugStringA("��Ա�Ѳ�Ϊ�գ�");
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
	string query = "select discount from vipinfo where platenumber = '" + plateNumber + "'";//QUERY��SQL��䣬���ڲ�ѯ��Ա�ۿ�
	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("��ȡ��Ա�ۿ�ʧ�ܣ�");
	}
	else {
	//	OutputDebugStringA("��ȡ��Ա�ۿ۳ɹ���");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("��Ա�ۿ�Ϊ��!");
		}
		else {
	//		OutputDebugStringA("��Ա�ۿ۲�Ϊ�գ�");
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
    //	OutputDebugStringA("��ȡ��Աlasttimeʧ�ܣ�");
    }
    else {
    //	OutputDebugStringA("��ȡ��Աlasttime�ɹ���");

        if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
    //		OutputDebugStringA("��ԱlasttimeΪ��!");
        }
        else {
    //		OutputDebugStringA("��Աlasttime��Ϊ�գ�");
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
	string query = "select vipdiscount from price";//QUERY��SQL��䣬���ڲ�ѯ��Ա�ۿ�
	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("��ȡ��Ա�ۿ�ʧ�ܣ�");
	}
	else {
	//	OutputDebugStringA("��ȡ��Ա�ۿ۳ɹ���");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("��Ա�ۿ�Ϊ��!");
		}
		else {
	//		OutputDebugStringA("��Ա�ۿ۲�Ϊ�գ�");
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

	string query_sum = "";//�������ȷ��query��SQL���,���ܳ��ܳ��������ܽ�������ֶ�
	string query_allInfo = "";//�������ȷ��query��SQL��䣬�������з�����������Ŀ
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
	//	OutputDebugStringA("��ȡ������Ϣʧ�ܣ�");
	}
	else {
	//	OutputDebugStringA("��ȡ������Ϣ�ɹ���");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("������ϢΪ��!");
		}
		else {
	//		OutputDebugStringA("������Ϣ��Ϊ�գ�");
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

			//��ѯ������Ŀ
			if (mysql_query(&mysqlConnection_, query_allInfo.c_str())) {
	//			OutputDebugStringA("��ȡ������Ϣʧ�ܣ�");
			}
			else {
	//			OutputDebugStringA("��ȡ������Ϣ�ɹ���");

				if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//				OutputDebugStringA("������ϢΪ��!");
				}
				else {
	//				OutputDebugStringA("������Ϣ��Ϊ�գ�");
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
	string query = "select workerid from carinfo where platenumber = '" + plateNumber + "'";//QUERY��SQL��䣬���ڲ�ѯworkerID

	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("��ȡͣ��ԱIDʧ�ܣ�");
	}
	else {
	//	OutputDebugStringA("��ȡͣ��ԱID�ɹ���");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("ͣ��ԱIDΪ��!");
		}
		else {
	//		OutputDebugStringA("ͣ��ԱID��Ϊ�գ�");
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
	string query = "select intime from carinfo where platenumber = '" + plateNumber + "'";//QUERY��SQL��䣬���ڲ�ѯworkerID
	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("��ȡͣ��ʱ��ʧ�ܣ�");
	}
	else {
	//	OutputDebugStringA("��ȡͣ��ʱ��ɹ���");

		if (!(mysqlRes_ = mysql_store_result(&mysqlConnection_))) {
	//		OutputDebugStringA("ͣ��ʱ��Ϊ��!");
		}
		else {
	//		OutputDebugStringA("ͣ��ʱ�䲻Ϊ�գ�");
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
	string query = "";//UPDATE��SQL��䣬���ڸ��»�Աʣ��ʱ��
	if (mysql_query(&mysqlConnection_, query.c_str())) {
	//	OutputDebugStringA("���»�Աʱ��ʧ�ܣ�");
		return false;
	}
	else {
	//	OutputDebugStringA("���»�Աʱ��ɹ���");
		return true;
	}
}
