#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Returns a 64-bin color histogram with mask 00bbggrr
vector<int> getHistogramFromFrame(Mat input)
{
	const int histSize = 64;
	const int channelPixels = input.rows * input.cols * input.channels();

	vector<int> hist(histSize);
	for (int i = 0; i < histSize; ++i)
		hist[i] = 0;

	uchar b, g, r;
	uchar* p = input.ptr<uchar>(0);
	for (int j = 0; j < channelPixels;)
	{
		b = (p[j++] & 0xC0) >> 2;
		g = (p[j++] & 0xC0) >> 4;
		r = (p[j++] & 0xC0) >> 6;

		hist[b | g | r]++;
	}

	return hist;
}