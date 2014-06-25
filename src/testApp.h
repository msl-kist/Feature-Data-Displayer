#pragma once

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"

#include "ofxHistogram/ofxHistogram.h"
#include "ofxHistogram/ofxGaussianHistogram.h"
#include "ofxHistogram/ofxHistogramROC.h"
#include "ROC_Calc.h"


#define MOUSE_THRESHOLD 10

#define NOT_KEYPOINT -1

enum TYPE { GENUINE, IMPOSTOR };

class testApp : public ofBaseApp{

	public:
		void setup();
		void exit();

		void calculateROC();

		void loadFile( char * name, ofxHistogram * distribution, TYPE mode );

		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);

		void setNormalizationValues();

		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void loadKeyPoint();
		void drawGUI();

		ofImage image;

		vector<cv::KeyPoint> keypoints;
		vector<int>			sortedIndexList;
		vector<float>		sortedScoreList;

		int selectedKeypointRank;
		int selectedKeypointIndex;
		bool processed;

		ofxGaussianHistogram genuine;
		ofxGaussianHistogram impostor;

		ofxGaussianHistogram genuine_total;
		ofxGaussianHistogram impostor_total;

		ofxGaussianHistogram * current_genuine;
		ofxGaussianHistogram * current_impostor;

		// GUI 관련
		ofTrueTypeFont verdana;

		ofxUICanvas *gui; 
		void guiEvent(ofxUIEventArgs &e);
		void loadSortedIndexList( int whole_file_index );
		bool loadScoreFiles();
		float normalizeValue_genuine;
		float normalizeValue_Impostor;

		bool isGenuineFirst;
		bool isFixedNormalization;
		bool isGaussianMode;

		vector<string> whole_files;
		int whole_file_index;

		// ROC 관련
		ROC_Calc ROC;
		ofxHistogramROC ROC_curve;
		double EER_Candidate, EER;
		int EER_bin;


		// [윤식] 정렬된 스코어 저장
		void MakeSortedFiles(int keypointCount, const char * prefix);
		void Bubble_sort(vector<double> &Score, vector<cv::KeyPoint> &referenceKeyPoints, vector<int> &index);
		int numOfKeypoints;
};
