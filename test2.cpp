#include "stdafx.h"
#include <highgui.h>
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <ml.hpp>
#include <string.h>
using namespace std;
using namespace cv;
/* 验证字母 */
bool verifyLetter(Mat r) {
	const float aspect = 45.0f / 77.0f;
	float charAspect = (float)r.cols / (float)r.rows;
	float error = 0.35;
	float minHeight = 15;
	float maxHeight = 28;
	float minAspect = 0.2;
	float maxAspect = aspect + aspect * error;
	float area = countNonZero(r);
	float bbArea = r.cols * r.rows;
	float percPixels = area / bbArea;
	return percPixels < 0.8 &&
		charAspect > minAspect &&
		charAspect < maxAspect &&
		r.rows >= minHeight && r.rows <= maxHeight;
}

const int HORIZONTAL = 1; // 水平方向，在创建累积直方图时，需要用到
const int VERTICAL = 0; // 竖直方向，在创建累积直方图时，需要用到
const string strCharacters[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E","F", "G", "H", "J", "K",
	"L", "M", "N", "P", "Q", "R", "S", "T", "U", "V",
	"W", "X", "Y", "Z",
	"藏", "川", "鄂", "甘", "赣", "贵", "桂", "黑", "沪", "吉",
	"冀", "津", "晋", "京", "辽", "鲁", "蒙", "闽", "宁", "青",
	"琼", "陕", "苏", "皖", "湘", "新", "渝", "豫", "粤", "云", "浙"
};

const string strCharacters2[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E","F", "G", "H", "J", "K",
	"L", "M", "N", "P", "Q", "R", "S", "T", "U", "V",
	"W", "X", "Y", "Z","沪", "京", "闽", "苏", "粤", "浙"
};
const string testStrCharacters[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E","F", "G", "H", "J", "K",
	"L", "M", "N", "P", "Q", "R", "S", "T", "U", "V",
	"W", "X", "Y", "Z" };

const int numCharacters = 65; // 一共65个字符
const int numCharacters2 = 40;
const int charSize = 20;
const int numFilesChars[] = {
	585, 613, 585, 628, 655, 609, 604, 605, 601, 623,
	683, 697, 683, 682, 695, 679, 692, 687, 700, 695,
	698, 697, 622, 691, 676, 695, 690, 695, 698, 688,
	686, 697, 697, 687, 23, 377, 745, 582, 155, 711,
	124, 104, 215, 259, 739, 740, 504, 756, 750, 180,
	546, 602, 107, 722, 44, 328, 829, 346, 777, 183,
	183, 30, 781, 558, 556 };

const int testNumFilesChars[] = {
	585, 613, 585, 628, 655, 609, 604, 605, 601, 623,
	683, 697, 683, 682, 695, 679, 692, 687, 700, 695,
	698, 697, 622, 691, 676, 695, 690, 695, 698, 688,
	686, 697, 697, 687,  };
const int numFilesChars2[] = {
	175,94,75,81,150,78,86,78,137,131,
	438,420,246,150,198,120,108,72,66,90,
	78,126,96,72,210,96,138,96,54,90,
	66,54,60,54,143,180,168,228,330,192
};
/* 累积直方图 */
Mat ProjectHistogram(Mat img, int t) {

	// 如果是竖直方向，t = 0，sz = img.cols
	// 总之，选取长宽的最大值，以便创建完全包含数字图像的矩阵
	int sz = (t) ? img.rows : img.cols;
	Mat mhist = Mat::zeros(1, sz, CV_32F); // mhist是一个1 * sz的矩阵

										   // 按行/列 统计非零像素值的个数，并保存在mhist中
	for (int j = 0; j < sz; j++) {
		Mat data = (t) ? img.row(j) : img.col(j);
		mhist.at<float>(j) = countNonZero(data);
	}

	double min, max;
	min = max = 0.0;
	minMaxLoc(mhist, &min, &max); // 找到矩阵中的最大值，以便归一化

	if (max > 0) {
		// 矩阵的每一个元素都除以最大值，这正是归一化操作
		mhist.convertTo(mhist, -1, 1.0f / max, 0);
		return mhist;
	}
	return mhist;
}

