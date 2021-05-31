#pragma once
#include "CV_part.h"

using namespace std;
using namespace cv;

cv::Rect before_rect;
cv::Point before_point;
cv::Mat kernel = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(-1, -1));
Mat frame_hsv, frame_hsv_mask, ROI_image, ROI_image_hsv, ROI_image_mask, ROI_hist/*直方图*/, backProjection/*反投影*/;
Mat ROI_hist_image = Mat::zeros(300, 300, CV_8UC3);//ROI区域的直方图掩码图
int deliver_x, deliver_y;
const int channels[] = { 0 };
const int bins = 8;
int bin_width = ROI_hist_image.cols / bins;
float hrange[] = { 0,180 };
const float* ranges[] = { hrange };
DWORD Croit1;
bool countTime = false;
bool PreWork = false;


void seek_range(cv::Mat frame, cv::Mat& dst_dst);
void seek_myROI(cv::Mat Image, cv::Rect& ROI);
void Control_position(cv::Mat frame, cv::Rect rect);
void doublCase(Mat& frame, Mat& dst_dst, Rect& ROI_rect);

void CVconnect(cv::Mat& frame, cv::Mat& dst_dst) {
	cv::Rect ROI;

	flip(frame, frame, 1);//视频镜像翻转

	doublCase(frame, dst_dst, ROI);

	//找出红色（指定的范围）
	//seek_range(frame, dst_dst);
	//框住（二值化后的）白色区域
	//seek_myROI(dst_dst, ROI);
	//rectangle(frame, ROI.tl(), ROI.br(), cv::Scalar(255, 0, 0), 2, 8, 0);

	Control_position(frame, ROI);


}

void seek_range(cv::Mat frame, cv::Mat& dst_dst) {
	//红色通道并二值化，找到要框住的范围
	cv::Mat dst, dst_01, dst_02, dst_03, dst_04, dst_05, dst_06;

	//blur(frame, dst, Size(5, 5));
	inRange(frame, cv::Scalar(50, 255, 213), cv::Scalar(70, 255, 255), dst);
	inRange(frame, cv::Scalar(0, 0, 213), cv::Scalar(255, 128, 255), dst_03);
	inRange(frame, cv::Scalar(15, 210, 213), cv::Scalar(110, 255, 255), dst_04);
	inRange(frame, cv::Scalar(100, 170, 255), cv::Scalar(160, 190, 255), dst_05);
	inRange(frame, cv::Scalar(50, 50, 142), cv::Scalar(75, 65, 169), dst_06);
	//彩色图像分割（输入图像，下限，上限，输出图像）

	//叠加起来
	addWeighted(dst_04, 0.5, dst_03, 0.5, 0, dst_03);
	addWeighted(dst, 0.3, dst_03, 0.7, 0, dst_03);
	addWeighted(dst_05, 0.3, dst_03, 0.7, 0, dst_03);
	addWeighted(dst_06, 0.3, dst_03, 0.7, 0, dst_03);

	//二值化
	threshold(dst_03, dst_dst, 1, 255, CV_THRESH_BINARY);

}

void seek_myROI(cv::Mat Image, cv::Rect& ROI) {
	//红色通道，并二值化，找到并框住
	//Mat Image = Image_01->clone();
	cv::Mat threshold_output;
	cv::Mat gray;

	vector<vector<cv::Point>>contours;
	vector<cv::Vec4i>hierarchy;
	

	//寻找图像轮廓
	blur(Image, Image, cv::Size(7, 7));

	findContours(Image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//使用多边形逼近检测到的图像轮廓来寻找包围轮廓的矩形
	if (contours.empty())
		return;


	vector<vector<cv::Point>>contours_poly(contours.size());
	vector<cv::Rect>boundRect(contours.size());


	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
		//minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
	}

	//绘制检测到的图像轮廓、矩形和最小包围圆
	cv::Mat drawing = cv::Mat::zeros(Image.size(), CV_8UC3);

	//vector<rect> boundRect;
	int maxarea = 0, area = 0;
	for (int i = 0; i < contours.size(); i++) {
		if (area < boundRect[i].area()) {
			maxarea = i;
			area = boundRect[i].area();
		}
	}


	ROI = boundRect[maxarea];

}

