#include <filesystem>
#include <iostream>

using namespace std;

// todo docu
class LabeledKeyframe
{
public:
	string filepath;
	vector<int> hist;
	string labelStr;
	int label;

	LabeledKeyframe(string filepath, vector<int> hist)
	{
		this->filepath = filepath;
		this->hist = hist;
		
		string filename = filepath.substr(filepath.find_last_of("/")+1, filepath.find_last_not_of(" "));
		string labelStr = filename.substr(0, filename.find_first_of("_"));
		this->labelStr = labelStr;
		this->label = getLabelId(labelStr);
	}

	// bmx = 11, parachuting = 479, marathon = 211, skateboarding = 187
	int getLabelId(string label)
	{
		if (label == "bmx")
			return 11;

		if (label == "parachuting")
			return 479;

		if (label == "marathon")
			return 211;

		if (label == "skateboarding")
			return 187;

		throw invalid_argument("label is unknown");
	}
};
