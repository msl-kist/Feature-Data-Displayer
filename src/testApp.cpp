#include "testApp.h"

#define	_GUI_WIDTH	300

int iMargin = OFX_UI_GLOBAL_WIDGET_SPACING*2;

char * whole_files[3] = {"[Match_count]", "[Original]", "[rep_div_med]"};

//--------------------------------------------------------------
void testApp::setup(){
	// �̹��� �ε�
	//--------------------------------------------------------------
	image.loadImage("data/image.jpg");

	// Ű����Ʈ �ε�
	//--------------------------------------------------------------
	loadKeyPoint();

	selectedKeypointIndex = NOT_KEYPOINT;
	selectedKeypointRank = 0;
	processed = true;

	whole_file_index = 0;

	char name[100];

	sprintf(name, "data/%sresult_genuine.txt",whole_files[whole_file_index]);
	loadFile(name, &genuine_total);
	genuine_total.setColor(ofColor::green);

	sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index]);
	loadFile(name, &impostor_total);
	impostor_total.setColor(ofColor::red);

	loadSortedIndexList(whole_file_index);

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
		genuine.setColor(ofColor::green);

		sprintf(name, "data/impostor/3_%d", selectedKeypointIndex);
		loadFile(name, &impostor);
		impostor.setColor(ofColor::red);

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

	// Ű����Ʈ ���
	//--------------------------------------------------------------
	ofPushStyle();
	for(int i=0; i<keypoints.size();++i)
	{
		ofSetColor(ofColor::green);
		ofCircle(ofPoint(keypoints[i].pt.x, keypoints[i].pt.y), 3);
	}

	// ���õ� �� ���
	if(selectedKeypointIndex != NOT_KEYPOINT)
	{
		ofSetColor(ofColor::red);
		ofCircle(ofPoint(keypoints[selectedKeypointIndex].pt.x, keypoints[selectedKeypointIndex].pt.y), 5);
	}

	//ofNoFill();
	//ofSetColor(ofColor::red);
	//ofCircle(ofPoint(keypoints[839].pt.x, keypoints[839].pt.y),10);
	ofPopStyle();

	// ��� �簢�� ���
	ofPushStyle();
	ofSetColor(ofColor::black);
	ofRect(image.width, 0, 640, 480);
	ofPopStyle();

	// ������׷� ���
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
		genuine_total.setColor(ofColor::green);

		sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index]);
		loadFile(name, &impostor_total);
		impostor_total.setColor(ofColor::red);

		loadSortedIndexList(whole_file_index);
	}
	if(key == 'b')
		gui->toggleVisible();
	if(key == 'f')
		ofToggleFullscreen();

	if(key == OF_KEY_UP)
	{
		if(selectedKeypointRank > 0)
			selectedKeypointRank--;
		selectedKeypointIndex = sortedIndexList[ selectedKeypointRank ];

		processed = false;

		// GUI�� �ݿ�
		ofxUISlider *slider = (ofxUISlider *) gui->getWidget("Score Rank");
		slider->setValue(selectedKeypointRank);
	}
	if(key == OF_KEY_DOWN)
	{
		if(selectedKeypointRank < keypoints.size()-1)
			selectedKeypointRank++;
		selectedKeypointIndex = sortedIndexList[ selectedKeypointRank ];

		processed = false;

		// GUI�� �ݿ�
		ofxUISlider *slider = (ofxUISlider *) gui->getWidget("Score Rank");
		slider->setValue(selectedKeypointRank);
	}

	if(key == OF_KEY_PAGE_UP)
	{
		if(selectedKeypointRank > 10)
			selectedKeypointRank -= 10;
		else 
			selectedKeypointRank = 0;

		selectedKeypointIndex = sortedIndexList[ selectedKeypointRank ];

		processed = false;

		// GUI�� �ݿ�
		ofxUISlider *slider = (ofxUISlider *) gui->getWidget("Score Rank");
		slider->setValue(selectedKeypointRank);
	}
	if(key == OF_KEY_PAGE_DOWN)
	{
		if(selectedKeypointRank < keypoints.size()-11)
			selectedKeypointRank += 10;
		else
			selectedKeypointRank = keypoints.size()-1;
		selectedKeypointIndex = sortedIndexList[ selectedKeypointRank ];

		processed = false;

		// GUI�� �ݿ�
		ofxUISlider *slider = (ofxUISlider *) gui->getWidget("Score Rank");
		slider->setValue(selectedKeypointRank);
	}
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
	if(gui->isVisible())
		x -= _GUI_WIDTH;

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

		vector<int>::iterator iter = find(sortedIndexList.begin(), sortedIndexList.end(), selectedKeypointIndex);
		selectedKeypointRank = (int)(iter - sortedIndexList.begin());

		//cout << "space: " << selectedKeypointRank << "th\t" << selectedKeypointIndex << endl;
		
		// GUI�� �ݿ�
		ofxUISlider *slider = (ofxUISlider *) gui->getWidget("Score Rank");
		slider->setValue(selectedKeypointRank);
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
	gui->addSlider("Score Rank", 0, keypoints.size()-1, selectedKeypointRank, _GUI_WIDTH - iMargin, dim);

	ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();

	ofxUISlider *slider = (ofxUISlider *) e.widget; 
	selectedKeypointRank = (int)(slider->getScaledValue()) ;
	selectedKeypointIndex = sortedIndexList[ selectedKeypointRank  ]; 
	//cout << "gui: " << selectedKeypointRank << "th\t" << selectedKeypointIndex << endl;
	processed = false;
}

// �� score�� ���Ͽ� ���ĵ� index ����Ʈ�� ����
//--------------------------------------------------------------
void testApp::loadSortedIndexList( int whole_file_index )
{
	char name[100];
	sprintf(name, "data/%sindex.txt",whole_files[whole_file_index]);

	FILE *file;
	int index;

	file = fopen(name,"r");

	if(file == NULL)
		cerr << "SORTED FILE NOT FOUND" << endl;

	sortedIndexList.clear();

	while(true){
		fscanf(file,"%d \n",&index);

		sortedIndexList.push_back(index);

		if(feof(file)){
			break;
		}
	}
	fclose(file);

	// Score�� ū�� �����Ƿ� Reverse!
	std::reverse(sortedIndexList.begin(),sortedIndexList.end());
}