//叠加两种方法
void doublCase(Mat& frame, Mat& dst_dst, Rect& ROI_rect) {

	seek_range(frame, dst_dst);
	seek_myROI(dst_dst, ROI_rect);

	cvtColor(frame, frame_hsv, COLOR_BGR2HSV);
	inRange(frame_hsv, Scalar(0, 42, 45), Scalar(180, 256, 255), frame_hsv_mask);//分割颜色

	if (!ROI_rect.empty()) {
		if (PreWork) {
			//后续处理
		
		// 将直方图进行反向投影
			calcBackProject(&frame_hsv, 1, channels, ROI_hist, backProjection, ranges);//反向投影
			// 取公共部分
			backProjection &= frame_hsv_mask;//inrange与反向投影相结合；

			//旋转矩形的类
			RotatedRect tracking_rect = CamShift(backProjection, ROI_rect, TermCriteria((TermCriteria::COUNT | TermCriteria::EPS), 10, 1));
			//再接画外接矩形，确定ROI
			//两个加起来
			ROI_rect |= tracking_rect.boundingRect();
			rectangle(frame, ROI_rect.tl(), ROI_rect.br(), cv::Scalar(255, 0, 0), 2, 8, 0);

			

		}
		if (!PreWork && !countTime) {//配合时间1秒使用
			Croit1 = GetTickCount();
			countTime = true;
		}
		if (!PreWork && GetTickCount() - Croit1 > 1 * 1000) {
			//两秒钟后的处理，再进行后续处理
			//后续的准备工作，针对找到的rect，只执行一次
			//找到rect部分
			ROI_image = Mat(frame, ROI_rect);//在frame上画出ROI储存在ROI_image上
			ROI_image_hsv = frame_hsv(ROI_rect);//ROI的HSV图
			ROI_image_mask = frame_hsv_mask(ROI_rect);//ROI区域掩模的二值图，掩码是8位（CV_8U）的数组
			//计算直方图，得到ROI_hist
			calcHist(&ROI_image_hsv, 1, channels, ROI_image_mask, ROI_hist, 1, &bins, ranges);
			normalize(ROI_hist, ROI_hist, 0, 300, NORM_MINMAX);//归一化处理
			//cout << "准备工作   ok" << endl;
			countTime = false;
			PreWork = true;
		}


	}
	else if (ROI_rect.empty())
	{
		countTime = false;
		PreWork = false;
	}

}

//对接游戏
void Control_position(cv::Mat frame, cv::Rect rect) {
	//xy的控制

	if (rect.empty()) {
		before_rect = rect;
		//DontMove();
		return;
	}
	cv::Point now_point = getCenterPoint(rect);

	if (before_rect.empty())//第一帧的初始化，记录长度（加上计时器）
	{
		before_rect = rect;
		before_point = now_point;
		
		return;
	}


	cv::Rect judge_rect(before_point.x - 2, before_point.y - 2, 6, 6);//以中心点创建的矩形4*4
	if (!judge_rect.contains(now_point)) {//如果后面的点不在矩内，才移动
		//找距离
		int distance = getDistance(before_point, now_point);


		double angle = funGetAngle(before_point, now_point);//找角度
		before_point = now_point;//刷新before的记录
		if ((angle >= 0 && angle < 45) || (angle >= 315 && angle <= 365)) {//右x+
			//control_point.x += distance;
			deliver_x = +distance;
			return;
		}
		else if (angle >= 45 && angle < 135) {//上y-
			//control_point.y += distance;
			deliver_y = -distance;
			return;
		}
		else if (angle >= 135 && angle < 225) {//左x-
			//control_point.x -= distance;
			deliver_x = -distance;
			return;
		}
		else if (angle >= 225 && angle < 315) {//下y+
			//control_point.y -= distance;
			deliver_y = +distance;
			return;
		}

	}



}

//快捷方法打包起来；
cv::Point getCenterPoint(cv::Rect rect)
{
	cv::Point cpt;
	cpt.x = rect.x + cvRound(rect.width / 2.0);
	cpt.y = rect.y + cvRound(rect.height / 2.0);
	return cpt;
}

double funGetAngle(cv::Point pointO, cv::Point pointA) {
	if (pointA.x == pointO.x && pointA.y > pointO.y) {
		return 90;
	}
	else if (pointA.x == pointO.x && pointA.y <= pointO.y) {
		return 270;
	}

	double angle;

	angle = (int)(pointO.y - pointA.y) / (pointA.x - pointO.x);

	angle = atan(angle);
	angle *= 57.30/*(180 / CV_PI)*/;
	if (pointA.x >= pointO.x && pointA.y >= pointO.y) {
		return angle;
	}
	else if (pointA.y <= pointO.y) {
		return 180 + angle;
	}
	else if (pointA.x >= pointO.x && pointA.y <= pointO.y) {
		return 360 + angle;
	}

}

int getDistance(cv::Point pointO, cv::Point pointA)
{
	float distance;
	distance = powf((pointO.x - pointA.x), 2) + powf((pointO.y - pointA.y), 2);
	distance = sqrtf(distance);

	int distance_01 = (int)distance % 50;

	return distance_01;
}

