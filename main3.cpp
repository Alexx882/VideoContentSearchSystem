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
using namespace cv::ml;

const vector<string> PATHS = {
	"./sports-1m/bmx/", "./sports-1m/marathon/", "./sports-1m/parachuting/", "./sports-1m/skateboarding/"
};
const float TRAIN_PCT = .9;
const int HIST_BINS = 64; // 64 or 256 only.

// todo docu
// Loads all keyframe pointers (aka 'real' objects).
// These contain filepath, color histogram and cluster number.
vector<LabeledKeyframe*> loadKeyframes(string path)
{
	vector<LabeledKeyframe*> frames;

	int cur = 0;
	filesystem::directory_iterator it{path};
	while (it != filesystem::directory_iterator{})
	{
		string filepath = it->path().string();

		if (filepath.find(".png") != string::npos)
		{
			Mat img = imread(filepath, IMREAD_COLOR);
			vector<int> hist;
			if (HIST_BINS == 64)
				hist = getHistogramFromFrame(img); // using 64 bin hist from assignment1
			else
				hist = getLargeHistogramFromFrame(img);

			frames.push_back(new LabeledKeyframe(filepath, hist));

			cout << cur++ / 2529.0 << "%\n";
		}

		++it;
	}

	return frames;
}

void printRes(vector<vector<LabeledKeyframe*>> correctClassFrames,
              vector<vector<LabeledKeyframe*>> incorrectClassFrames);


int mainSvmClassifier(int argc, char** argv)
{
	vector<vector<LabeledKeyframe*>> keyframeClasses;
	std::default_random_engine rng(std::random_device{}());
	int cntTrainData = 0;

	// load all keyframes and shuffle per class
	for (int i = 0; i < PATHS.size(); ++i)
	{
		string path = PATHS[i];

		vector<LabeledKeyframe*> frames = loadKeyframes(path);
		cout << "Calculated histograms for " << frames.size() << " frames\n";
		cntTrainData += floor(frames.size() * TRAIN_PCT);

		std::shuffle(std::begin(frames), std::end(frames), rng);

		keyframeClasses.push_back(frames);
	}


	// Step 2: Set up training data
	// Remember Mat constructor, type: CV_<bit_depth>(S|U|F)C<number_of_channels>
	Mat trainingDataMat(cntTrainData, HIST_BINS, CV_32FC1);
	Mat labelsMat(cntTrainData, 1, CV_32SC1);

	//copy data (NOTE: each image is one row of image with label at the end)
	int curTrainingNr = 0;
	for (int classIdx = 0; classIdx < keyframeClasses.size(); ++classIdx)
	{
		auto curTrainingClass = keyframeClasses[classIdx];
		for (int frameIdx = 0; frameIdx < floor(curTrainingClass.size() * TRAIN_PCT); ++frameIdx)
		{
			auto curTrainingFrame = curTrainingClass[frameIdx];
			for (int bin = 0; bin < HIST_BINS; ++bin)
			{
				trainingDataMat.at<float>(curTrainingNr, bin) = curTrainingFrame->hist[bin];
			}
			labelsMat.at<int>(curTrainingNr) = curTrainingFrame->label;
			++curTrainingNr;
		}
	}

	// Step 3: Train the SVM with the first part of data (e.g. 80%)
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 1e-6));
	svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);
	cout << "trained model\n";

	// Step 4: Test the SVM with the second part of data (i.e. 20%)
	double correct = 0, total = 0;
	vector<vector<LabeledKeyframe*>> correctClassFrames(4), incorrectClassFrames(4);
	for (int classIdx = 0; classIdx < keyframeClasses.size(); ++classIdx)
	{
		auto curTestClass = keyframeClasses[classIdx];
		for (int frameIdx = floor(curTestClass.size() * TRAIN_PCT) + 1; frameIdx < curTestClass.size(); ++frameIdx)
		{
			auto curTestFrame = curTestClass[frameIdx];

			Mat sampleHist(1, HIST_BINS, CV_32FC1);
			for (int k = 0; k < HIST_BINS; k++)
			{
				sampleHist.at<float>(0, k) = curTestFrame->hist[k];
			}
			int response = (int)svm->predict(sampleHist);

			curTestFrame->setPrediction(response);
			if (response == curTestFrame->label)
			{
				correctClassFrames[classIdx].push_back(curTestFrame);
				correct++;
			}
			else
			{
				incorrectClassFrames[classIdx].push_back(curTestFrame);
			}
			++total;
		}
	}

	cout << "Correct: " << correct << " from " << total << " (" << correct / total << ")\n";

	printRes(correctClassFrames, incorrectClassFrames);

	return 0;
}

string generateHtmlForClassFrames(vector<vector<LabeledKeyframe*>> classFrames)
{
	stringstream html;

	for (int i = 0; i < classFrames.size(); ++i)
	{
		auto correctFrames = classFrames[i];
		for (int j = 0; j < min(3, (int)correctFrames.size()); ++j)
		{
			auto curFrame = correctFrames[j];
			html << "<img style=\"width:50%; padding:5px;\" src=\"" << curFrame->filepath << "\">\n";
			html << "<p>From: " << curFrame->labelStr << ", Classified as: " << curFrame->predictedLabelStr << "</p>\n";
		}
	}

	return html.str();
}

void printRes(vector<vector<LabeledKeyframe*>> correctClassFrames,
              vector<vector<LabeledKeyframe*>> incorrectClassFrames)
{
	stringstream html;

	html << "<h1>Correctly Classified Frames</h1>";
	html << generateHtmlForClassFrames(correctClassFrames);

	html << "<h1>Incorrectly Classified Frames</h1>";
	html << generateHtmlForClassFrames(incorrectClassFrames);

	ofstream myfile;
	//fs::create_directory("reports"); // use mkdir(folderName) for linux
	myfile.open("classification_report.html");
	myfile << html.str();
	myfile.close();
}
