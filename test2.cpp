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
/* ��֤��ĸ */
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

const int HORIZONTAL = 1; // ˮƽ�����ڴ����ۻ�ֱ��ͼʱ����Ҫ�õ�
const int VERTICAL = 0; // ��ֱ�����ڴ����ۻ�ֱ��ͼʱ����Ҫ�õ�
const string strCharacters[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E","F", "G", "H", "J", "K",
	"L", "M", "N", "P", "Q", "R", "S", "T", "U", "V",
	"W", "X", "Y", "Z",
	"��", "��", "��", "��", "��", "��", "��", "��", "��", "��",
	"��", "��", "��", "��", "��", "³", "��", "��", "��", "��",
	"��", "��", "��", "��", "��", "��", "��", "ԥ", "��", "��", "��"
};

const string strCharacters2[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E","F", "G", "H", "J", "K",
	"L", "M", "N", "P", "Q", "R", "S", "T", "U", "V",
	"W", "X", "Y", "Z","��", "��", "��", "��", "��", "��"
};
const string testStrCharacters[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E","F", "G", "H", "J", "K",
	"L", "M", "N", "P", "Q", "R", "S", "T", "U", "V",
	"W", "X", "Y", "Z" };

const int numCharacters = 65; // һ��65���ַ�
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
/* �ۻ�ֱ��ͼ */
Mat ProjectHistogram(Mat img, int t) {

	// �������ֱ����t = 0��sz = img.cols
	// ��֮��ѡȡ��������ֵ���Ա㴴����ȫ��������ͼ��ľ���
	int sz = (t) ? img.rows : img.cols;
	Mat mhist = Mat::zeros(1, sz, CV_32F); // mhist��һ��1 * sz�ľ���

										   // ����/�� ͳ�Ʒ�������ֵ�ĸ�������������mhist��
	for (int j = 0; j < sz; j++) {
		Mat data = (t) ? img.row(j) : img.col(j);
		mhist.at<float>(j) = countNonZero(data);
	}

	double min, max;
	min = max = 0.0;
	minMaxLoc(mhist, &min, &max); // �ҵ������е����ֵ���Ա��һ��

	if (max > 0) {
		// �����ÿһ��Ԫ�ض��������ֵ�������ǹ�һ������
		mhist.convertTo(mhist, -1, 1.0f / max, 0);
		return mhist;
	}
	return mhist;
}

/*
������������
ˮƽ�����ۻ�ֱ��ͼ + ��ֱ�����ۻ�ֱ��ͼ + �ͷֱ���ͼ��
*/
Mat features(Mat in, int sizeData) {

	// �ֱ���ˮƽ����ʹ�ֱ������ �����ۻ�ֱ��ͼ
	Mat vhist = ProjectHistogram(in, VERTICAL);
	Mat hhist = ProjectHistogram(in, HORIZONTAL);

	// �ͷֱ���ͼ��
	// �ͷֱ���ͼ���е�ÿһ�����ض���������������������
	Mat lowData;
	resize(in, lowData, Size(sizeData, sizeData));

	// �������������
	int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;
	Mat out = Mat::zeros(1, numCols, CV_32F); // ������������

											  // ����������ֵ
	int j = 0;
	// ���Ȱ�ˮƽ�����ۻ�ֱ��ͼ��ֵ���浽����������
	for (int i = 0; i < vhist.cols; i++) {
		out.at<float>(j) = vhist.at<float>(i);
		j++;
	}
	// Ȼ�����ֱ�����ۻ�ֱ��ͼ��ֵ���浽����������
	for (int i = 0; i < hhist.cols; i++) {
		out.at<float>(j) = hhist.at<float>(i);
		j++;
	}
	// ���ѵͷֱ���ͼ�������ֵ���浽����������
	for (int x = 0; x < lowData.cols; x++) {
		for (int y = 0; y < lowData.rows; y++) {
			out.at<float>(j) = (float)lowData.at<unsigned char>(x, y);
			j++;
		}
	}
	return out;
}

