#pragma once

#ifndef CV_part
#define CV_part
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc.hpp>
#include <windows.h>
void seek_range(cv::Mat frame, cv::Mat& dst_dst);
void seek_myROI(cv::Mat Image, cv::Rect& ROI);
void Control_position(cv::Mat frame, cv::Rect rect);
void CVconnect(cv::Mat& frame, cv::Mat& dst_dst);

cv::Point getCenterPoint(cv::Rect rect);
double funGetAngle(cv::Point pointO, cv::Point pointA);
int getDistance(cv::Point pointO, cv::Point pointA);

//extern cv::Point control_point;
extern int deliver_x, deliver_y;

#endif // !CV_part
