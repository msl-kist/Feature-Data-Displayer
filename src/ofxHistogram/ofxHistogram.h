
#ifndef OFXHISTOGRAM
#define OFXHISTOGRAM

#include <map>
#include <iostream>		// for ostream ( @operator<<())

using namespace std;

class ofxHistogram
{
public:
	ofxHistogram() { init(0, 1000, 1); }
	ofxHistogram(int min, int width) { init(min, -1, width);	}
	ofxHistogram(int min, int max, int width) { init(min, max, width); }
	
	// 주어진 값을 히스토그램에 추가
	//--------------------------------------------------------------
	void  add(int x)
	{
		int repValue = getRepresentativeValue(x);
		iterator = data.find(repValue);
		if(iterator == data.end())
			data[repValue] = 1;
		else
			iterator->second = iterator->second+1;
	}

	// 주어진 값으로 히스토그램 할당
	//--------------------------------------------------------------
	int assign(int key, int value)
	{
		iterator = data.find(key);
		if(iterator == data.end())
			data[key] = value;
		else
			iterator->second = value;
		return value;
	}

	// cout으로 객체 출력
	//--------------------------------------------------------------
	friend ostream& operator<<(ostream& os, const ofxHistogram& hist)
	{
		os << "\tKEY\tCOUNT\n";
		os << "================================\n";

		for( map<int, int>::const_iterator iterator = hist.data.begin() ; iterator != hist.data.end(); ++iterator)
			os << "\t   " << iterator->first << "\t    " << iterator->second << "\n";

		return os;
	}

	void draw(int x, int y, int width, float height)
	{
		int max = 0;
		int min = 0;
		getMaxMinCount(&max, &min);
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
	void draw(int x, int y, int width, int height, int max_count)
	{
		int max = 0;
		int min = 0;
		getMaxMinCount(&max, &min);
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
		ofScale(width, -((float)(height * max) / max_count), 1);

		for(iterator = data.begin() ; iterator != data.end(); ++iterator, ++i)
		{
			ofSetColor(bin_color[i]);
			ofRect(i * 1.0 / maxBinSize, 0, 1.0 / maxBinSize, 
				(float)(iterator->second) / (float)max > (float)max_count / max ? (float)max_count / max : (float)(iterator->second) / (float)max );
		}			

		ofPopStyle();
		ofPopMatrix();
	}
	vector<ofColor>	bin_color;

	// 그래프의 색상을 입력받아 저장
	//--------------------------------------------------------------
	void setColor(ofColor color)
	{
		if(data.size() != bin_color.size())
			bin_color.resize(data.size());
		
		for(int i=0; i<bin_color.size(); ++i)
			bin_color[i] = color;
	}

	// 최대/최소 개수 반환
	//--------------------------------------------------------------
	void getMaxMinCount(int * max, int * min = NULL)
	{
		*max = -1;
		if(min != NULL)
			*min = 10000000000;

		for(iterator = data.begin() ; iterator != data.end(); ++iterator)
		{
			if(iterator->second > *max)
				*max = iterator->second;
			if(min != NULL && iterator->second < *min)
				*min = iterator->second;
		}
	}

	// 중간에 빈(또는 생략된) bin을 추가함
	//--------------------------------------------------------------
	void normalize()
	{
		int prev_key;

		// iterator 2번째 노드로 세팅
		iterator = data.begin();
		prev_key = iterator->first;
		iterator++;

		for(iterator = iterator ; iterator != data.end(); ++iterator)
		{
			int this_key = iterator->first;
			for( int i=prev_key+width; i < this_key; i += width)
				data[i] = 0;
			prev_key = this_key;
		}
	}

	// 최대/최소값 범위 수정
	//--------------------------------------------------------------
	void resize(int min, int max)
	{
		this->min = min;
		this->max = max;

		// Max 값 초과 노드 제거
		for(iterator = data.begin(); iterator != data.end(); ++iterator)
		{
			if(iterator->first > max)
			{
				map<int, int>::iterator toRemove = iterator;
				iterator--;

				data.erase(toRemove);
			}
		}

		// Min 값 미만 노드 제거
		map<int, int>::reverse_iterator riterator;

		for(riterator = data.rbegin(); riterator != data.rend(); ++riterator)
		{
			//TODO: min 값 미만 노드 제거!	
		}
	}

	void clear()
	{
		data.clear();
	}

protected:
	// Histogram Parameters
	int min;
	int max;
	int width;

	// Histogram Data
	map<int, int>	data;
	map<int, int>::iterator	iterator;

	//  초기화 함수
	//--------------------------------------------------------------
	void init(int min, int max, int width)
	{
		this->min = min;
		this->max = max;
		this->width = width;

		// map형 초기화
		if(max != -1)
		{
			for(int i=min; i<=max; i+=width)
			{
				data[i] = 0;
			}
		}
	}

	// 주어진 표본이 들어갈 적절한 bin을 선택
	//--------------------------------------------------------------
	int getRepresentativeValue(int x)
	{
		int v = x - min;
		return (v / width)*width + min;
	}




};

#endif