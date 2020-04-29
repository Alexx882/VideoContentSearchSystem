#pragma once

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Returns a 64-bin color histogram with mask 00bbggrr
vector<int> getHistogramFromFrame(Mat input);