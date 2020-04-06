#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
using namespace cv;

// Returns a 64-bin color histogram with mask 00bbggrr
Mat getHistogramFromFrame(Mat input)
{
	const int histSize = 64;
	const int channelPixels = input.rows * input.cols * input.channels();
	
	vector<float> hist(histSize);
	for (int i = 0; i < histSize; ++i)
		hist[i] = 0;

	uchar b, g, r;
	uchar* p = input.ptr<uchar>(0);
	for (int j = 0; j < channelPixels; )
	{
		b = (p[j++] & 0xC0) >> 2;
		g = (p[j++] & 0xC0) >> 4;
		r = (p[j++] & 0xC0) >> 6;

		hist[b | g | r]++;
	}

	return Mat(hist);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cout << "Please specify video.";
		exit(0);
	}

	VideoCapture cap = VideoCapture(argv[1]);

	Mat frame;
	Mat curHist = Mat(), prevHist = Mat();
	
	int count = 0;
	while (cap.read(frame))
	{
		prevHist = curHist;
		curHist = getHistogramFromFrame(frame);
		
		cout << "Frame " << ++count << endl;
	}

	return 0;
}
