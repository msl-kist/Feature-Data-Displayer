#include "testApp.h"

#define	_GUI_WIDTH	300

int iMargin = OFX_UI_GLOBAL_WIDGET_SPACING*2;

//char * whole_files[3] = {"[Match_count]", "[Original]", "[rep_div_med]"};

#define Percent2Number_ofKeypoints() (percentOfKeypoints/100 * keypoints.size())
#define Number2Percent_ofKeypoints() ((float)numOfKeypoints / (float)keypoints.size() * 100)

//--------------------------------------------------------------
void testApp::setup(){
	// Score 파일 로드
	loadScoreFiles();

	// 이미지 로드
	//--------------------------------------------------------------
	image.loadImage("data/image.jpg");

	// 키포인트 로드
	//--------------------------------------------------------------
	loadKeyPoint();

	numOfKeypoints = 300;
	percentOfKeypoints = Number2Percent_ofKeypoints();

	selectedKeypointIndex = NOT_KEYPOINT;
	selectedKeypointRank = 0;
	processed = true;
	isGenuineFirst = false;
	isFixedNormalization = false;
	isGaussianMode = false;

	whole_file_index = 0;

	char name[100];
	ROC.init();

	sprintf(name, "data/%sresult_genuine.txt",whole_files[whole_file_index].c_str());
	loadFile(name, &genuine_total, GENUINE);
	genuine_total.setColor(ofColor::green);

	sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index].c_str());
	loadFile(name, &impostor_total, IMPOSTOR);
	impostor_total.setColor(ofColor::red);

	current_genuine = &genuine_total;
	current_impostor = &impostor_total;

	loadSortedIndexList(whole_file_index);

	// GUI
	//==============================================================
	drawGUI();

	// GUI의 normalization value를 지정
	setNormalizationValues();

	ROC.update();
	calculateROC();
}

void testApp::exit(){
	// 스코어 세팅 파일 저장
	//--------------------------------------------------------------
	ofstream settings("data/score.settings");

	for(int i=0; i<whole_files.size(); ++i)
		settings << whole_files[i] << endl;
	settings.close();
}


