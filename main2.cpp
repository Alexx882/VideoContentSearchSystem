#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <filesystem>

#include "Keyframe.cpp"
#include "ContentDescriptorsUtilities.h"

using namespace std;
namespace fs = std::filesystem;
using namespace cv;

struct DistanceInfo
{
	Keyframe* frame1;
	Keyframe* frame2;
	double distance;

	DistanceInfo(Keyframe* f1, Keyframe* f2, float d) : frame1(f1), frame2(f2), distance(d)
	{
	}

	bool operator <(DistanceInfo& other)
	{
		return distance < other.distance;
	}

	bool operator >(DistanceInfo& other)
	{
		return distance > other.distance;
	}
};


vector<Keyframe*> loadKeyframes(string path)
{
	vector<Keyframe*> frames;

	filesystem::directory_iterator it{path};
	while (it != filesystem::directory_iterator{})
	{
		string filename = it->path().string();
		//cout << filename << '\n';

		if (filename.find(".jpg") != string::npos)
		{
			Mat img = imread(filename, IMREAD_COLOR);
			vector<int> hist = getHistogramFromFrame(img); // using 64 bin hist from assignment1

			frames.push_back(new Keyframe(filename, hist));
		}

		++it;
	}

	return frames;
}


// Returns a sorted list of distances starting with the smallest distance.
vector<DistanceInfo> calculateAndSortDistances(vector<Keyframe*> frames)
{
	int nElements = frames.size();

	vector<DistanceInfo> distance;

	for (int i = 0; i < nElements; ++i)
		for (int j = i + 1; j < nElements; ++j)
		{
			DistanceInfo di = DistanceInfo(frames[i], frames[j], (*frames[i]).distanceTo(*frames[j]));
			distance.push_back(di);
		}

	// sort ascending based on distance
	sort(distance.begin(), distance.end());

	return distance;
}

int mainKeyframeClustering(int argc, char** argv)
{
	vector<Keyframe*> frames = loadKeyframes("./everest1s/key");
	cout << "Number frames: " << frames.size() << '\n';

	vector<DistanceInfo> distances = calculateAndSortDistances(frames);
	cout << "Number combinations nCr(181, 2): " << distances.size() << '\n';

	// clustering
	map<int, vector<Keyframe*>> clusters = map<int, vector<Keyframe*>>();

	// start with single frame per cluster
	for (int i = 0; i < frames.size(); ++i)
	{
		frames[i]->clusterNo = i;
		clusters.insert(pair<int, vector<Keyframe*>>(i, {frames[i]}));
	}
	cout << "Initial cluster size: " << clusters.size() << '\n';

	// merge the clusters
	for (int i = 0; i < distances.size(); ++i)
	{
		DistanceInfo nextSmallestPair = distances[i];
		int newClusterNo = nextSmallestPair.frame1->clusterNo;
		int oldClusterNo = nextSmallestPair.frame2->clusterNo;

		cout << "dist: " << nextSmallestPair.distance << " ; " << newClusterNo << " : " << oldClusterNo << '\n';

		if (newClusterNo == oldClusterNo)
			continue;

		// load the lists for the two clusters
		auto new_it = clusters.find(newClusterNo);
		auto old_it = clusters.find(oldClusterNo);

		if (old_it == clusters.end() || new_it == clusters.end())
			throw runtime_error("Lost cluster entry");

		// move all nodes from old cluster to new cluster
		for (int i = 0; i < old_it->second.size(); ++i)
		{
			old_it->second[i]->clusterNo = newClusterNo;
			new_it->second.push_back(old_it->second[i]);
		}
		clusters.erase(old_it);

		cout << "New cluster size: " << clusters.size() << '\n';
	}

	cout << clusters.find(frames[0]->clusterNo)->second.size() << '\n';

	return 0;
}
