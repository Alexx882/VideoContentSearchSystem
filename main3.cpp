#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <filesystem>
#include <random>


#include "LabeledKeyframe.cpp"
#include "ContentDescriptorsUtilities.h"

using namespace std;
namespace fs = std::filesystem;
using namespace cv;

const string PATH = "./sports-1m/keyframes/";

// todo docu
// Loads all keyframe pointers (aka 'real' objects).
// These contain filepath, color histogram and cluster number.
vector<LabeledKeyframe*> loadKeyframes(string path)
{
	vector<LabeledKeyframe*> frames;

	int cur = 0;
	filesystem::directory_iterator it{ path };
	while (it != filesystem::directory_iterator{})
	{
		string filepath = it->path().string();

		if (filepath.find(".png") != string::npos)
		{
			Mat img = imread(filepath, IMREAD_COLOR);
			vector<int> hist = getHistogramFromFrame(img); // using 64 bin hist from assignment1

			frames.push_back(new LabeledKeyframe(filepath, hist));

			cout << cur++ / 2529.0 << "%\n";
		}

		++it;
	}

	return frames;
}

int mainSvmClassifier(int argc, char** argv)
{
	vector<LabeledKeyframe*> frames = loadKeyframes(PATH);
	cout << "Calculated histograms for " << frames.size() << " frames\n";

	auto rng = std::default_random_engine{};
	std::shuffle(std::begin(frames), std::end(frames), rng);
	
	return 0;
}
