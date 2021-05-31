#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc.hpp>
#include "CV_part.h"
#include "game_part.h"

using namespace std;
using namespace cv;

//����ȫ�ֱ���
cv::Mat frame, dst_dst;

int main()
{
	//����ͷ�Ŀ������
	VideoCapture capture;
	capture.open(0);
	if (!capture.isOpened())
	{
		printf("can not open video file   \n");
		return -1;
	}

	//��Ϸ
	initgraph(WIN_WIDTH, WIN_HEIGHT, SHOWCONSOLE);
	GameInit();
	BeginBatchDraw();
	//

	while (capture.read(frame))//ѭ��������Ƶ֡
	{
		CVconnect(frame, dst_dst);
		//��ʾ
		//imshow("output_dst", dst_dst);
		imshow("output", frame);

		//��Ϸ����
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
