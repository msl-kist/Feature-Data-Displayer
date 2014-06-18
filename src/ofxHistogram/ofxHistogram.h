
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
	
	// �־��� ���� ������׷��� �߰�
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

	// �־��� ������ ������׷� �Ҵ�
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

	// cout���� ��ü ���
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

		// data�� key�� �ִ밪
		int maxBinSize = data.size();

		// ���� ä���
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
	// �־��� max_count ũ��� ����ȭ�Ͽ� ���
	//--------------------------------------------------------------
	void draw(int x, int y, int width, int height, int max_count)
	{
		int max = 0;
		int min = 0;
		getMaxMinCount(&max, &min);
		int i = 0;

		// data�� key�� �ִ밪
		int maxBinSize = data.size();

		// ���� ä���
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

	// �׷����� ������ �Է¹޾� ����
	//--------------------------------------------------------------
	void setColor(ofColor color)
	{
		if(data.size() != bin_color.size())
			bin_color.resize(data.size());
		
		for(int i=0; i<bin_color.size(); ++i)
			bin_color[i] = color;
	}

	// �ִ�/�ּ� ���� ��ȯ
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

	// �߰��� ��(�Ǵ� ������) bin�� �߰���
	//--------------------------------------------------------------
	void normalize()
	{
		int prev_key;

		// iterator 2��° ���� ����
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

	// �ִ�/�ּҰ� ���� ����
	//--------------------------------------------------------------
	void resize(int min, int max)
	{
		this->min = min;
		this->max = max;

		// Max �� �ʰ� ��� ����
		for(iterator = data.begin(); iterator != data.end(); ++iterator)
		{
			if(iterator->first > max)
			{
				map<int, int>::iterator toRemove = iterator;
				iterator--;

				data.erase(toRemove);
			}
		}

		// Min �� �̸� ��� ����
		map<int, int>::reverse_iterator riterator;

		for(riterator = data.rbegin(); riterator != data.rend(); ++riterator)
		{
			//TODO: min �� �̸� ��� ����!	
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

	//  �ʱ�ȭ �Լ�
	//--------------------------------------------------------------
	void init(int min, int max, int width)
	{
		this->min = min;
		this->max = max;
		this->width = width;

		// map�� �ʱ�ȭ
		if(max != -1)
		{
			for(int i=min; i<=max; i+=width)
			{
				data[i] = 0;
			}
		}
	}

	// �־��� ǥ���� �� ������ bin�� ����
	//--------------------------------------------------------------
	int getRepresentativeValue(int x)
	{
		int v = x - min;
		return (v / width)*width + min;
	}




};

#endif