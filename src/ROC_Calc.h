#include <stdio.h>

#include "ofMain.h"
#include "ofxOpenCv.h"

class ROC_Calc{
private: 
	int res;
	int genuineT, genuineD;
	int imposterT, imposterD;
	double TPSum, FNSum, FPSum, TNSum;
	cv::vector<int> TP, FN, FP, TN;

public :	
	cv::vector<std::pair<int, int>> Genuine, Imposter;
	cv::vector<double> Sensitivity, Specificity;
	int i, j;

	void init()
	{
		TPSum = FNSum = FPSum = TNSum = 0;

		Genuine.clear();
		Imposter.clear();
		Sensitivity.clear();
		Specificity.clear();
	}

	void update()
	{
		std::vector<std::pair<int, int>>::iterator it;
		for(i = 0, it = Genuine.begin(); it != Genuine.end(); ++i, it = Genuine.begin()+i)
		{
			TPSum += Genuine[i].second;
			FPSum += Imposter[i].second;

			for(j = i+1, it++ ; it != Genuine.end();it++, ++j)
			{
				FNSum += Genuine[j].second;
				TNSum += Imposter[j].second;
			}

			TP.push_back(TPSum);
			FP.push_back(FPSum);
			FN.push_back(FNSum);
			TN.push_back(TNSum);

			Specificity.push_back(1-(double)(TNSum / (FPSum + TNSum)));
			Sensitivity.push_back((double)(TPSum / (TPSum + FNSum))); 

			FNSum = TNSum = 0;

			//fprintf(resultFile, "%.9lf\t%.9lf\n", Specificity[i], Sensitivity[i]);
		}
	}
};