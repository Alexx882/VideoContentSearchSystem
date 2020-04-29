#include <filesystem>

using namespace std;

// A single keyframe used by the clustering algorithm.
class Keyframe
{
public:
	string filepath;
	vector<int> hist;
	int clusterNo;

	Keyframe(string filepath, vector<int> hist)
	{
		this->filepath = filepath;
		this->hist = hist;
	}

	// Returns the Euclidean distance to the other keyframe based on their histograms.
	double distanceTo(Keyframe other)
	{
		if (this->hist.size() != other.hist.size())
			throw invalid_argument("Histograms must have same size.");

		double dist = 0;
		for (int i = 0; i < this->hist.size(); ++i)
			dist += std::pow(this->hist[i] - other.hist[i], 2);

		return std::pow(dist, .5);
	}
};

// Contains information about the distance between two keyframes.
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
