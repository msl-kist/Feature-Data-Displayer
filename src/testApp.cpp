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
	selectedKeypointRank = 0;
	processed = true;
	isGenuineFirst = false;
	isFixedNormalization = false;

	whole_file_index = 0;

	char name[100];

	sprintf(name, "data/%sresult_genuine.txt",whole_files[whole_file_index]);
	loadFile(name, &genuine_total);
	genuine_total.setColor(ofColor::green);

	sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index]);
	loadFile(name, &impostor_total);
	impostor_total.setColor(ofColor::red);

	current_genuine = &genuine_total;
	current_impostor = &impostor_total;

	loadSortedIndexList(whole_file_index);

	// GUI
	//==============================================================
	drawGUI();

	// GUI의 normalization value를 지정
	setNormalizationValues();
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

		// GUI의 normalization value를 저장
		setNormalizationValues();

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
		ofSetColor(ofColor::green);
		ofCircle(ofPoint(keypoints[i].pt.x, keypoints[i].pt.y), 3);
	}

	// 선택된 점 출력
	if(selectedKeypointIndex != NOT_KEYPOINT)
	{
		ofSetColor(ofColor::red);
		ofCircle(ofPoint(keypoints[selectedKeypointIndex].pt.x, keypoints[selectedKeypointIndex].pt.y), 5);
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

	if(isGenuineFirst)
	{
		current_impostor->draw(image.width, 0, 640, 480, normalizeValue_Impostor);
		current_genuine->draw(image.width, 0, 640, 480, normalizeValue_genuine);
	} else {
		current_genuine->draw(image.width, 0, 640, 480, normalizeValue_genuine);
		current_impostor->draw(image.width, 0, 640, 480, normalizeValue_Impostor);
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

		// GUI에 반영
		ofxUISlider *slider = (ofxUISlider *) gui->getWidget("Score Rank");
		slider->setValue(selectedKeypointRank);
	}
	if(key == OF_KEY_DOWN)
	{
		if(selectedKeypointRank < keypoints.size()-1)
			selectedKeypointRank++;
		selectedKeypointIndex = sortedIndexList[ selectedKeypointRank ];

		processed = false;

		// GUI에 반영
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

		// GUI에 반영
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

		// GUI에 반영
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
	if(gui->isVisible()){
		if( x < _GUI_WIDTH)
			return;
		x -= _GUI_WIDTH;
	}

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
		
		// GUI에 반영
		ofxUISlider *slider = (ofxUISlider *) gui->getWidget("Score Rank");
		slider->setValue(selectedKeypointRank);

		// current histogram 설정
		current_genuine = &genuine;
		current_impostor = &impostor;
	}
	else
	{
		selectedKeypointIndex = NOT_KEYPOINT;

		// current histogram 설정
		current_genuine = &genuine_total;
		current_impostor = &impostor_total;

		setNormalizationValues();

	}
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
	//gui->addSlider("Normalize Genuine", 0, normalizeValue_genuine*2, normalizeValue_genuine, _GUI_WIDTH - iMargin, dim);
	//gui->addSlider("Normalize Impostor", 0, normalizeValue_Impostor*2,  normalizeValue_Impostor, _GUI_WIDTH - iMargin, dim);
	gui->addToggle( "Genuine First", false, dim, dim);
	gui->addTextInput("Normalize Genuine", "Normalize Genuine", length-xInit);
	gui->addTextInput("Normalize Impostor", "Normalize Impostor", length-xInit);
	gui->addToggle( "Fix Normalization Value", false, dim, dim);

	ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();

	if( name == "Score Rank")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		selectedKeypointRank = (int)(slider->getScaledValue()) ;
		selectedKeypointIndex = sortedIndexList[ selectedKeypointRank  ]; 
		//cout << "gui: " << selectedKeypointRank << "th\t" << selectedKeypointIndex << endl;
		processed = false;
	}
	else if( name == "Normalize Genuine" )
	{
		ofxUITextInput *textinput = (ofxUITextInput *) e.widget; 
		if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
			normalizeValue_genuine = ofToFloat(textinput->getTextString());
	}
	else if( name == "Normalize Impostor" )
	{
		ofxUITextInput *textinput = (ofxUITextInput *) e.widget; 
		if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
			normalizeValue_Impostor = ofToFloat(textinput->getTextString());
	}
	else if( name == "Genuine First" )
		isGenuineFirst = !isGenuineFirst;
	else if( name == "Fix Normalization Value" )
		isFixedNormalization = !isFixedNormalization;
}

// 각 score에 의하여 정렬된 index 리스트를 읽음
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

	// Score가 큰게 좋으므로 Reverse!
	std::reverse(sortedIndexList.begin(),sortedIndexList.end());
}

void testApp::setNormalizationValues()
{
	if(isFixedNormalization)
		return;

	normalizeValue_genuine = current_genuine->getMaxCount();
	normalizeValue_Impostor = current_impostor->getMaxCount();

	ofxUITextInput *textinput = (ofxUITextInput *)gui->getWidget("Normalize Genuine");
	textinput->setTextString(ofToString((int)normalizeValue_genuine));
	textinput = (ofxUITextInput *)gui->getWidget("Normalize Impostor");
	textinput->setTextString(ofToString((int)normalizeValue_Impostor));
}
