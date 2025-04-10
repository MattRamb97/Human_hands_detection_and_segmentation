#pragma once

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>



// 
void task1();

// 
void meanShift();

void hsv_space();
void hsv_abgr();
void hsv();
cv::Mat showHistogram(cv::Mat src);


//void onMouse(int event, int x, int y, int flags, void* userdata);		// Not really used but it is the starting scratch
//cv::Mat showHistogram(cv::Mat src);										// Usefull!!