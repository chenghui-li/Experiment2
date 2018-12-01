
#include <time.h>
#include <string>
#include "pr/include/Pipeline.h"
#include "ParkSystemWorker.h"

using std::to_string;
using std::stoll;

string getFormatTime(const string& curtime){
    string formattime;
    tm curYMD;
    long long curT = stoll(curtime);
    time_t c = (time_t)curT;
    localtime_r(&c, &curYMD);
    formattime = formattime + to_string(curYMD.tm_year + 1900) + "/" +
            to_string(curYMD.tm_mon + 1) + "/" +
            to_string(curYMD.tm_mday) + " "+
            to_string(curYMD.tm_hour) + ":"+
            to_string(curYMD.tm_min) + ":"+
            to_string(curYMD.tm_sec);

    return formattime;
}

string getFormatLastTime(const string& curtime){
    string formattime;
    long long curT = stoll(curtime);
    long long curTime = curT % 60 == 0 ? curT / 60 : curT / 60 + 1;
    formattime = formattime + to_string(curTime) + "分钟";

    return formattime;
}

string getCurTime() {
	time_t curtime;
	time(&curtime);
	
	string cur_time = to_string(curtime);
	return cur_time;
}

ParkSystemWorker::ParkSystemWorker() : prc_("/Users/mac/test2/pr/model/cascade.xml",
    "/Users/mac/test2/pr/model/HorizonalFinemapping.prototxt", "/Users/mac/test2/pr/model/HorizonalFinemapping.caffemodel",
    "/Users/mac/test2/pr/model/Segmentation.prototxt", "/Users/mac/test2/pr/model/Segmentation.caffemodel",
    "/Users/mac/test2/pr/model/CharacterRecognization.prototxt", "/Users/mac/test2/pr/model/CharacterRecognization.caffemodel"
) {
}
float ParkSystemWorker::getCurrentDiscount(){
    return db_.getCurDiscount();
}
bool ParkSystemWorker::init()
{
    return db_.connectWithDatabase("localhost", "3306", "root", "00000000", "mydb");
}

bool ParkSystemWorker::login(const string& workerID, const string& passwd) {
	workerID_ = workerID;
	return db_.verifyUandP("worker", workerID, passwd);
}

std::vector<std::string> ParkSystemWorker::submit(const string& plateNumber,bool &in) {
    if (db_.searchPlateNumber(plateNumber)){
        in = false;
        return leaving(plateNumber);
    }
    else{
        in = true;
        return parking(plateNumber);
    }
}

std::vector<std::string> ParkSystemWorker::parking(const string& plateNumber) {
    std::vector<std::string> result;

    string cur_time = getCurTime();

    if (db_.isVIP((plateNumber)))
        result.push_back("是会员");
    else
        result.push_back("不是会员");

    string lasttime = db_.getVIPLasttime(plateNumber);
    result.push_back(lasttime);
	db_.insertPlateNumer(plateNumber, workerID_, cur_time);

    return result;
}

vector<string> ParkSystemWorker::leaving(const string& plateNumber) {
    vector<string> result;
    string outTime = getCurTime();
	time_t curtime;
	time(&curtime);
	tm curYMD;
    localtime_r(&curtime,&curYMD);
	string inworkerID = db_.getInworkerID(plateNumber);
	string inTime = db_.getInTime(plateNumber);
	long long parkingTimeL = stoll(outTime) - stoll(inTime);

    result.push_back(getFormatTime(inTime));
    result.push_back(getFormatTime(outTime));
    result.push_back(getFormatLastTime(to_string(parkingTimeL)));


	int parkingTime = static_cast<int>(parkingTimeL / 3600);
	if (parkingTime < 1)
		parkingTime = 1;

	string lastTime = to_string(parkingTime);

	float parkingprice = getParkingPrice();
	float parkingFee = parkingTime * parkingprice;
    string isvip = "";
    string vipDiscount = "";
	if (isVIP(plateNumber)) {
        isvip = "是会员";
		float vipdiscount = getVIPDiscount(plateNumber);
        vipDiscount = to_string(vipdiscount);
        unsigned int location = vipDiscount.find('.');
        if (location <= vipDiscount.size())
            vipDiscount = vipDiscount.substr(0, location + 3);

		parkingFee *= vipdiscount;
	}
    else
        isvip = "不是会员";

	//将当前时间减去停入时间计算停车时间并计算费用

	db_.insertParkingInfo(plateNumber, inworkerID, workerID_,
		outTime, inTime, lastTime, parkingFee, to_string(curYMD.tm_year + 1900),
		to_string(curYMD.tm_mon + 1), to_string(curYMD.tm_mday));

	db_.deletePlateNumber(plateNumber);

    string pkFee = to_string(parkingFee);
    unsigned int location = vipDiscount.find('.');
    if (location <= pkFee.size())
        pkFee = pkFee.substr(0, location + 3);
    result.push_back(pkFee);
    result.push_back(isvip);
    result.push_back(vipDiscount);

    return result;
}

bool ParkSystemWorker::registerMembership(const string& plateNumber, const string& lasttime, const string& vipPrice) {
	if (isVIP(plateNumber)) {
		return false;
	}
	else {
		float discount = db_.getCurDiscount();
        long long lastTime = std::stoll(lasttime);
        db_.insertVIPInfo(plateNumber, to_string(lastTime), to_string(discount), workerID_, vipPrice);
	}
}

float ParkSystemWorker::getParkingPrice() {
	return db_.getParkingPrice();
}

float ParkSystemWorker::getVIPPrice() {
	return db_.getVIPPrice();
}

float ParkSystemWorker::getVIPDiscount(const string& plateNumber) {
	return db_.getVIPDiscount(plateNumber);
}

bool ParkSystemWorker::isVIP(const string& plateNumber) {
	return db_.isVIP(plateNumber);
}

string ParkSystemWorker::recognizePlate(const string &path) {
    cv::Mat plateImage = cv::imread(path.c_str());
	std::vector<pr::PlateInfo> res = prc_.RunPiplineAsImage(plateImage);
	float conf = 0;
	string plateNumber = "";
	for (auto st : res) {
		if (st.confidence>0.1) {
			plateNumber = st.getPlateName();
			std::cout << st.getPlateName() << " " << st.confidence << std::endl;
			conf += st.confidence;
		}
	}
	
	return plateNumber;
}
