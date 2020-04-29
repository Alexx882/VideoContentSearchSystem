#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <filesystem>

#include "Keyframe.cpp"
#include "ContentDescriptorsUtilities.h"

using namespace std;
namespace fs = std::filesystem;
using namespace cv;

const string PATH = "./everest1s/highkey/";

// Loads all keyframe pointers (aka 'real' objects).
// These contain filepath, color histogram and cluster number.
vector<Keyframe*> loadKeyframes(string path)
{
	vector<Keyframe*> frames;

	filesystem::directory_iterator it{path};
	while (it != filesystem::directory_iterator{})
	{
		string filename = it->path().string();
		cout << filename << '\n';

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

// Generates a report in HTML.
void generateReport(map<int, vector<Keyframe*>> clusters)
{
	stringstream html;

	auto it = clusters.begin();
	int cnt = 0;
	while (it != clusters.end())
	{
		html << "<h1>Cluster " << cnt++ << "</h1>";
		for (int i = 0; i < it->second.size(); ++i)
		{
			html << "<img style=\"width:100px; padding:5px;\" src=\"." << it->second[i]->filepath << "\">";
		}
		++it;
	}

	ofstream myfile;
	fs::create_directory("reports"); // use mkdir(folderName) for linux
	myfile.open("reports/report_" + to_string(clusters.size()) + ".html");
	myfile << html.str();
	myfile.close();
}

// Runs the hierarchical clustering based on single-linkage.
// The distances vector must be sorted ascending by distance, as it will be iterated directly.
// Reports are generated for all importantClusterSizes.
void runClustering(vector<Keyframe*> frames, vector<DistanceInfo> distances, vector<int> importantClusterSizes)
{
	map<int, vector<Keyframe*>> clusters = map<int, vector<Keyframe*>>();

	// start with single frame per cluster
	for (int i = 0; i < frames.size(); ++i)
	{
		frames[i]->clusterNo = i;
		clusters.insert(pair<int, vector<Keyframe*>>(i, {frames[i]}));
	}
	cout << "Initial number of clusters: " << clusters.size() << '\n';

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
		auto newCluster_it = clusters.find(newClusterNo);
		auto oldCluster_it = clusters.find(oldClusterNo);

		if (oldCluster_it == clusters.end() || newCluster_it == clusters.end())
			throw runtime_error("Lost cluster entry");

		// move all nodes from old cluster to new cluster
		for (int i = 0; i < oldCluster_it->second.size(); ++i)
		{
			oldCluster_it->second[i]->clusterNo = newClusterNo;
			newCluster_it->second.push_back(oldCluster_it->second[i]);
		}
		clusters.erase(oldCluster_it);

		cout << "New number of clusters: " << clusters.size() << '\n';

		if (count(importantClusterSizes.begin(), importantClusterSizes.end(), clusters.size()))
			generateReport(clusters);

		if (clusters.size() == 1)
			// end condition reached
			break;
	}

	cout << "Final number of clusters: " << clusters.size()
		<< " with " << clusters.find(frames[0]->clusterNo)->second.size() << " elements\n";
}

int mainKeyframeClustering(int argc, char** argv)
{
	vector<Keyframe*> frames = loadKeyframes(PATH);
	cout << "Number frames: " << frames.size() << '\n';

	vector<DistanceInfo> distances = calculateAndSortDistances(frames);
	cout << "Number combinations nCr(181, 2): " << distances.size() << '\n';

	fs::remove_all("reports");
	runClustering(frames, distances, {150, 50, 30, 10, 5});

	return 0;
}
