#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc.hpp>
#include "CV_part.h"
#include "game_part.h"

using namespace std;
using namespace cv;

//定义全局变量
cv::Mat frame, dst_dst;

int main()
{
	//摄像头的开启检测
	VideoCapture capture;
	capture.open(0);
	if (!capture.isOpened())
	{
		printf("can not open video file   \n");
		return -1;
	}

	//游戏
	initgraph(WIN_WIDTH, WIN_HEIGHT, SHOWCONSOLE);
	GameInit();
	BeginBatchDraw();
	//

	while (capture.read(frame))//循环读入视频帧
	{
		CVconnect(frame, dst_dst);
		//显示
		//imshow("output_dst", dst_dst);
		imshow("output", frame);

		//游戏部分
		gamecontrol();
		GameDraw();
		FlushBatchDraw();
		//

		if (waitKey(1) == 27)
			break;
		//waitKey(1);
	}
	capture.release();
	return 0;
}
