#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
using namespace cv;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cout << "Please specify video.";
		exit(0);
	}

	cout << "Current path is " << fs::current_path() << endl;
	cout << "file to read: " << argv[1] << endl;

	VideoCapture cap = VideoCapture(argv[1]);
	cout << "Video " << argv[1] << " has " << cap.get(CAP_PROP_FPS) << " fps" << endl;

	Mat frame;
	int waitTime = 1000 / cap.get(CAP_PROP_FPS);

	int count = 0;
	while (cap.read(frame))
	{
		cout << "Frame " << ++count << endl;
		imshow("Video", frame);
		int key = waitKey(waitTime); //0
		if (key == 'q') break;
	}

	return 0;
}
