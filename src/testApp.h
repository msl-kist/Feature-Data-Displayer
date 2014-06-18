#pragma once

#include <stdio.h>

#include "ofMain.h"
#include "ofxOpenCv.h"

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

		ofxHistogram genuine_total;
		ofxHistogram impostor_total;
};
