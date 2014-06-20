#ifndef OFXGAUSSIANHISTOGRAM
#define OFXGAUSSIANHISTOGRAM

#include <iostream>

using namespace std;

#include "ofxHistogram.h"

class ofxGaussianHistogram : public ofxHistogram
{
public:
	float mean;
	float variance;
	float std_dev;


	//TODO; 이건 수정 안했음!
	void drawGaussian(int x, int y, int width, float height)
	{
		int max = 0;
		int min = 0;
		getMaxMinCount(&max, &min);
		calculateMeanAndVariance();

		int i = 0;

		// data의 key의 최대값
		int maxBinSize = data.size();

		// 색상 채우기
		if(data.size() != bin_color.size())
		{
			bin_color.resize(data.size());

			for(int i=0; i<bin_color.size(); ++i)
			{
				bin_color[i] = ofColor( ofRandom(255), ofRandom(255), ofRandom(255), ofRandom(255) );
			}
		}


		ofPushMatrix();
		ofPushStyle();

		ofTranslate(x, height + y);
		ofScale(width, -height, 1);

		for(iterator = data.begin() ; iterator != data.end(); ++iterator, ++i)
		{
			ofSetColor(bin_color[i]);
			ofRect(i * 1.0 / maxBinSize, 0, 1.0 / maxBinSize, (float)(iterator->second) / (float)max);
		}			

		ofPopStyle();
		ofPopMatrix();
	}



	// 주어진 max_count 크기로 정규화하여 출력
	//--------------------------------------------------------------
	void drawGaussian(int x, int y, int width, int height, float max_probability)
	{
		calculateMeanAndVariance();

		// data의 key의 최대값
		int maxBinSize = data.size();

		// 색상 채우기
		if(data.size() != bin_color.size())
		{
			bin_color.resize(data.size());

			for(int i=0; i<bin_color.size(); ++i)
			{
				 bin_color[i] = ofColor( ofRandom(255), ofRandom(255), ofRandom(255), ofRandom(255) );
			}
		}


		ofPushMatrix();
		ofPushStyle();

		ofTranslate(x, height + y);
		ofScale(width, -((float)(height ) / max_probability), 1);

		ofSetColor(bin_color[0]);
		//ofSetLineWidth(0.1);
		int xx = 0;
		float yy = 1.0 / (std_dev * sqrt( 2 * PI )) * exp( -(xx-mean)*(xx-mean) / (2*std_dev*std_dev) );
		ofPoint p0(xx, yy);
		ofPoint p1;
		for(int xx=1; xx<1000; ++xx)
		{
			float yy = 1.0 / (std_dev * sqrt( 2 * PI )) * exp( -(xx-mean)*(xx-mean) / (2*std_dev*std_dev) );
			p1 = ofPoint(xx/1000.0, yy);
			ofLine(p0, p1);
			p0 = p1;
		}

		ofPopStyle();
		ofPopMatrix();
	}

	// cout으로 객체 출력
	//--------------------------------------------------------------
	void print()
	{
		cout << "mean: " << mean << "\tstdev: " << std_dev << endl;
	}

private:


	// 평균 분산 계산
	//--------------------------------------------------------------
	void calculateMeanAndVariance()
	{
		// 평균 계산
		//--------------------------------------------------------------
		float sum = 0;
		int total_count = 0; 
		for(iterator = data.begin(); iterator != data.end() ; ++iterator)
		{
			sum += iterator->first * iterator->second;
			total_count += iterator->second;
		}

		mean = sum / total_count;

		// 분산 계산
		//--------------------------------------------------------------
		sum = 0;
		for(iterator = data.begin(); iterator != data.end() ; ++iterator)
			sum += (iterator->first - mean) * (iterator->first - mean) * iterator->second;
		
		variance = sum / total_count;
		std_dev = sqrt(variance);
	}
};

#endif
