#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#pragma once
class FrameComparer
{

private:
	const int T_H = 170000;
	const int T_D = 20000;

	int frameSize;
	int accumulated_sad;

public:
	FrameComparer(int frameSize)
	{
		this->frameSize = frameSize;
		this->accumulated_sad = 0;
	}

	// Returns true if T_H was exceeded immediately or T_D was exceeded gradually.
	bool isNextTwinComparisonExceeded(vector<int> f1, vector<int> f2)
	{
		// Sum of absolute differences with Manhattan distance
		int sad = 0;
		for (uchar p = 0; p < this->frameSize; ++p)
			sad += std::abs(f1[p] - f2[p]);
				
		bool exceeded = false;

		if (sad > T_H) {
			exceeded = true;
		}
		else if (sad > T_D) {
			accumulated_sad += sad - T_D;
			if (accumulated_sad > T_H)
				exceeded = true;
		}
		else {
			accumulated_sad = 0;
		}

		cout << "sad=" << sad << " | " << accumulated_sad << endl;
		if (exceeded) {
			// reset accumulated_sad if boundary is found
			accumulated_sad = 0;

			cout << endl << "### new shot started" << endl;
		}

		return exceeded;
	}

};

