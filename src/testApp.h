#pragma once

#include <stdio.h>

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"

#include "ofxHistogram/ofxHistogram.h"

#define MOUSE_THRESHOLD 10

#define NOT_KEYPOINT -1

class testApp : public ofBaseApp{

	public:
		void setup();

		void loadFile( char * name, ofxHistogram * distribution );

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

		int selectedKeypointRank;
		int selectedKeypointIndex;
		bool processed;

		ofxHistogram genuine;
		ofxHistogram impostor;

		int whole_file_index;

		ofxHistogram genuine_total;
		ofxHistogram impostor_total;

		ofxHistogram * current_genuine;
		ofxHistogram * current_impostor;

		ofTrueTypeFont verdana;

		ofxUICanvas *gui; 
		void guiEvent(ofxUIEventArgs &e);
		void loadSortedIndexList( int whole_file_index );

		float normalizeValue_genuine;
		float normalizeValue_Impostor;

		bool isGenuineFirst;
		bool isFixedNormalization;
};
