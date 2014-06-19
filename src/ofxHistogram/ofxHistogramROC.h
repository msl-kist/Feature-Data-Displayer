
#ifndef OFXHISTOGRAMROC
#define OFXHISTOGRAMROC

#include <map>
#include <iostream>		// for ostream ( @operator<<())

using namespace std;

class ofxHistogramROC
{
public:
	ofxHistogramROC() { init(0, 1000, 1); }
	ofxHistogramROC(double min, double width) { init(min, -1, width);	}
	ofxHistogramROC(double min, double max, double width) { init(min, max, width); }
	
	// �־��� ���� ������׷��� �߰�
	//--------------------------------------------------------------
	void  add(double x)
	{
		double repValue = getRepresentativeValue(x);
		iterator = data.find(repValue);
		if(iterator == data.end())
			data[repValue] = 1;
		else
			iterator->second = iterator->second+1;
	}

	// �־��� ������ ������׷� �Ҵ�
	//--------------------------------------------------------------
	double assign(double key, double value)
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
	friend ostream& operator<<(ostream& os, const ofxHistogramROC& hist)
	{
		os << "\tKEY\tCOUNT\n";
		os << "================================\n";

		for( map<double, double>::const_iterator iterator = hist.data.begin() ; iterator != hist.data.end(); ++iterator)
			os << "\t   " << iterator->first << "\t    " << iterator->second << "\n";

		return os;
	}

	void draw(double x, double y, double width, float height, int EER_bin)
	{
		double max = 0;
		double min = 0;
		getMaxMinCount(&max, &min);
		double i = 0;

		// data�� key�� �ִ밪
		double maxBinSize = data.size();

		// ���� ä���
		if(data.size() != bin_color.size())
		{
			bin_color.resize(data.size());

			for(double i=0; i<bin_color.size(); i++)
			{
				bin_color[i] = ofColor( ofRandom(255), ofRandom(255), ofRandom(255), ofRandom(255) );
			}
		}


		ofPushMatrix();
		ofPushStyle();

			ofTranslate(x, height + y);
			ofScale(width, -height, 1);
		
			for(iterator = data.begin() ; iterator != data.end(); ++iterator, i++)
			{
				ofSetColor(bin_color[i]);
				ofRect(i * 1.0 / maxBinSize, 0, 1.0 / maxBinSize, (float)(iterator->second) / (float)max);

				ofSetColor(ofColor::red);
				if(i == EER_bin)
					ofCircle(ofPoint(i * 1.0 / maxBinSize , (float)(iterator->second) / (float)max ), 0.01);
			}			
			
		ofPopStyle();
		ofPopMatrix();
	}

	

	// �־��� max_count ũ��� ����ȭ�Ͽ� ���
	//--------------------------------------------------------------
	void draw(double x, double y, double width, double height, double max_count)
	{
		double max = 0;
		double min = 0;
		getMaxMinCount(&max, &min);
		double i = 0;

		// data�� key�� �ִ밪
		double maxBinSize = data.size();

		// ���� ä���
		if(data.size() != bin_color.size())
		{
			bin_color.resize(data.size());

			for(double i=0; i<bin_color.size(); i++)
			{
				bin_color[i] = ofColor( ofRandom(255), ofRandom(255), ofRandom(255), ofRandom(255) );
			}
		}

		ofPushMatrix();
		ofPushStyle();

		ofTranslate(x, height + y);
		ofScale(width, -((float)(height * max) / max_count), 1);

		for(iterator = data.begin() ; iterator != data.end(); ++iterator, i++)
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
		
		for(double i=0; i<bin_color.size(); i++)
			bin_color[i] = color;
	}

	// �ִ�/�ּ� ���� ��ȯ
	//--------------------------------------------------------------
	
	void getMaxMinCount(double * max, double * min = NULL)
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
		double prev_key;

		// iterator 2��° ���� ����
		iterator = data.begin();
		prev_key = iterator->first;
		iterator++;

		for(iterator = iterator ; iterator != data.end(); ++iterator)
		{
			double this_key = iterator->first;
			for( double i=prev_key+width; i < this_key; i += width)
				data[i] = 0;
			prev_key = this_key;
		}
	}

	// �ִ�/�ּҰ� ���� ����
	//--------------------------------------------------------------
	void resize(double min, double max)
	{
		this->min = min;
		this->max = max;

		// Max �� �ʰ� ��� ����
		for(iterator = data.begin(); iterator != data.end(); ++iterator)
		{
			if(iterator->first > max)
			{
				map<double, double>::iterator toRemove = iterator;
				iterator--;

				data.erase(toRemove);
			}
		}

		// Min �� �̸� ��� ����
		map<double, double>::reverse_iterator riterator;

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
	double min;
	double max;
	double width;

	// Histogram Data
	map<double, double>	data;
	map<double, double>::iterator	iterator;

	//  �ʱ�ȭ �Լ�
	//--------------------------------------------------------------
	void init(double min, double max, double width)
	{
		this->min = min;
		this->max = max;
		this->width = width;

		// map�� �ʱ�ȭ
		if(max != -1)
		{
			for(double i=min; i<=max; i+=width)
			{
				data[i] = 0;
			}
		}
	}

	// �־��� ǥ���� �� ������ bin�� ����
	//--------------------------------------------------------------
	double getRepresentativeValue(double x)
	{
		double v = x - min;
		return (v / width)*width + min;
	}
};

#endif