/*
ѵ����ʶ��
ע��Ϊ�˲��Է��㣬�Ұ�ѵ����ʶ��д��һ����������
��������£�Ӧ�õ�����װΪ����
*/
CvANN_MLP bp;
void train(Mat TrainingData, Mat classes, int nlayers) {
	// step1. ����ѵ������

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
	// step2. ����������
	Mat layers(1, 3, CV_32SC1);
	layers.at<int>(0) = TrainingData.cols;
	layers.at<int>(1) = nlayers;
	layers.at<int>(2) = numCharacters;
	bp.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);
	// step3. ѵ��
	Mat weights(1, TrainingData.rows, CV_32FC1, Scalar::all(1));
	bp.train(TrainingData, trainClasses, weights);

}
int classificationANN(Mat f) {


	// step4. Ԥ��
	// �������������Mat f

	Mat output(1, numCharacters, CV_32FC1);
	bp.predict(f, output); // ��ʼԤ��

	Point maxLoc;
	double maxVal;
	// outputΪһ��������������ÿһ��Ԫ�ط�ӳ��������������ÿ�����ĸ���
	minMaxLoc(output, 0, &maxVal, 0, &maxLoc); // �ҵ�������

											   // �����ַ���strCharacters[]�����е�����
	return maxLoc.x;
}
void trainOCR() {

	//diy����OCR.xml�ļ�
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
			resize(img, img, Size(charSize, charSize)); // ���µ�����С
			// ֱ��ͼ���⻯
			Mat src;
			equalizeHist(img, src);
			//imshow("�����⻯��ĻҶ�ͼ��", src);

			// CV_THRESH_BINARY_INV ��ǰ��ֵ����200ʱ������Ϊ255����������Ϊ0
			threshold(src, src, 200, 255, CV_THRESH_BINARY);
			//imshow("����ֵ�����ͼ��", src);

			//���Ͳ���
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

	//diy����OCR2.xml�ļ�
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
			resize(img, img, Size(charSize, charSize)); // ���µ�����С
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
	// ֱ��ͼ���⻯
	equalizeHist(src, src);
	//imshow("�����⻯��ĻҶ�ͼ��", src);

	// CV_THRESH_BINARY_INV ��ǰ��ֵ����200ʱ������Ϊ255����������Ϊ0
	threshold(src, src, 200, 255, CV_THRESH_BINARY);
	//imshow("����ֵ�����ͼ��", src);

	//���Ͳ���
	Mat element = getStructuringElement(0, Size(3, 3));
	Mat dst;
	dilate(src, dst, element);
	//imshow("�����ͺ��ͼ��", dst);
	return dst;
}
int main(int argc, char **argv)
{
	/*
	Mat srcImage = imread("chepaidd.jpg");      //����ͼ���ļ�
	imshow("lena", srcImage);               //��ʾͼƬ
	// ���ƺ�ʶ��
	Mat src = imread("chepaidd.jpg", 0);
	// ֱ��ͼ���⻯
	equalizeHist(src, src);
	imshow("�����⻯��ĻҶ�ͼ��", src);

	// CV_THRESH_BINARY_INV ��ǰ��ֵ����60ʱ������Ϊ255����������Ϊ0
	threshold(src, src, 200, 255, CV_THRESH_BINARY);
	imshow("����ֵ�����ͼ��", src);

	//���Ͳ���
	Mat element = getStructuringElement(0, Size(3, 3));
	Mat dst;
	dilate(src, dst, element);
	imshow("�����ͺ��ͼ��", dst);

	// Ϊ�˸��õػ��Ƴ�����������BGR��ͨ�����¶�ȡͼƬ�������Ϳ��Ի�������ɫ��
	Mat copy = imread("chepaidd.jpg");
	vector<vector<Point>> contours;

	// CV_RETR_EXTERNAL ֻ���������
	// CV_CHAIN_APPROX_NONE �洢����������
	findContours(dst, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++) {
		drawContours(copy, contours, i, Scalar(0, 255, 0), 1); // ��������
		Rect rect = boundingRect(contours[i]);
		rectangle(copy, rect, Scalar(0, 0, 255), 1);

		Mat roi(src, rect);
		if (verifyLetter(roi)) {
			// ����ͨ����֤�ľ���
			rectangle(copy, rect, Scalar(255, 0, 0), 2);

			// ����ͼ��
			imwrite(to_string(i) + ".jpg", roi);
		}
	}

	imshow("������������", copy);
	*/

	// Step0. Ԥ��������ͼ��
	//Mat src = imread("A.png", 0); // ��ȡ�Ҷ�ͼ
	//imshow("src", src);
	/*int h = src.rows;
	int w = src.cols;
	Mat transformMat = Mat::eye(2, 3, CV_32F); // �����Խ���
	int m = max(w, h);
	transformMat.at<float>(0, 2) = m / 2 - w / 2;
	transformMat.at<float>(1, 2) = m / 2 - h / 2;

	// ����任
	Mat warpImage(m, m, src.type());
	warpAffine(src, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));
	*/
	//Mat out;
	//resize(src, out, Size(charSize, charSize)); // ���µ�����С
	//imshow("out", out);
	//trainOCR();

	
	// Step1. ��ȡѵ������OCR.xml
	FileStorage fs;
	fs.open("OCR.xml", FileStorage::READ);
	Mat TrainingData;
	Mat Classes;
	fs["TrainingDataF5"] >> TrainingData;
	fs["classes"] >> Classes;
	train(TrainingData, Classes, 10);
	// Step2. ������������

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
	// Step3. ѵ�� + ���ԣ�д��һ���������ˣ�



	waitKey(0);

	
	return 0;
}
