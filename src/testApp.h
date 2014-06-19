#pragma once

#include <stdio.h>

#include "ofMain.h"
#include "ofxOpenCv.h"

#include "ofxHistogram/ofxHistogram.h"
#include "ofxHistogram/ofxHistogramROC.h"
#include "ROC_Calc.h"


#define MOUSE_THRESHOLD 10

#define NOT_KEYPOINT -1

enum TYPE { GENUINE, IMPOSTOR };

class testApp : public ofBaseApp{

	public:
		void setup();

		void loadFile( char * name, ofxHistogram * distribution, TYPE mode );

		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void loadKeyPoint();

		ofImage image;

		vector<cv::KeyPoint> keypoints;

		int selectedKeypointIndex;
		bool processed;

		ofxHistogram genuine;
		ofxHistogram impostor;

		int whole_file_index;

		ofxHistogram genuine_total;
		ofxHistogram impostor_total;

		ROC_Calc ROC;
		ofxHistogramROC ROC_curve;
		double EER_Candidate, EER;
		int EER_bin;
};
