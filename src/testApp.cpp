#include "testApp.h"

#define	_GUI_WIDTH	300

int iMargin = OFX_UI_GLOBAL_WIDGET_SPACING*2;

char * whole_files[3] = {"[Match_count]", "[Original]", "[rep_div_med]"};

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

	whole_file_index = 0;

	char name[100];

	sprintf(name, "data/%sresult_genuine.txt",whole_files[whole_file_index]);
	loadFile(name, &genuine_total);
	genuine_total.setColor(ofColor::red);

	sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index]);
	loadFile(name, &impostor_total);
	impostor_total.setColor(ofColor::green);

	// GUI
	//==============================================================
	drawGUI();
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

	if(gui->isVisible()){
		ofPushMatrix();
		ofTranslate(_GUI_WIDTH, 0);
	}

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

	//ofNoFill();
	//ofSetColor(ofColor::red);
	//ofCircle(ofPoint(keypoints[839].pt.x, keypoints[839].pt.y),10);
	ofPopStyle();

	// 배경 사각형 출력
	ofPushStyle();
	ofSetColor(ofColor::black);
	ofRect(image.width, 0, 640, 480);
	ofPopStyle();

	// 히스토그램 출력
	//--------------------------------------------------------------
	ofPushMatrix();
	ofPushStyle();
	ofSetColor(ofColor::white);
	if(selectedKeypointIndex == NOT_KEYPOINT)
	{ 
		int max_g, max_i;
		genuine_total.getMaxMinCount(&max_g);
		impostor_total.getMaxMinCount(&max_i);
		int max = max_g > max_i ? max_g : max_i;
		
		genuine_total.draw(image.width, 0, 640, 480, max_g);
		impostor_total.draw(image.width,0, 640, 480, max_i);
	} else {
		int max_g, max_i;
		genuine.getMaxMinCount(&max_g);
		impostor.getMaxMinCount(&max_i);
		int max = max_g > max_i ? max_g : max_i;

		genuine.draw(image.width, 0, 640, 480, max_g);
		impostor.draw(image.width,0, 640, 480, max_i);
	}
	ofPopStyle();
	ofPopMatrix();

	if(gui->isVisible())
		ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' ')
	{
		char name[100];

		whole_file_index = (whole_file_index + 1) % 3;

		sprintf(name, "data/%sresult_genuine.txt",whole_files[whole_file_index]);
		loadFile(name, &genuine_total);
		genuine_total.setColor(ofColor::red);

		sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index]);
		loadFile(name, &impostor_total);
		impostor_total.setColor(ofColor::green);
	}
	if(key == 'b')
		gui->toggleVisible();
	if(key == 'f')
		ofToggleFullscreen();
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

void testApp::drawGUI()
{
	float dim = 16; 
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
	float length = _GUI_WIDTH - OFX_UI_GLOBAL_WIDGET_SPACING; 

	gui = new ofxUICanvas(0, 0, _GUI_WIDTH, ofGetScreenHeight());
	gui->setColorBack(ofColor::lightSkyBlue);
	gui->setColorFill(ofColor::white);

	gui->addWidgetDown(new ofxUILabel("KEYPOINT DATA VISUALIZER", OFX_UI_FONT_MEDIUM)); 
	gui->addSpacer(_GUI_WIDTH-iMargin, 4);
	//------------------------------
	gui->addWidgetDown(new ofxUILabel("[b]: Menu on/off", OFX_UI_FONT_SMALL));
	gui->addWidgetDown(new ofxUILabel("[F]: Full screen on/off", OFX_UI_FONT_SMALL));
	gui->addSpacer(_GUI_WIDTH-iMargin, 1);
	//------------------------------
	float tt = 0;
	gui->addSlider("CONTRAST", 1, 255, tt, _GUI_WIDTH - iMargin, dim);
	//gui->addSlider("8", 0.0, 255.0, 150, dim*3, 350);


	//gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	//gui->addSlider("CONTRAST", 1, 10, fContrast, _GUI_WIDTH - iMargin, dim);
	//gui->addLabelButton("TRAIN", false, _GUI_WIDTH - iMargin, 0);
	//gui->addSpacer(_GUI_WIDTH-iMargin, 1);
}
