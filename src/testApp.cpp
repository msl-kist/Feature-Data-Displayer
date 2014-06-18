#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	// 이미지 로드
	//--------------------------------------------------------------
	image.loadImage("data/image.jpg");

	// 키포인트 로드
	//--------------------------------------------------------------
	loadKeyPoint();

	selectedKeypointIndex = NOT_KEYPOINT;
	processed = true;

	char name[100];

	sprintf(name, "data/result_genuine.txt");
	loadFile(name, &genuine_total);
	genuine_total.setColor(ofColor(200, 50, 50, 100));

	sprintf(name, "data/result_imposter.txt");
	loadFile(name, &impostor_total);
	impostor_total.setColor(ofColor(50, 200, 50, 0));
}

//--------------------------------------------------------------
void testApp::update(){
	if(!processed)
	{
		genuine.clear();
		impostor.clear();

		char name[100];

		sprintf(name, "data/genuine/2_%d",selectedKeypointIndex);
		loadFile(name, &genuine);
		genuine.setColor(ofColor(200, 50, 50, 100));

		sprintf(name, "data/impostor/3_%d", selectedKeypointIndex);
		loadFile(name, &impostor);
		impostor.setColor(ofColor(50, 200, 50, 0));

		processed = true;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackgroundGradient(ofColor::black, ofColor::dimGray);
	image.draw(0, 0);

	// 키포인트 출력
	//--------------------------------------------------------------
	ofPushStyle();
	for(int i=0; i<keypoints.size();++i)
	{
		if(i == selectedKeypointIndex)
			ofSetColor(ofColor::lightYellow);
		else
			ofSetColor(ofColor::green);
		ofCircle(ofPoint(keypoints[i].pt.x, keypoints[i].pt.y), 3);
	}
	ofPopStyle();

	// 배경 사각형 출력
	ofPushStyle();
	ofSetColor(ofColor::darkGrey);
	ofRect(image.width, 0, 640, 480);
	ofPopStyle();

	if(selectedKeypointIndex == NOT_KEYPOINT)
	{
		genuine_total.draw(image.width, 0, 640, 480);
		impostor_total.draw(image.width,0, 640, 480);
	} else {
		genuine.draw(image.width, 0, 640, 480);
		impostor.draw(image.width,0, 640, 480);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	float min_distance = 100000000000000;
	int min_index = 0;
	for(int i=0; i<keypoints.size(); ++i)
	{
		cv::KeyPoint *curr = &(keypoints[i]);
		float distance = (curr->pt.x - x)*(curr->pt.x - x) + (curr->pt.y - y)*(curr->pt.y - y);
		if( distance < min_distance)
		{
			min_distance = distance;
			min_index = i;
		}
	}
	if(min_distance <= MOUSE_THRESHOLD){
		selectedKeypointIndex = min_index;
		processed = false;
	}
	else
		selectedKeypointIndex = NOT_KEYPOINT;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::loadFile( char * name, ofxHistogram * distribution )
{
	FILE *file;
	int distance, count; 

	file = fopen(name,"r");

	while(true){
		fscanf(file,"%d %d\n",&distance,&count);

		distribution->assign(distance, count);
		if(feof(file)){
			break;
		}
	}
	fclose(file);

	distribution->resize(0, distance);
	distribution->normalize();
}

//--------------------------------------------------------------
void testApp::loadKeyPoint()
{
	//File open 
	cv::FileStorage fs("data/Data.xml",cv::FileStorage::READ);

	if(!fs.isOpened())
	{
		cout << "FILE NOT FOUND";
		return;
	}

	//Read reference image keypoints and descriptors
	char name[100];
	sprintf(name, "K_%d_%d_%d_%d",1, -10, -1, -1);
	cv::FileNode features = fs[name];
	read(features, keypoints);
}
