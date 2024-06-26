#include "stdafx.h"
using namespace cv;
using namespace std;

bool valid_coord(Mat& image, int x, int y) {
	if (x < 0 || y<0 || x >= image.rows || y>=image.cols) {
		return false;
	}
	return true;
}

bool same_color_rgb(Vec3b& pixel1, Vec3b& pixel2) {
	if (pixel1[0] != pixel2[0] ||
		pixel1[1] != pixel2[1] ||
		pixel1[2] != pixel2[2]) {
		return false;
	}
	return true;
}

void change_color(Vec3b& color) {
	color[0] = rand() % 254 + 1;
	color[1] = rand() % 254 + 1;
	color[2] = rand() % 254 + 1;
}

void floodfill(Mat& source, Mat& result, int x, int y,Vec3b color) {
	if (!valid_coord(source, x, y) ||
		source.at<uchar>(x, y) != 255 ||	//not thresholded
		same_color_rgb(result.at<Vec3b>(x,y),color)) {	//already indexed
		return;
	}

	result.at<Vec3b>(x, y) = color;
	floodfill(source, result, x - 1, y,color);
	floodfill(source, result, x , y + 1,color);
	floodfill(source, result, x + 1, y,color);
	floodfill(source, result, x , y - 1,color);
}

void threshold_image(Mat& source, Mat& result,uchar threshold,uchar max=255) {
	if (source.type() != CV_8UC1) {
		throw "Invalid image format";
	}

	result.create(source.rows, source.cols, CV_8UC1);
	for (int x = 0; x < source.rows; x++) {
		for (int y = 0; y < source.cols; y++) {
			if (source.at<uchar>(x, y) > threshold) {
				result.at<uchar>(x, y) = max;
			}
			else {
				result.at<uchar>(x, y) = 0;
			}
		}
	}
}

void index_image(Mat& source, Mat& result) {
	result = Mat::zeros(source.rows, source.cols, CV_8UC3);
	Vec3b curr_color,black;
	black[0] = 0;
	black[1] = 0;
	black[2] = 0;
	change_color(curr_color);

	for (int x = 0; x < source.rows; x++) {
		for (int y = 0; y < source.cols; y++) {
			if (source.at<uchar>(x, y) == 255 &&
				same_color_rgb(result.at<Vec3b>(x,y),black)) {	//pixel is thresholded in source and not indexed in result
				floodfill(source, result, x, y,curr_color);
				change_color(curr_color);
			}
		}
	}
}

int main()
{
	Mat source = imread("images/train.png", cv::IMREAD_GRAYSCALE),thresholded,indexed;
	imshow("Initial image", source);
	waitKey(0);

	threshold_image(source, thresholded, 50);
	imshow("Thresholded image", thresholded);
	waitKey(0);

	index_image(thresholded,indexed);
	imshow("Indexed image", indexed);
	waitKey(0);
	return 0;
}