/*
创建特征矩阵
水平方向累积直方图 + 竖直方向累积直方图 + 低分辨率图像
*/
Mat features(Mat in, int sizeData) {

	// 分别在水平方向和垂直方向上 创建累积直方图
	Mat vhist = ProjectHistogram(in, VERTICAL);
	Mat hhist = ProjectHistogram(in, HORIZONTAL);

	// 低分辨率图像
	// 低分辨率图像中的每一个像素都将被保存在特征矩阵中
	Mat lowData;
	resize(in, lowData, Size(sizeData, sizeData));

	// 特征矩阵的列数
	int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;
	Mat out = Mat::zeros(1, numCols, CV_32F); // 创建特征矩阵

											  // 向特征矩阵赋值
	int j = 0;
	// 首先把水平方向累积直方图的值，存到特征矩阵中
	for (int i = 0; i < vhist.cols; i++) {
		out.at<float>(j) = vhist.at<float>(i);
		j++;
	}
	// 然后把竖直方向累积直方图的值，存到特征矩阵中
	for (int i = 0; i < hhist.cols; i++) {
		out.at<float>(j) = hhist.at<float>(i);
		j++;
	}
	// 最后把低分辨率图像的像素值，存到特征矩阵中
	for (int x = 0; x < lowData.cols; x++) {
		for (int y = 0; y < lowData.rows; y++) {
			out.at<float>(j) = (float)lowData.at<unsigned char>(x, y);
			j++;
		}
	}
	return out;
}

/*
训练和识别
注：为了测试方便，我把训练和识别写到一个函数里了
正常情况下，应该单独封装为函数
*/
CvANN_MLP bp;
void train(Mat TrainingData, Mat classes, int nlayers) {
	// step1. 生成训练数据

	Mat trainClasses;
	trainClasses.create(TrainingData.rows, numCharacters, CV_32FC1);
	for (int i = 0; i < trainClasses.rows; i++) {
		for (int j = 0; j < trainClasses.cols; j++) {
			if (j == classes.at<int>(i))
				trainClasses.at<float>(i, j) = 1;
			else
				trainClasses.at<float>(i, j) = 0;
		}
	}
	// step2. 创建分类器
	Mat layers(1, 3, CV_32SC1);
	layers.at<int>(0) = TrainingData.cols;
	layers.at<int>(1) = nlayers;
	layers.at<int>(2) = numCharacters;
	bp.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);
	// step3. 训练
	Mat weights(1, TrainingData.rows, CV_32FC1, Scalar::all(1));
	bp.train(TrainingData, trainClasses, weights);

}
int classificationANN(Mat f) {


	// step4. 预测
	// 处理输入的特征Mat f

	Mat output(1, numCharacters, CV_32FC1);
	bp.predict(f, output); // 开始预测

	Point maxLoc;
	double maxVal;
	// output为一个向量，向量的每一个元素反映了输入样本属于每个类别的概率
	minMaxLoc(output, 0, &maxVal, 0, &maxLoc); // 找到最大概率

											   // 返回字符在strCharacters[]数组中的索引
	return maxLoc.x;
}
void trainOCR() {

	//diy生成OCR.xml文件
	Mat classes;
	Mat trainingDataf5;
	Mat trainingDataf10;
	Mat trainingDataf15;
	Mat trainingDataf20;

	vector<int> trainingLabels;
	string path = "./data/";

	for (int i = 0; i < numCharacters; i++)
	{
		int numFiles = numFilesChars[i];
		for (int j = 0; j < numFiles; j++) {
			cout << "Character " << strCharacters[i] << " file: " << j << "\n";
			string ss = "";
			ss = ss + path + strCharacters[i] + "/" + to_string(j) + ".jpg";
			//cout << ss << endl;
			Mat img = imread(ss, 0);
			resize(img, img, Size(charSize, charSize)); // 重新调整大小
			// 直方图均衡化
			Mat src;
			equalizeHist(img, src);
			//imshow("【均衡化后的灰度图】", src);

			// CV_THRESH_BINARY_INV 当前点值大于200时，设置为255，否则设置为0
			threshold(src, src, 200, 255, CV_THRESH_BINARY);
			//imshow("【阈值化后的图像】", src);

			//膨胀操作
			Mat element = getStructuringElement(0, Size(3, 3));
			Mat dst;
			dilate(src, dst, element);
			//imshow("before", img);
			
			//imshow("after", img);
			Mat f5 = features(dst, 5);
			Mat f10 = features(dst, 10);
			Mat f15 = features(dst, 15);
			Mat f20 = features(dst, 20);

			trainingDataf5.push_back(f5);
			trainingDataf10.push_back(f10);
			trainingDataf15.push_back(f15);
			trainingDataf20.push_back(f20);
			trainingLabels.push_back(i);
		}
	}

	trainingDataf5.convertTo(trainingDataf5, CV_32FC1);
	trainingDataf10.convertTo(trainingDataf10, CV_32FC1);
	trainingDataf15.convertTo(trainingDataf15, CV_32FC1);
	trainingDataf20.convertTo(trainingDataf20, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);

	FileStorage fs("OCR.xml", FileStorage::WRITE);
	fs << "TrainingDataF5" << trainingDataf5;
	fs << "TrainingDataF10" << trainingDataf10;
	fs << "TrainingDataF15" << trainingDataf15;
	fs << "TrainingDataF20" << trainingDataf20;
	fs << "classes" << classes;
	fs.release();
}

