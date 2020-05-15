#include <filesystem>
#include <iostream>

using namespace std;

// A keyframe used for classification.
class LabeledKeyframe
{
public:
	string filepath;
	vector<int> hist;

	int label;
	string labelStr;

	int predictedLabel;
	string predictedLabelStr;

	// Creates a new instance where the label is set from the filepath.
	LabeledKeyframe(string filepath, vector<int> hist)
	{
		this->filepath = filepath;
		this->hist = hist;

		string filename = filepath.substr(filepath.find_last_of("/") + 1, filepath.find_last_not_of(" "));
		string labelStr = filename.substr(0, filename.find_first_of("_"));
		this->labelStr = labelStr;
		this->label = getLabelIdFromString(labelStr);
	}

	void setPrediction(int label)
	{
		this->predictedLabel = label;
		this->predictedLabelStr = getLabelStrFromId(label);
	}

	// Mapping for bmx = 11, parachuting = 479, marathon = 211, skateboarding = 187.
	static int getLabelIdFromString(string label)
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

	// Inverse mapping for bmx = 11, parachuting = 479, marathon = 211, skateboarding = 187.
	static string getLabelStrFromId(int label)
	{
		switch (label)
		{
		case 11:
			return "bmx";
		case 479:
			return "parachuting";
		case 211:
			return "marathon";
		case 187:
			return "skateboarding";
		default:
			throw invalid_argument("label is unknown");
		}
	}
};
