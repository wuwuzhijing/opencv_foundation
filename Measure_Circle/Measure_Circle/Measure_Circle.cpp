#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include "cv.h"

#include "opencv/cvwimage.h"
using namespace cv;
using std::cout;

RNG rng(12345);

int threshold_value = 70;
int threshold_type = 8;
int const max_value = 255;
int const max_type = 16;
int const max_binary_value = 255;
Mat src, src_gray, dst;
const char* window_name = "Threshold Demo";
const char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
const char* trackbar_value = "Value";
std::vector<Vec4i> hierarchy;

static void Threshold_Demo(int, void*)
{
	/* 0: Binary
	 1: Binary Inverted
	 2: Threshold Truncated
	 3: Threshold to Zero
	 4: Threshold to Zero Inverted
	*/
	threshold(src_gray, dst, threshold_value, max_binary_value, threshold_type);
	GaussianBlur(dst, dst, Size(3, 3), 0, 0, 4);
	//std::vector<std::vector<Point>> contours;
	//findContours(dst, contours, hierarchy, RETR_TREE, CHAIN_APPROX_TC89_L1);
	std::vector<Mat> contours;
	Canny(dst, contours, 20, 40, 0.5, false);
	//std::vector<RotatedRect> minEllipse(contours.size());
	//for (size_t i = 0 ; i < contours.size(); i++)
	//{
	//	minEllipse[i] = fitEllipse(Mat(contours[i]));
	//	cout << "center =" << "(" << minEllipse[i].center.x << "," << minEllipse[i].center.y << ")";
	//	cout << "size =" << "[" << minEllipse[i].size.width << "," << minEllipse[i].size.height << "]";
	//	cout << "angle = " << minEllipse[i].angle;
	//}
	std::vector<RotatedRect> minRect(contours.size());
	std::vector<RotatedRect> minEllipse(contours.size());
	
	//for (int i = 0; i < contours.size(); i++)
	for (int i = 0; i < contours.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contours[i]));
		if (contours[i].rows > 5)
		{
			minEllipse[i] = fitEllipse(Mat(contours[i]));
		}
	}

	/// Draw contours + rotated rects + ellipses
	Mat threshold_output;
	Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC1);
	for (int i = 0; i < contours.size(); i++)
	{

		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		// contour
		drawContours(drawing, contours, i, color, 1, 8, std::vector<Vec4i>(), 0, Point());
		// ellipse
		ellipse(drawing, minEllipse[i], color, 2, 8);
		// rotated rectangle
		Point2f rect_points[4]; minRect[i].points(rect_points);
		for (int j = 0; j < 4; j++)
			line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
	}

	imshow(window_name, drawing);
}

int main(int argc, char** argv)
{
	String imageName("D:/test.tiff"); // by default
	if (argc > 1)
	{
		imageName = argv[1];
	}

	src = imread(imageName, IMREAD_COLOR); // Load an image
	if (src.empty())
	{
		cout << "Cannot read image: " << imageName << std::endl;
		return -1;
	}
	cvtColor(src, src_gray, COLOR_BGR2GRAY); // Convert the image to Gray
	namedWindow(window_name, WINDOW_AUTOSIZE); // Create a window to display results
	createTrackbar(trackbar_type,
		window_name, &threshold_type,
		max_type, Threshold_Demo); // Create Trackbar to choose type of Threshold
	createTrackbar(trackbar_value,
		window_name, &threshold_value,
		max_value, Threshold_Demo); // Create Trackbar to choose Threshold value
	Threshold_Demo(0, 0); // Call the function to initialize
	waitKey();
	return 0;
}