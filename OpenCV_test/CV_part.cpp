#pragma once
#include "CV_part.h"

using namespace std;
using namespace cv;

cv::Rect before_rect;
cv::Point before_point;
cv::Mat kernel = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(-1, -1));
Mat frame_hsv, frame_hsv_mask, ROI_image, ROI_image_hsv, ROI_image_mask, ROI_hist/*ֱ��ͼ*/, backProjection/*��ͶӰ*/;
Mat ROI_hist_image = Mat::zeros(300, 300, CV_8UC3);//ROI�����ֱ��ͼ����ͼ
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

	flip(frame, frame, 1);//��Ƶ����ת

	doublCase(frame, dst_dst, ROI);

	//�ҳ���ɫ��ָ���ķ�Χ��
	//seek_range(frame, dst_dst);
	//��ס����ֵ����ģ���ɫ����
	//seek_myROI(dst_dst, ROI);
	//rectangle(frame, ROI.tl(), ROI.br(), cv::Scalar(255, 0, 0), 2, 8, 0);

	Control_position(frame, ROI);


}

void seek_range(cv::Mat frame, cv::Mat& dst_dst) {
	//��ɫͨ������ֵ�����ҵ�Ҫ��ס�ķ�Χ
	cv::Mat dst, dst_01, dst_02, dst_03, dst_04, dst_05, dst_06;

	//blur(frame, dst, Size(5, 5));
	inRange(frame, cv::Scalar(50, 255, 213), cv::Scalar(70, 255, 255), dst);
	inRange(frame, cv::Scalar(0, 0, 213), cv::Scalar(255, 128, 255), dst_03);
	inRange(frame, cv::Scalar(15, 210, 213), cv::Scalar(110, 255, 255), dst_04);
	inRange(frame, cv::Scalar(100, 170, 255), cv::Scalar(160, 190, 255), dst_05);
	inRange(frame, cv::Scalar(50, 50, 142), cv::Scalar(75, 65, 169), dst_06);
	//��ɫͼ��ָ����ͼ�����ޣ����ޣ����ͼ��

	//��������
	addWeighted(dst_04, 0.5, dst_03, 0.5, 0, dst_03);
	addWeighted(dst, 0.3, dst_03, 0.7, 0, dst_03);
	addWeighted(dst_05, 0.3, dst_03, 0.7, 0, dst_03);
	addWeighted(dst_06, 0.3, dst_03, 0.7, 0, dst_03);

	//��ֵ��
	threshold(dst_03, dst_dst, 1, 255, CV_THRESH_BINARY);

}

void seek_myROI(cv::Mat Image, cv::Rect& ROI) {
	//��ɫͨ��������ֵ�����ҵ�����ס
	//Mat Image = Image_01->clone();
	cv::Mat threshold_output;
	cv::Mat gray;

	vector<vector<cv::Point>>contours;
	vector<cv::Vec4i>hierarchy;
	

	//Ѱ��ͼ������
	blur(Image, Image, cv::Size(7, 7));

	findContours(Image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//ʹ�ö���αƽ���⵽��ͼ��������Ѱ�Ұ�Χ�����ľ���
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

	//���Ƽ�⵽��ͼ�����������κ���С��ΧԲ
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

//�������ַ���
void doublCase(Mat& frame, Mat& dst_dst, Rect& ROI_rect) {

	seek_range(frame, dst_dst);
	seek_myROI(dst_dst, ROI_rect);

	cvtColor(frame, frame_hsv, COLOR_BGR2HSV);
	inRange(frame_hsv, Scalar(0, 42, 45), Scalar(180, 256, 255), frame_hsv_mask);//�ָ���ɫ

	if (!ROI_rect.empty()) {
		if (PreWork) {
			//��������
		
		// ��ֱ��ͼ���з���ͶӰ
			calcBackProject(&frame_hsv, 1, channels, ROI_hist, backProjection, ranges);//����ͶӰ
			// ȡ��������
			backProjection &= frame_hsv_mask;//inrange�뷴��ͶӰ���ϣ�

			//��ת���ε���
			RotatedRect tracking_rect = CamShift(backProjection, ROI_rect, TermCriteria((TermCriteria::COUNT | TermCriteria::EPS), 10, 1));
			//�ٽӻ���Ӿ��Σ�ȷ��ROI
			//����������
			ROI_rect |= tracking_rect.boundingRect();
			rectangle(frame, ROI_rect.tl(), ROI_rect.br(), cv::Scalar(255, 0, 0), 2, 8, 0);

			

		}
		if (!PreWork && !countTime) {//���ʱ��1��ʹ��
			Croit1 = GetTickCount();
			countTime = true;
		}
		if (!PreWork && GetTickCount() - Croit1 > 1 * 1000) {
			//�����Ӻ�Ĵ����ٽ��к�������
			//������׼������������ҵ���rect��ִֻ��һ��
			//�ҵ�rect����
			ROI_image = Mat(frame, ROI_rect);//��frame�ϻ���ROI������ROI_image��
			ROI_image_hsv = frame_hsv(ROI_rect);//ROI��HSVͼ
			ROI_image_mask = frame_hsv_mask(ROI_rect);//ROI������ģ�Ķ�ֵͼ��������8λ��CV_8U��������
			//����ֱ��ͼ���õ�ROI_hist
			calcHist(&ROI_image_hsv, 1, channels, ROI_image_mask, ROI_hist, 1, &bins, ranges);
			normalize(ROI_hist, ROI_hist, 0, 300, NORM_MINMAX);//��һ������
			//cout << "׼������   ok" << endl;
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

//�Խ���Ϸ
void Control_position(cv::Mat frame, cv::Rect rect) {
	//xy�Ŀ���

	if (rect.empty()) {
		before_rect = rect;
		//DontMove();
		return;
	}
	cv::Point now_point = getCenterPoint(rect);

	if (before_rect.empty())//��һ֡�ĳ�ʼ������¼���ȣ����ϼ�ʱ����
	{
		before_rect = rect;
		before_point = now_point;
		
		return;
	}


	cv::Rect judge_rect(before_point.x - 2, before_point.y - 2, 6, 6);//�����ĵ㴴���ľ���4*4
	if (!judge_rect.contains(now_point)) {//�������ĵ㲻�ھ��ڣ����ƶ�
		//�Ҿ���
		int distance = getDistance(before_point, now_point);


		double angle = funGetAngle(before_point, now_point);//�ҽǶ�
		before_point = now_point;//ˢ��before�ļ�¼
		if ((angle >= 0 && angle < 45) || (angle >= 315 && angle <= 365)) {//��x+
			//control_point.x += distance;
			deliver_x = +distance;
			return;
		}
		else if (angle >= 45 && angle < 135) {//��y-
			//control_point.y += distance;
			deliver_y = -distance;
			return;
		}
		else if (angle >= 135 && angle < 225) {//��x-
			//control_point.x -= distance;
			deliver_x = -distance;
			return;
		}
		else if (angle >= 225 && angle < 315) {//��y+
			//control_point.y -= distance;
			deliver_y = +distance;
			return;
		}

	}



}

//��ݷ������������
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