//--------------------------------------------------------------
void testApp::update(){
	if(!processed)
	{
		genuine.clear();
		impostor.clear();

		ROC_curve.clear();

		char name[100];

		sprintf(name, "data/genuine/2_%d",selectedKeypointIndex);
		loadFile(name, &genuine, GENUINE);
		genuine.setColor(ofColor::green);

		sprintf(name, "data/impostor/3_%d", selectedKeypointIndex);
		loadFile(name, &impostor, IMPOSTOR);
		impostor.setColor(ofColor::red);

		// GUI의 normalization value를 저장
		setNormalizationValues();

		// GUI의 Score value를 저장
		ofxUILabel * label = (ofxUILabel*)gui->getWidget("Score");
		label->setLabel( ofToString( sortedScoreList[selectedKeypointRank] ));

		// GUI의 index를 저장
		label = (ofxUILabel *) gui->getWidget("Index");
		label->setLabel( ofToString( selectedKeypointIndex ) );

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

	if(!isGaussianMode)		// 일반 Histogram 모드
	{	
		if(isGenuineFirst)
		{
			current_impostor->draw(image.width, 0, 640, 480, normalizeValue_Impostor);
			current_genuine->draw(image.width, 0, 640, 480, normalizeValue_genuine);
		} else {
			current_genuine->draw(image.width, 0, 640, 480, normalizeValue_genuine);
			current_impostor->draw(image.width, 0, 640, 480, normalizeValue_Impostor);
		}
	} else {				// Gaussian Distribution
		if(isGenuineFirst)
		{
			current_impostor->drawGaussian(image.width, 0, 640, 480, normalizeValue_Impostor);
			current_genuine->drawGaussian(image.width, 0, 640, 480, normalizeValue_genuine);
		} else {
			current_genuine->drawGaussian(image.width, 0, 640, 480, normalizeValue_genuine);
			current_impostor->drawGaussian(image.width, 0, 640, 480, normalizeValue_Impostor);
		}
		string str = "[Genuine] mean: " + ofToString(current_genuine->mean) + "     stdev: " + ofToString(current_genuine->variance);
		ofDrawBitmapString(str, image.width + 10, 20);
		str = "[Impostor] mean: " + ofToString(current_impostor->mean) + "     stdev: " + ofToString(current_impostor->variance);
		ofDrawBitmapString(str, image.width + 10, 40);
	}
	ROC_curve.draw(image.width, 500, 500, 480, EER_bin);

	if(selectedKeypointIndex == NOT_KEYPOINT)
		ROC_curve.draw(image.width, 500, 500, 480, EER_bin);
	
	ofPopStyle();
	ofPopMatrix();

	if(gui->isVisible())
		ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(key == ' ')
	{
		whole_file_index = (whole_file_index+1) % whole_files.size();

		ChangeScoreFunction();
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

		// ROC 재계산
		calculateROC();

		// current histogram 설정
		current_genuine = &genuine_total;
		current_impostor = &impostor_total;

		setNormalizationValues();

		ofxUILabel* l = (ofxUILabel*) gui->getWidget("EER_Value");
		l->setLabel(ofToString(EER));
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


void testApp::loadFile( char * name, ofxHistogram * distribution, TYPE mode)
{
	FILE *file;
	int distance, count; 

	file = fopen(name,"r");

	while(true){
		fscanf(file,"%d %d\n",&distance,&count);

		distribution->assign(distance, count);

		if(mode == GENUINE) 
			ROC.Genuine.push_back(std::make_pair(distance, count));
		else 
			ROC.Imposter.push_back(std::make_pair(distance, count));	

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
	//------------------------------
	gui->addLabel("Score Value: ", OFX_UI_FONT_SMALL);
	gui->addWidgetRight(new ofxUILabel("Score", OFX_UI_FONT_MEDIUM));
	gui->addLabel("KP Index: ", OFX_UI_FONT_SMALL);
	gui->addWidgetRight(new ofxUILabel("Index", OFX_UI_FONT_MEDIUM));

	gui->addSpacer(_GUI_WIDTH-iMargin, 1);
	gui->addToggle( "Genuine First", false, dim, dim);
	gui->addToggle( "Gaussian Mode", false, dim, dim);
	gui->addTextInput("Normalize Genuine", "Normalize Genuine", length-xInit);
	gui->addTextInput("Normalize Impostor", "Normalize Impostor", length-xInit);
	gui->addToggle( "Fix Normalization Value", false, dim, dim);
	gui->addSpacer(_GUI_WIDTH-iMargin, 1);
	//------------------------------

	gui->addSpacer(_GUI_WIDTH-iMargin, 1);
	gui->addWidgetDown(new ofxUILabel(" ", OFX_UI_FONT_SMALL));
	gui->addWidgetDown(new ofxUILabel("EER value: ", OFX_UI_FONT_SMALL));
	gui->addWidgetRight(new ofxUILabel("EER_Value", OFX_UI_FONT_MEDIUM));

	//--------------------------------------------------------------
	gui->addSpacer(_GUI_WIDTH-iMargin, 1);
	gui->addLabel("lblCurrentScore", "Current: ", OFX_UI_FONT_SMALL);
	gui->addWidgetRight(new ofxUILabel("Metric", whole_files[whole_file_index], OFX_UI_FONT_MEDIUM));
	gui->addLabel("lblNewScore", "New: ", OFX_UI_FONT_SMALL);
	gui->addWidgetRight(new ofxUITextInput("NewPrefix", "New Prefix", length-xInit - 50));
	gui->addTextInput("percentOfKeyPoints", ofToString(percentOfKeypoints), length-xInit-50)->setAutoClear(true);
	gui->addWidgetRight(new ofxUILabel("%", OFX_UI_FONT_MEDIUM));
	gui->addLabelButton("PROCESS NEW SCORE", false, length-xInit);

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
	else if( name == "Gaussian Mode" ){
		isGaussianMode = !isGaussianMode;

		setNormalizationValues();
	}
	else if( name == "PROCESS NEW SCORE")
	{
		ofxUIButton *btn = (ofxUIButton*)e.widget;
		if(btn->getValue() == true)		// 버튼 클릭됨
		{
			// Prefix 처리
			ofxUITextInput *textinput = (ofxUITextInput*) gui->getWidget("NewPrefix");

			whole_files.push_back(textinput->getTextString());
			whole_file_index = whole_files.size()-1;

			textinput = (ofxUITextInput *)gui->getWidget("percentOfKeyPoints");

			percentOfKeypoints = ofToFloat(textinput->getTextString());
			numOfKeypoints = Percent2Number_ofKeypoints();

			MakeSortedFiles(numOfKeypoints, whole_files[whole_file_index].c_str());
		}

	}
}

// 각 score에 의하여 정렬된 index 리스트를 읽음
//--------------------------------------------------------------
void testApp::loadSortedIndexList( int whole_file_index )
{
	char name[100];
	sprintf(name, "data/%sindex.txt",whole_files[whole_file_index].c_str());

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

	sprintf(name, "data/%sScoresort.txt", whole_files[whole_file_index].c_str());
	file = fopen(name, "r");
	if(file == NULL)
		cerr << "SORTED FILE NOT FOUND" << endl;

	sortedScoreList.clear();

	float score;

	while(true){
		fscanf(file,"%f \n",&score);

		sortedScoreList.push_back(score);

		if(feof(file)){
			break;
		}
	}
	fclose(file);

	// Score가 큰게 좋으므로 Reverse!
	std::reverse(sortedScoreList.begin(),sortedScoreList.end());
}

void testApp::setNormalizationValues()
{
	if(isFixedNormalization)
		return;

	if(isGaussianMode)
	{
		normalizeValue_genuine = 1.0;
		normalizeValue_Impostor = 1.0;

		ofxUITextInput *textinput = (ofxUITextInput *)gui->getWidget("Normalize Genuine");
		textinput->setTextString(ofToString(normalizeValue_genuine));
		textinput = (ofxUITextInput *)gui->getWidget("Normalize Impostor");
		textinput->setTextString(ofToString(normalizeValue_Impostor));
	} else
	{
		normalizeValue_genuine = current_genuine->getMaxCount();
		normalizeValue_Impostor = current_impostor->getMaxCount();

		ofxUITextInput *textinput = (ofxUITextInput *)gui->getWidget("Normalize Genuine");
		textinput->setTextString(ofToString((int)normalizeValue_genuine));
		textinput = (ofxUITextInput *)gui->getWidget("Normalize Impostor");
		textinput->setTextString(ofToString((int)normalizeValue_Impostor));
	}

}

void testApp::calculateROC()
{
	EER = 1;
	ROC_curve.clear();
	for(int i = 0; i < ROC.i; i++)
	{
		ROC_curve.assign(ROC.Specificity[i], ROC.Sensitivity[i]);

		EER_Candidate = abs((ROC.Specificity[i] + ROC.Sensitivity[i]) - 1);
		if(EER > EER_Candidate)
		{
			EER = EER_Candidate;
			EER_bin = i;
		}
	}
	//ROC_curve.resize(0, ROC.Specificity.size());
	ROC_curve.resize(0, 1);
	ROC_curve.setColor(ofColor(255, 255, 0, 100));

	//GUI에 반영
	ofxUILabel* l;
	l = (ofxUILabel*) gui->getWidget("EER_Value");
	l->setLabel(ofToString(EER));
}

bool testApp::loadScoreFiles()
{
	ifstream myfile;
	myfile.open ("data/score.settings");

	if (myfile.is_open())
	{
		while ( !myfile.eof() )
		{
			string str;
			myfile >> str;
			if(str.length() > 0)
				whole_files.push_back(str);
		}
		myfile.close();
	}
	else 
	{
		cout << "Unable to open file"; 
		return false;
	}

	return true;
}

//Bubble sort
void testApp::Bubble_sort(vector<double> &Score, vector<cv::KeyPoint> &referenceKeyPoints, vector<int> &index)
{
	double temp_score;
	int temp_index;
	cv::KeyPoint temp_keypoint;

	for(int i=0;i<Score.size();i++)
	{
		for(int j=0;j<Score.size()-1;j++)
		{
			if(Score[j]>Score[j+1])
			{
				temp_score = Score[j];
				Score[j] = Score[j+1];
				Score[j+1] = temp_score;

				temp_keypoint = referenceKeyPoints[j];
				referenceKeyPoints[j] = referenceKeyPoints[j+1];
				referenceKeyPoints[j+1] = temp_keypoint;

				temp_index = index[j];
				index[j] = index[j+1];
				index[j+1] = temp_index;
			}
		}
	}
}

// Score 파일 읽어서 상위 keypointCount 개수의 키포인트를 이용하여 Impostor / Genuine 분포 계산
void testApp::MakeSortedFiles(int keypointCount, const char * prefix)
{
	FILE *file;
	FILE *file1;
	cv::FileStorage fs;
	cv::FileStorage fss;

	double num;
	vector<double> Score;
	vector<cv::KeyPoint> referenceKeyPoints;
	vector<int> index;

	int res;
	char name[100];

	sprintf(name, "data/%sScore.txt",prefix);
	file = fopen(name,"rt");

	sprintf(name, "data/%sScoresort.txt",prefix);
	file1 = fopen(name,"wt");

	fs.open("data/Data.xml", cv::FileStorage::READ);

	sprintf(name, "data/%sDataSort.xml",prefix);
	fss.open(name, cv::FileStorage::WRITE);

	if(!fs.isOpened())
	{
		std::cout << "FILE NOT FOUND";
		return;
	}

	sprintf(name, "K_%d_%d_%d_%d",1, -10, -1, -1);
	cv::FileNode features = fs[name];
	read(features, referenceKeyPoints);

	// score 파일 읽기
	while(1)
	{
		res = fscanf(file, "%lf\n", &num);
		if(res ==  EOF)
			break;
		Score.push_back(num);
		//fprintf(file1, "%.9lf\n", num);
	}


	for(int i=0;i<Score.size();i++){
		index.push_back(i);
	}

	//sort(Score.rbegin(), Score.rend());		// 내림차순 정렬
	Bubble_sort(Score, referenceKeyPoints, index);

	//Score 정렬 파일 출력
	for(int l= 0; l < Score.size(); l++)
		fprintf(file1, "%.9lf\n", Score[l]);
	fclose(file1);

	//Score에 따른 Keypoint 정렬 파일 출력
	sprintf(name, "D_%d_%d_%d_%d",1,1,1,2);
	fss <<  "referenceKeyPoints" << referenceKeyPoints;


	map<int, int>	selfSimilarity; 
	map<int, int>	seperatibility;

	int count = Score.size() - keypointCount;

	for(int i=Score.size()-1; i >= count; i--){
		//for(int i=0; i < 500; i++){
		//3번 실험 데이터
		sprintf(name,"data/impostor/3_%d",index[i]);
		file = fopen(name,"r");
		while(true){
			int a, b;
			fscanf(file,"%d %d\n",&a,&b);

			seperatibility[a] += b;
			if(feof(file)){
				break;
			}
		}
		fclose(file);

		//2번실험 데이터
		sprintf(name,"data/genuine/2_%d",index[i]);
		file = fopen(name,"r");
		while(true){
			int a, b;
			fscanf(file,"%d %d\n",&a,&b);

			selfSimilarity[a] += b;
			if(feof(file)){
				break;
			}
		}
		fclose(file);
	}

	map<int, int>::iterator Iter_Pos;
	map<int, int>::iterator Iter_Pos1;
	//2번 실험 출력
	sprintf(name, "data/%sresult_genuine.txt",prefix);
	Iter_Pos = seperatibility.end();
	Iter_Pos--;

	file = fopen(name,"w");
	for(int i = 0 ; i<= Iter_Pos->first;i++){
		fprintf(file,"%d %d\n", i, selfSimilarity[i]);
	}
	fclose(file);

	//3번 실험 출력
	sprintf(name, "data/%sresult_imposter.txt",prefix);
	file = fopen(name,"w");
	for(int i = 0 ; i<= Iter_Pos->first;i++){
		fprintf(file,"%d %d\n", i, seperatibility[i]);
	}
	fclose(file);

	//Index 출력
	sprintf(name, "data/%sindex.txt",prefix);
	file = fopen(name,"w");
	for(int i=0; i< referenceKeyPoints.size();i++){
		fprintf(file,"%d\n", index[i]);
	}
	fclose(file);

	// 파일 읽고 UI 갱신
	//--------------------------------------------------------------
	whole_file_index = whole_files.size() - 1;
	ChangeScoreFunction();
}

void testApp::ChangeScoreFunction()
{
	ROC.init();

	char name[100];

	sprintf(name, "data/%sresult_genuine.txt",whole_files[whole_file_index].c_str());
	loadFile(name, &genuine_total, GENUINE);
	genuine_total.setColor(ofColor::green);

	sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index].c_str());
	loadFile(name, &impostor_total, IMPOSTOR);
	impostor_total.setColor(ofColor::red);

	loadSortedIndexList(whole_file_index);

	current_impostor = &impostor_total;
	current_genuine = &genuine_total;

	ROC.update();
	calculateROC();

	// GUI 수정
	ofxUILabel *label = (ofxUILabel *)gui->getWidget("Metric");
	label->setLabel(whole_files[whole_file_index]);

	setNormalizationValues();
}
