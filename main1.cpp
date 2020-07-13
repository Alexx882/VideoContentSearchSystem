#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>

#include "FrameComparer.cpp"
#include "VideoShotDescriptor.cpp"
#include "ContentDescriptorsUtilities.h"

using namespace std;
namespace fs = std::filesystem;
using namespace cv;

const char* FOLDER_NAME = "keyframes";

void storeKeyFrameForVideoShot(VideoCapture cap, int shotNr, int frameNr)
{	
	Mat frame;
	cap.set(CAP_PROP_POS_FRAMES, frameNr);
	cap.read(frame);
	
	string path = format("%s\\shot%d_frame%d.png", FOLDER_NAME, shotNr, frameNr);
	cv::imwrite(path, frame);
}

int mainShotDetection(int argc, const char** argv)
{
	if (argc < 2)
	{
		cout << "Please specify video.";
		exit(0);
	}

	if (argc == 3)
		FOLDER_NAME = argv[2];
	
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

	fs::remove_all(FOLDER_NAME);
	fs::create_directories(FOLDER_NAME); // use mkdir(folderName) for linux
	vector<int> keyframes = shotDescr.getKeyFrameNumbers();
	for (int i = 0; i < keyframes.size(); ++i)
		storeKeyFrameForVideoShot(cap, i, keyframes[i]);

	return 0;
}
