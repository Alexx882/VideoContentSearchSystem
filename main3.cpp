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
const int HIST_BINS = 256; // 64 or 256 only.


// Loads all keyframe pointers including their histogram and labels from the given path.
vector<LabeledKeyframe*> loadKeyframes(string path)
{
	vector<LabeledKeyframe*> frames;

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
		}

		++it;
	}

	return frames;
}

// Returns an HTML report for the given classes with 3 examples each.
string getHtmlForClassFrames(vector<vector<LabeledKeyframe*>> frameClasses)
{
	stringstream html;

	for (int i = 0; i < frameClasses.size(); ++i)
	{
		auto curClassFrames = frameClasses[i];
		for (int j = 0; j < min(3, (int)curClassFrames.size()); ++j)
		{
			auto curFrame = curClassFrames[j];
			html << "<img style=\"width:50%; padding:5px;\" src=\"" << curFrame->filepath << "\">\n";
			html << "<p>From: " << curFrame->labelStr << ", Classified as: " << curFrame->predictedLabelStr << "</p>\n";
		}
	}

	return html.str();
}

// Generates an HTML report for some of the correctly and incorrectly labeled frames.
void generateHtmlReport(vector<vector<LabeledKeyframe*>> correctClassFrames,
	vector<vector<LabeledKeyframe*>> incorrectClassFrames)
{
	stringstream html;

	html << "<h1>Correctly Classified Frames</h1>";
	html << getHtmlForClassFrames(correctClassFrames);

	html << "<h1>Incorrectly Classified Frames</h1>";
	html << getHtmlForClassFrames(incorrectClassFrames);

	ofstream myfile;
	myfile.open("classification_report.html");
	myfile << html.str();
	myfile.close();
}

int mainSvmClassifier(int argc, char** argv)
{
	vector<vector<LabeledKeyframe*>> keyframeClasses;
	std::default_random_engine rng(std::random_device{}());
	//auto rng = std::default_random_engine{};
	int cntTrainData = 0;

	// load all keyframes and shuffle per class
	int nProcessed = 0;
	for (int i = 0; i < PATHS.size(); ++i)
	{
		string path = PATHS[i];
		vector<LabeledKeyframe*> frames = loadKeyframes(path);
		
		std::shuffle(std::begin(frames), std::end(frames), rng);
		keyframeClasses.push_back(frames);
		cntTrainData += floor(frames.size() * TRAIN_PCT);

		nProcessed += frames.size();
		cout << "Calculated histograms for " << frames.size() << " frames in " << path << "\n";
		cout << "Finished " << nProcessed / 2495.0 * 100 << "%\n";
	}

	// Step 2: Set up training data
	Mat trainingDataMat(cntTrainData, HIST_BINS, CV_32FC1);
	Mat labelsMat(cntTrainData, 1, CV_32SC1);

	// copy data with TRAIN_PCT frames per class
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

	// Step 3: Train the SVM with the first part of data 
	auto start = std::chrono::high_resolution_clock::now();
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR); // RBF is worse
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 1e-6));
	svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);
	std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
	cout << "SVM setup and training finished in " << elapsed.count() << " seconds for " << cntTrainData << " elements\n";

	// Step 4: Test the SVM with the second part of data
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
	generateHtmlReport(correctClassFrames, incorrectClassFrames);

	return 0;
}
