#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

int main()
{
	Mat image;

	image = imread("C:/Users/TETE/Desktop/OpenCVFiles/nut.jpg", IMREAD_GRAYSCALE);
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	namedWindow("Image", 1);
	imshow("Image", image);

	Mat binary;
	threshold(image, binary, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

	namedWindow("BINARY_INV", 1);
	imshow("BINARY_INV", binary);

	Mat opened_image;
	Mat SE(12, 12, CV_8U, Scalar(1));
	morphologyEx(binary, opened_image, MORPH_CLOSE, SE);

	namedWindow("opened_image", 1);
	imshow("opened_image", opened_image);

	vector<vector<Point>> contours;
	findContours(opened_image, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//drawContours(image, contours, -1,Scalar(0, 255, 0), 1);
	//
	//namedWindow("contours", 1);
	//imshow("contours", image);
	Mat convexHullImg = image.clone();
	vector<vector<Point>>  hull(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i], true, true);
		int hullCnt = hull[i].size();
		Point pt0 = hull[i][hullCnt - 1];
		for (size_t k = 0; k < hullCnt; k++)
		{
			Point pt1 = hull[i][k];
			line(convexHullImg, pt0, pt1, Scalar(0, 255, 0), 1, LINE_AA);
			pt0 = pt1;
		}
	}

	vector<double> len(hull.size());
	vector<double> areas(hull.size());
	vector<double> circularity(hull.size());
	for (size_t i = 0; i < hull.size(); i++)
	{
		len[i] = arcLength(hull[i], true);
		areas[i] = contourArea(hull[i]);
		circularity[i] = (4.0*CV_PI *areas[i]) / (len[i] * len[i]);
		putText(convexHullImg, to_string(circularity[i]), contours[i][0], 2, 0.4, Scalar(0, 255, 0));

	}

	namedWindow("convexHull", 1);
	imshow("convexHull", convexHullImg);

	Mat result;
	cvtColor(image, result, COLOR_GRAY2BGR);

	uint nuts = 0;
	uint bolts = 0;
	double circularityThreshold = 0.9;
	for (size_t i = 0; i < hull.size(); i++)
	{
		Rect bb = boundingRect(hull[i]);
		int sx = bb.tl().x; int sy = bb.tl().y;
		int ex = bb.br().x - 1; int ey = bb.br().y - 1;
		int cx = (sx + ex) / 2; int cy = (sy + ey) / 2;

		rectangle(result, bb, circularity[i] >= circularityThreshold ? Scalar(0, 255, 0) : Scalar(0, 0, 255), 2);
		drawMarker(result, Point(cx, cy), circularity[i] >= circularityThreshold ? Scalar(0, 255, 0) : Scalar(0, 0, 255));

		if (circularity[i] >= circularityThreshold)
			nuts++;
			
		else
			bolts++;

	}


	putText(result, "Nut = " + to_string(nuts),Point(0,40),1,3, Scalar(0, 255, 0),3);
	putText(result, "Bolt = " + to_string(bolts), Point(0, 40+40), 1, 3, Scalar(0, 0, 255), 3);
	namedWindow("circularity", 1);
	imshow("circularity", result);

	waitKey(0);
	destroyAllWindows();
	return 0;
}

