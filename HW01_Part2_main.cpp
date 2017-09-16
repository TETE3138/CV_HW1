#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

int main()
{
	Mat image;

	image = imread("C:/Users/TETE/Desktop/OpenCVFiles/mm5.jpg", IMREAD_COLOR);
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	Mat hls;
	cvtColor(image, hls, COLOR_BGR2HLS);


	Mat luminance(image.rows, image.cols, CV_8U);
	for (size_t y = 0; y < image.rows; y++)
	{
		for (size_t x = 0; x < image.cols; x++)
		{
			luminance.at<uchar>(y, x) = hls.at<Vec3b>(y, x)[1];
		}
	}
	namedWindow("Image", 1);
	imshow("Image", luminance);

	Mat binary;
	threshold(luminance, binary, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

	namedWindow("BINARY_INV", 1);
	imshow("BINARY_INV", binary);

	Mat opened_image;
	Mat SE(4, 4, CV_8U, Scalar(1));
	morphologyEx(binary, opened_image, MORPH_CLOSE, SE);

	Mat SE2(2, 2, CV_8U, Scalar(1));
	morphologyEx(binary, opened_image, MORPH_OPEN, SE);

	namedWindow("closed_image", 1);
	imshow("closed_image", opened_image);

	Mat contoursImg = image.clone();
	vector<vector<Point>> contours;
	findContours(opened_image, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	drawContours(contoursImg, contours, -1, Scalar(0, 255, 0), 1);

	namedWindow("contoursImg", 1);
	imshow("contoursImg", contoursImg);


	/*
	vector<double> len(contours.size());
	vector<double> areas(contours.size());
	vector<double> circularity(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		len[i] = arcLength(contours[i], true);
		areas[i] = contourArea(contours[i]);
		circularity[i] = (4.0*CV_PI *areas[i]) / (len[i] * len[i]);
		putText(contoursImg, to_string(areas[i]), contours[i][0], 2, 0.4, Scalar(0, 255, 0));

	}
	*/


	Mat result = image.clone();
	int orange = 0;
	int red = 0;
	int blue = 0;
	int yellow = 0;
	int brown = 0;
	int green = 0;
	vector<double> avg_luminance(contours.size());
	vector<double> avg_hue(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		Rect bb = boundingRect(contours[i]);
		int sx = bb.tl().x; int sy = bb.tl().y;
		int ex = bb.br().x - 1; int ey = bb.br().y - 1;
		int cx = (sx + ex) / 2; int cy = (sy + ey) / 2;

		Vec3i avg_bgr(0, 0, 0);
		int h = 0;
		int l = 0;
		for (size_t y = 0; y < bb.height; y++)
		{
			for (size_t x = 0; x < bb.width; x++)
			{
				double inside = pointPolygonTest(contours[i], Point2f(float(x + sx), float(y + sy)), false);
				if (inside >= 0) {
					//inside contour
			/*		avg_bgr[0] += hls.at<Vec3b>(y + sy, x + sx)[0];
					avg_bgr[1] += image.at<Vec3b>(y + sy, x + sx)[1];
					avg_bgr[2] += image.at<Vec3b>(y + sy, x + sx)[2];*/
					//result.at<Vec3b>(y + sy, x + sx) = Vec3b(255, 255, 0);
					h += hls.at<Vec3b>(y + sy, x + sx)[0];
					l += hls.at<Vec3b>(y + sy, x + sx)[1];
				}

			}
		}

		/*avg_bgr[0] = avg_bgr[0] / (bb.height*bb.width);
		avg_bgr[1] = avg_bgr[1] / (bb.height*bb.width);
		avg_bgr[2] = avg_bgr[2] / (bb.height*bb.width);*/
		avg_luminance[i] = l / (bb.height*bb.width);
		avg_hue[i] = h / (bb.height*bb.width);

		if (10 <= avg_hue[i] && avg_hue[i] <= 12 && avg_luminance[i] >=65) {
			orange++;
			putText(result, "Orange", Point(sx, cy), 1, 1, Scalar(0, 0, 0),2);
		}
		else if (0 <= avg_hue[i] && avg_hue[i] <= 9) {
			red++;
			putText(result, "Red", Point(sx, cy), 1, 1, Scalar(0, 0, 0), 2);
		}
		else if (78 <= avg_hue[i] && avg_hue[i] <= 83) {
			blue++;
			putText(result, "Blue", Point(sx, cy), 1, 1, Scalar(0, 0, 0), 2);
		}
		else if (12 <= avg_hue[i] && avg_hue[i] <= 17 && avg_luminance[i] <65) {
			brown++;
			putText(result, "Brown", Point(sx, cy), 1, 1, Scalar(0, 0, 0), 2);
		}
		else if (20 <= avg_hue[i] && avg_hue[i] <= 23) {
			yellow++;
			putText(result, "Yellow", Point(sx, cy), 1, 1, Scalar(0, 0, 0), 2);
		}
		else if (38 <= avg_hue[i] && avg_hue[i] <= 43) {
			green++;
			putText(result, "Green", Point(sx, cy), 1, 1, Scalar(0, 0, 0), 2);
		}

		
		//putText(result, to_string(avg_hue[i]), contours[i][0], 1, 1, Scalar(255, 0, 0));

		putText(result, to_string(i), Point(cx,cy), 1, 1, Scalar(255, 0, 0));
		printf("%d\t%.2f\t%.2f\n", i, avg_hue[i],avg_luminance[i]);
		rectangle(result, bb, Scalar(0, 0, 255), 1);
		drawMarker(result, Point(cx, cy), Scalar(0, 255, 0));


		
	}
	printf("Orange: %d\n", orange);
	printf("Red: %d\n", red);
	printf("Blue: %d\n", blue);
	printf("Yellow: %d\n", yellow);
	printf("Green %d\n", green);
	printf("Brown: %d\n", brown);

	int i = 0;
	putText(result, "Orange: " + to_string(orange), Point(0, (i++ + 1) * 15), 1, 1, Scalar(0, 127, 255));
	putText(result, "Red: " + to_string(red), Point(0, (i++ + 1) * 15), 1, 1, Scalar(0, 0, 255));
	putText(result, "Blue: " + to_string(blue), Point(0, (i++ + 1) * 15), 1, 1, Scalar(255, 0, 0));
	putText(result, "Yellow: " + to_string(yellow), Point(0, (i++ + 1) * 15), 1, 1, Scalar(0, 200, 200));
	putText(result, "Green: " + to_string(green), Point(0, (i++ + 1) * 15), 1, 1, Scalar(0, 255, 0));
	putText(result, "Brown: " + to_string(brown), Point(0, (i++ + 1) * 15), 1, 1, Scalar(0, 0, 128));



	namedWindow("result", 1);
	imshow("result", result);

	waitKey(0);
	destroyAllWindows();
	return 0;
}

