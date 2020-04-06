#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>

#include "FrameComparer.cpp"
#include "VideoShotDescriptor.cpp"

using namespace std;
namespace fs = std::filesystem;
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

void storeKeyFrameForVideoShot(VideoCapture cap, int shotNr, int frameNr)
{	
	Mat frame;
	cap.set(CAP_PROP_POS_FRAMES, frameNr);
	cap.read(frame);
	
	string path = format("keyframes\\shot%d_frame%d.png", shotNr, frameNr);
	cv::imwrite(path, frame);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cout << "Please specify video.";
		exit(0);
	}

	VideoCapture cap = VideoCapture(argv[1]);
	FrameComparer comparer = FrameComparer(64);
	VideoShotDescriptor shotDescr = VideoShotDescriptor();

	Mat frame;
	vector<int> curHist = {}, prevHist = {};
	
	int waitTime = 1000 / cap.get(CAP_PROP_FPS);

	int count = 0;
	while (cap.read(frame))
	{
		cout << "Frame " << count << endl;

		prevHist = curHist;
		curHist = getHistogramFromFrame(frame);

		if (count > 1 && comparer.isNextTwinComparisonExceeded(prevHist, curHist))
			shotDescr.addBoundary(count);

		//cv::imshow("Video", frame);
		//waitKey(waitTime);

		++count;
	}
	shotDescr.addLastBoundary(count);

	shotDescr.printShots();

	fs::remove_all("keyframes");
	fs::create_directory("keyframes"); // use mkdir(folderName) for linux
	vector<int> keyframes = shotDescr.getKeyFrameNumbers();
	for (int i = 0; i < keyframes.size(); ++i)
		storeKeyFrameForVideoShot(cap, i, keyframes[i]);

	return 0;
}
