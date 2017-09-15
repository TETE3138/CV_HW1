#include "opencv2/opencv.hpp"
#include "iostream"
using namespace cv;
using namespace std;
int main()
{
	Mat bg_im, image, bg_im_gray, image_gray, diff_im, bw;
	double th = -1;
	bg_im = imread("C:/Users/TETE/Desktop/Basketball2_frame/basketball2_frame_0001.bmp", IMREAD_COLOR);
	cvtColor(bg_im, bg_im_gray, COLOR_BGR2GRAY);
	namedWindow("BG", 1);
	namedWindow("Image", 1);
	namedWindow("Diff", 1);
	namedWindow("BW", 1);
	namedWindow("opened_image", 1);
	imshow("BG", bg_im);
	for (int i = 2; i <= 100; i++)
	{
		char filename[200];

		sprintf_s(filename, "C:/Users/TETE/Desktop/Basketball2_frame/basketball2_frame_%04d.bmp", i);
		image = imread(filename);
		cvtColor(image, image_gray, COLOR_BGR2GRAY);
		cv::absdiff(bg_im_gray, image_gray, diff_im);;
		threshold(diff_im, bw, th, 255, THRESH_BINARY | THRESH_OTSU);
		// Your Code to detect the ball
		Mat hls;
		cvtColor(image, hls, COLOR_BGR2HLS);

		Mat SE(15, 15, CV_8U, Scalar(1));
		Mat opened_image;
		morphologyEx(bw, opened_image, MORPH_DILATE, SE);

		vector<vector<Point2i>>  contours;
			
		findContours(opened_image, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		for (size_t i = 0; i < contours.size(); i++)
		{
			Rect bb = boundingRect(contours[i]);
			int sx = bb.tl().x; int sy = bb.tl().y;
			int ex = bb.br().x -1; int ey = bb.br().y-1;
			int cx = (sx + ex) / 2; int cy = (sy + ey) / 2;

			int area = bb.width * bb.height;

			putText(opened_image, to_string(area), contours[i][0], 1, 1,Scalar(255,255,255));
			if (360<area && area< 1200) {
				int avg_hue = 0;
				int ball_area = 0;
				for (size_t y = 0; y < bb.height; y++) 
				{
					for (size_t x = 0; x < bb.width; x++)
					{
						double inside = pointPolygonTest(contours[i], Point2f(float(x + sx), float(y + sy)), false);
						if (inside >= 0) {		
							ball_area++;
							avg_hue += hls.at<Vec3b>(y + sy, x + sx)[0];
							//image.at<Vec3b>(y + sy, x + sx) = Vec3b(255, 0, 0);
						}

					}
				} 
				avg_hue = avg_hue / ball_area;

				if (avg_hue > 35 && cy < 370) {
					cvtColor(opened_image, opened_image, COLOR_GRAY2BGR);
					putText(opened_image, to_string(avg_hue), contours[i][contours[i].size()/2], 1, 1, Scalar(0, 0, 255));
					putText(image, "Ball", contours[i][0], 1, 1, Scalar(0, 0, 255));
					drawMarker(opened_image, Point(cx, cy), Scalar(0, 255, 0));
					drawMarker(image, Point(cx, cy), Scalar(0, 255, 0));
					rectangle(image, bb, Scalar(0, 0, 255));
					rectangle(opened_image, bb, Scalar(0, 0, 255));
				}
				
			}
		}
		
	
		imshow("Diff", diff_im);
		imshow("BW", bw);
		imshow("openned_image", opened_image);
		imshow("Image", image);
		

		waitKey(0);
		if (i == 100)i = 2;
	}
	destroyAllWindows();
	return 0;
}