#include "testApp.h"

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
	ROC.init();

	sprintf(name, "data/%sresult_genuine.txt",whole_files[whole_file_index]);
	loadFile(name, &genuine_total, GENUINE);
	genuine_total.setColor(ofColor(200, 50, 50, 100));

	sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index]);
	loadFile(name, &impostor_total, IMPOSTOR);
	impostor_total.setColor(ofColor(50, 200, 50, 0));

	
	ROC.update();

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
	ROC_curve.resize(0, ROC.Specificity.size());
	ROC_curve.setColor(ofColor(255, 255, 0, 100));


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
		genuine.setColor(ofColor(200, 50, 50, 100));

		sprintf(name, "data/impostor/3_%d", selectedKeypointIndex);
		loadFile(name, &impostor, IMPOSTOR);
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

	//ofNoFill();
	//ofSetColor(ofColor::red);
	//ofCircle(ofPoint(keypoints[839].pt.x, keypoints[839].pt.y),10);
	ofPopStyle();

	// 배경 사각형 출력
	ofPushStyle();
	ofSetColor(ofColor::darkGrey);
	ofRect(image.width, 0, 640, 480);
	ofPopStyle();

	if(selectedKeypointIndex == NOT_KEYPOINT)
	{ 
		int max_g, max_i;
		genuine_total.getMaxMinCount(&max_g);
		impostor_total.getMaxMinCount(&max_i);
		int max = max_g > max_i ? max_g : max_i;
		
		genuine_total.draw(image.width, 0, 640, 480, max_g);
		impostor_total.draw(image.width,0, 640, 480, max_i);
		
		ROC_curve.draw(image.width, 500, 500, 480, EER_bin);

	} else {
		int max_g, max_i;
		genuine.getMaxMinCount(&max_g);
		impostor.getMaxMinCount(&max_i);
		int max = max_g > max_i ? max_g : max_i;

		genuine.draw(image.width, 0, 640, 480, max_g);
		impostor.draw(image.width,0, 640, 480, max_i);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	ROC.init();
	if(key == ' ')
	{
		char name[100];

		whole_file_index = (whole_file_index + 1) % 3;

		sprintf(name, "data/%sresult_genuine.txt",whole_files[whole_file_index]);
		loadFile(name, &genuine_total, GENUINE);
		genuine_total.setColor(ofColor(200, 50, 50, 100));

		sprintf(name, "data/%sresult_imposter.txt",whole_files[whole_file_index]);
		loadFile(name, &impostor_total, IMPOSTOR);
		impostor_total.setColor(ofColor(50, 200, 50, 0));
	}

	//ROC curve
	ROC.update();

	ROC_curve.clear();
	EER = 1;
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
	ROC_curve.resize(0, ROC.Specificity.size());
	ROC_curve.setColor(ofColor(255, 255, 0, 100));
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
	{
		selectedKeypointIndex = NOT_KEYPOINT;


		EER = 1;

		//ROC curve
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
		ROC_curve.resize(0, ROC.Specificity.size());
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