void trainOCR2() {

	//diy生成OCR2.xml文件
	Mat classes;
	Mat trainingDataf5;
	Mat trainingDataf10;
	Mat trainingDataf15;
	Mat trainingDataf20;

	vector<int> trainingLabels;
	string path = "./data2/";

	for (int i = 0; i < numCharacters2; i++)
	{
		int numFiles = numFilesChars2[i];
		for (int j = 0; j < numFiles; j++) {
			//cout << "Character " << strCharacters2[i] << " file: " << j << "\n";
			string ss = "";
			ss = ss + path + strCharacters2[i] + "/" + to_string(j) + ".bmp";
			cout << ss << endl;
			Mat img = imread(ss, 0);
			//imshow("before", img);
			resize(img, img, Size(charSize, charSize)); // 重新调整大小
														//imshow("after", img);
			Mat f5 = features(img, 5);
			Mat f10 = features(img, 10);
			Mat f15 = features(img, 15);
			Mat f20 = features(img, 20);

			trainingDataf5.push_back(f5);
			trainingDataf10.push_back(f10);
			trainingDataf15.push_back(f15);
			trainingDataf20.push_back(f20);
			trainingLabels.push_back(i);
		}
	}

	trainingDataf5.convertTo(trainingDataf5, CV_32FC1);
	trainingDataf10.convertTo(trainingDataf10, CV_32FC1);
	trainingDataf15.convertTo(trainingDataf15, CV_32FC1);
	trainingDataf20.convertTo(trainingDataf20, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);

	FileStorage fs("OCR2.xml", FileStorage::WRITE);
	fs << "TrainingDataF5" << trainingDataf5;
	fs << "TrainingDataF10" << trainingDataf10;
	fs << "TrainingDataF15" << trainingDataf15;
	fs << "TrainingDataF20" << trainingDataf20;
	fs << "classes" << classes;
	fs.release();
}
Mat init(string str) {
	Mat src = imread(str, 0);
	resize(src, src, Size(charSize, charSize));
	// 直方图均衡化
	equalizeHist(src, src);
	//imshow("【均衡化后的灰度图】", src);

	// CV_THRESH_BINARY_INV 当前点值大于200时，设置为255，否则设置为0
	threshold(src, src, 200, 255, CV_THRESH_BINARY);
	//imshow("【阈值化后的图像】", src);

	//膨胀操作
	Mat element = getStructuringElement(0, Size(3, 3));
	Mat dst;
	dilate(src, dst, element);
	//imshow("【膨胀后的图像】", dst);
	return dst;
}
int main(int argc, char **argv)
{
	/*
	Mat srcImage = imread("chepaidd.jpg");      //加载图像文件
	imshow("lena", srcImage);               //显示图片
	// 车牌号识别
	Mat src = imread("chepaidd.jpg", 0);
	// 直方图均衡化
	equalizeHist(src, src);
	imshow("【均衡化后的灰度图】", src);

	// CV_THRESH_BINARY_INV 当前点值大于60时，设置为255，否则设置为0
	threshold(src, src, 200, 255, CV_THRESH_BINARY);
	imshow("【阈值化后的图像】", src);

	//膨胀操作
	Mat element = getStructuringElement(0, Size(3, 3));
	Mat dst;
	dilate(src, dst, element);
	imshow("【膨胀后的图像】", dst);

	// 为了更好地绘制出轮廓，按照BGR三通道从新读取图片，这样就可以画其他颜色了
	Mat copy = imread("chepaidd.jpg");
	vector<vector<Point>> contours;

	// CV_RETR_EXTERNAL 只检测外轮廓
	// CV_CHAIN_APPROX_NONE 存储所有轮廓点
	findContours(dst, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++) {
		drawContours(copy, contours, i, Scalar(0, 255, 0), 1); // 绘制轮廓
		Rect rect = boundingRect(contours[i]);
		rectangle(copy, rect, Scalar(0, 0, 255), 1);

		Mat roi(src, rect);
		if (verifyLetter(roi)) {
			// 绘制通过验证的矩形
			rectangle(copy, rect, Scalar(255, 0, 0), 2);

			// 保存图像
			imwrite(to_string(i) + ".jpg", roi);
		}
	}

	imshow("【绘制轮廓】", copy);
	*/

	// Step0. 预处理数字图像
	//Mat src = imread("A.png", 0); // 读取灰度图
	//imshow("src", src);
	/*int h = src.rows;
	int w = src.cols;
	Mat transformMat = Mat::eye(2, 3, CV_32F); // 创建对角阵
	int m = max(w, h);
	transformMat.at<float>(0, 2) = m / 2 - w / 2;
	transformMat.at<float>(1, 2) = m / 2 - h / 2;

	// 仿射变换
	Mat warpImage(m, m, src.type());
	warpAffine(src, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));
	*/
	//Mat out;
	//resize(src, out, Size(charSize, charSize)); // 重新调整大小
	//imshow("out", out);
	//trainOCR();

	
	// Step1. 读取训练数据OCR.xml
	FileStorage fs;
	fs.open("OCR.xml", FileStorage::READ);
	Mat TrainingData;
	Mat Classes;
	fs["TrainingDataF5"] >> TrainingData;
	fs["classes"] >> Classes;
	train(TrainingData, Classes, 10);
	// Step2. 创建特征矩阵

	while (1) {
		for (int i = 0; i < 34; i++) {
			string path = testStrCharacters[i] + ".png";

			Mat img = init(path);
			
			Mat f = features(img, 5);
			int index = classificationANN(f);
			cout << strCharacters2[index] << endl;
		}
		cout << "input please" << endl;
		string file;
		cin >> file;
		Mat img = init(file);
		Mat f = features(img, 5);
		int index = classificationANN(f);
		cout << strCharacters2[index] << endl;
	}
	// Step3. 训练 + 测试（写到一个函数里了）



	waitKey(0);

	
	return 0;
}
