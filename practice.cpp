#include<opencv2/opencv.hpp>
#include <opencv\cv.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>

#pragma comment(lib, "Ws2_32.lib")
#define default_buflen 1024
#define default_port "1234"

using namespace std;
using namespace cv;

Mat Filter(Mat& frame, Mat& edge, Mat& filter, Mat& gray_image) {

	cvtColor(frame, gray_image, COLOR_BGR2GRAY);
	GaussianBlur(gray_image, filter, Size(7, 7), 1.5);
	Canny(filter, edge, 50, 150, 3);

	return edge;
}

void Error1(VideoCapture& cap, VideoWriter& oVideoWriter) {

	if (!cap.isOpened())
	{
		cout << "Cannot connect to camera" << endl;
		getchar();
		return exit(1);
	}

	if (!oVideoWriter.isOpened())
	{
		cout << "ERROR: Failed to write the video" << endl;
		return exit(1);
	}
}

void Error2(VideoCapture& cap, Mat& frame) {

	bool bSuccess = cap.read(frame);

	if (!bSuccess)
	{
		cout << "ERROR: Cannot read a frame from video file" << endl;
		return exit(1);
	}
}

class Line {

	int MRecX[100] = { 0 };
	int MRecY[100] = { 0 };
	int CLx[100] = { 0 };
	int CLy[100] = { 0 };
	float Distance[100] = { 0 };
	int rectcentrey, rectcentrex, k;

public:
	Line(int rectcentreY, int rectcentreX, int K) {

		rectcentrey = rectcentreY;
		rectcentrex = rectcentreX;
		k = K;

	}

	int KI(int i) {
		k = i;

		return k;
	}

	int RectCentre(Rect& minRect) {

		rectcentrex = minRect.x + minRect.width / 2;
		rectcentrey = minRect.y + minRect.height / 2;

		return rectcentrey;
		return rectcentrex;
	}

	float Dist() {

		if (MRecX[k] != rectcentrex & MRecY[k] != rectcentrey)
			Distance[k] = norm(Point(MRecX[k], MRecY[k]) - Point(rectcentrex, rectcentrey));

		return Distance[k];
	}

	int CentreLine() {
		if (MRecX[k] != rectcentrex & MRecY[k] != rectcentrey) {
			CLx[k] = abs((rectcentrex - MRecX[k]) / 2);
			if (rectcentrex > MRecX[k])
				CLx[k] = MRecX[k] + CLx[k];
			else
				CLx[k] = rectcentrex + CLx[k];
			CLy[k] = abs((rectcentrey - MRecY[k]) / 2);
			if (rectcentrey > MRecY[k])
				CLy[k] = MRecY[k] + CLy[k];
			else
				CLy[k] = rectcentrey + CLy[k];
		}

		return CLy[k];
		return CLx[k];

	}

	int Equality() {


		if (rectcentrex != MRecX[k] & rectcentrey != MRecY[k]) {
			MRecX[k] = rectcentrex;
			MRecY[k] = rectcentrey;
		}

		return MRecX[k];
		return MRecY[k];
	}

	int GetrectcentreX() {
		return rectcentrex;
	}

	int GetrectcentreY() {
		return rectcentrey;
	}

	int* GetMRecX() {
		return MRecX;
	}

	int GetMRecXX(int i) {

		return MRecX[i];
	}

	int* GetMRecY() {
		return MRecY;
	}

	int GetMRecYY(int i) {

		return MRecY[i];
	}

	int* GetCLX() {
		return CLx;
	}

	int* GetCLY() {
		return CLy;
	}

	float* Getdistance() {
		return Distance;
	}

	float Getdistance1(int i) {
		return Distance[i];
	}

	int Getk() {

		return k;
	}

	void Null() {

		memset(MRecX, 0, 100);
		memset(MRecY, 0, 100);
		memset(CLx, 0, 100);
		memset(CLy, 0, 100);
		memset(Distance, 0, 100);
	}
};

class Area {
	int SP, dWidth, dHeight;
	double Rat, XM, YM, NM2;
	float NM[100] = { 0 };
public:
	Area(double Xm, double Ym, double rat, int sp, int DWidth, int DHeight, double Nm2) {

		XM = Xm;
		YM = Ym;
		Rat = rat;
		SP = sp;
		dWidth = DWidth;
		dHeight = DHeight;
		NM2 = Nm2;
	}

	void SetArea() {

		cout << "Enter the size of the workspace (form: X_Y (Centimeters)): " << endl;
		cin >> XM >> YM;
	}

	Size GetSize(VideoCapture& cap) {

		dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

		Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

		return frameSize;
	}

	void Ratio() {

		Rat = (XM * YM) / (dWidth * dHeight);
	}

	float* TransformM(float* N, int k) {

		NM[k] = Rat * N[k];
		return NM;
	}

	float* GetNM() {

		return NM;
	}

	float Transform(float N) {

		NM2 = Rat * N;
		return NM2;
	}
};

Mat Rendering(Mat& frame, vector<vector<Point> > contours, int rectcentreX, int rectcentreY, int* MRecX, int* MRecY,
	float* distance, int* CLX, int* CLY, Rect& minRect, float Rwidth, float Rheight, int k) {

	rectangle(frame, minRect, Scalar(0, 0, 255));
	circle(frame, Point(rectcentreX, rectcentreY), 1, (0, 0, 255), 3);
	if (MRecX[k] != 0 & MRecY[k] != 0) {
		line(frame, Point(rectcentreX, rectcentreY), Point(MRecX[k], MRecY[k]), Scalar(255, 0, 0));
		putText(frame, format("D = %.2f Cm", (distance[k])), Point(CLX[k], CLY[k]),
			FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
		cout << "Distance" << k << " " << distance[k] << endl;
	}

	/*if (distance < 50)
		cout << "Warning! Next to the object!" << endl;*/

	putText(frame, format("W = %.2f Cm , H = %.2f Cm", Rwidth, Rheight), Point(minRect.x, minRect.y),
		FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0));

	return frame;
}

int main(int argc, char** argv)
{
	/*VideoCapture cap(0);*/
	VideoCapture cap("Robot_without_shadows (1).mp4");

	vector<vector<Point> > contours;

	Mat gray_image;
	Mat edge;
	Mat filter;
	Mat frame;

	Line line1(0, 0, 0);
	Area area(0, 0, 0, 0, 0, 0, 0);

	area.SetArea();

	VideoWriter oVideoWriter("robot.avi", CV_FOURCC('F', 'L', 'V', '1'), 1, area.GetSize(cap));

	area.Ratio();

	Error1(cap, oVideoWriter);

	while (true)
	{

		Error2(cap, frame);

		findContours(Filter(frame, edge, filter, gray_image), contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

		for (size_t i = 0; i < contours.size(); i++)
		{
			Rect minRect = boundingRect(contours[i]);
			line1.KI(i);

			if (minRect.width > 10 & minRect.height > 10 & minRect.width < 400 & minRect.height < 400)
			{
				line1.Equality();
				line1.RectCentre(minRect);
				line1.CentreLine();
				line1.Dist();
				area.TransformM(line1.Getdistance(), line1.Getk());
				Rendering(frame, contours, line1.GetrectcentreX(), line1.GetrectcentreY(), line1.GetMRecX(), line1.GetMRecY(),
					area.GetNM(), line1.GetCLX(),
					line1.GetCLY(), minRect, area.Transform(minRect.width), area.Transform(minRect.height), line1.Getk());
			}


		}
		oVideoWriter.write(frame);

		imshow("Display", frame);

		waitKey(500);

		line1.Null();

	}
}