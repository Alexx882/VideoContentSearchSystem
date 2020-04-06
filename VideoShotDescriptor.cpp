#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct Shot
{
	int number;
	
	int start;
	int end;
};

#pragma once
// This class stores video shots calculated from boundaries added with 'addBoundary'. 'addLastBoundary' has to be called to finalize the entries.
class VideoShotDescriptor
{

private:
	std::list<int> boundaries;
	std::vector<Shot> shots;

public:
	VideoShotDescriptor()
	{
		this->boundaries = {};
		this->shots = {};
	}

	// Adds a shot boundary with semantics:
	// current shot end exclusive, next shot start inclusive
	void addBoundary(int frameNr)
	{
		boundaries.push_back(frameNr);
	}

	// Adds the last shot boundary and calculates the shots.
	void addLastBoundary(int frameNr)
	{
		if(boundaries.back() != frameNr)
			addBoundary(frameNr);
				
		int cnt = 0;
		int lastStart = 0;
		for(int boundary : boundaries)
		{
			if (boundary == 0)
				continue;

			Shot s = { cnt, lastStart, boundary - 1 };
			shots.push_back(s);

			++cnt;
			lastStart = boundary;
		}
	}

	// Prints all shots added with 'addBoundary' and 'addLastBoundary'
	void printShots()
	{
		for (int i = 0; i < shots.size(); ++i)
		{
			Shot shot = shots[i];
			cout << "Shot " << shot.number << ": " << shot.start << " - " << shot.end << endl;
		}
	}

	// Returns the indices of the center keyframe for each shot
	vector<int> getKeyFrameNumbers()
	{
		vector<int> keyframes = {};
		
		for (Shot s : shots)
			keyframes.push_back((s.end + s.start) / 2);

		return keyframes;
	}

};

