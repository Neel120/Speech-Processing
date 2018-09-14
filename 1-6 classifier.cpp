// 1-6 classifier.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <fstream>
#include <string>
#include <queue>
#include <iostream>
using namespace std;
ifstream inFile;
ofstream outFile;
double maxamp = 5000;	//Maximum Normalised Amplitude
int framesize = 1000;	//Frame Size
double level = 1000;	//Level for LCR values

//Getting the next sample
double getnextsample(){
	string line;
	getline(inFile,line);
	//cout<<line<<"\n";
	if(line=="") return 0;
	return stof(line);
}

int normalise(string filename){
	inFile.open("../../" + filename);
	double maxn = 0;
	while(!inFile.eof()){
		//cout<<maxn<<"\n";
		maxn = max(maxn,abs(getnextsample()));
	}
	inFile.close();
	inFile.open("../../" + filename);
	outFile.open("../../N_" + filename);
	while(!inFile.eof()){
		outFile<<getnextsample()*(maxamp/maxn)<<"\n";
	}
	inFile.close();
	outFile.close();
	return 0;
}

long long getLCR(deque<double> frame, double level){
	int crosses=0;
	double prev = frame[0];
	for(int i=1;i<(long long)frame.size();i++){
		if(frame[i]==level) continue;
		if((prev-level)*(frame[i]-level)<0) crosses++;
		prev = frame[i];
	}
	return crosses;
}

pair<double,double> getavgCRs(string filename){
	inFile.open("../../N_" + filename);
	if(!inFile.is_open()){
		cout<<"Error while opening file "<<filename<<"\n Did you normalise it?\n";
		return make_pair(0,0);
	}
	deque<double> frame;
	long long num_frames=0;
	double sumZCR=0;
	double sumLCR=0;
	while(!inFile.eof()){
		for(int x = 0;x<framesize && !inFile.eof();x++){
			if(frame.size()==framesize) frame.pop_front();
			frame.push_back(getnextsample());
		}
		sumZCR+=getLCR(frame,0);
		sumLCR+=getLCR(frame,level);
		num_frames++;
	}
	inFile.close();
	return make_pair(sumZCR/num_frames,sumLCR/num_frames);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int i = 0;
	for(int i=0;i<10;i++){
		//Using the 1s and 6s files and normalising.
		string filename1 = "150101042_1_" + to_string((long long)i) + ".txt";
		string filename6 = "150101042_6_" + to_string((long long)i) + ".txt";
		normalise(filename1);
		normalise(filename6);
	}
	
	double avgZCR_1 = 0, avgLCR_1 = 0;
	cout<<"Calibrating the Model\n\n";
	for(int i=0;i<10;i++){
		//Calibrating the model using the normalised files.
		string filename1 = "150101042_1_" + to_string((long long)i) + ".txt";
		pair<double,double> CRs = getavgCRs(filename1);
		cout<<filename1<<" Avg ZCR : "<<CRs.first<<" Avg LCR : "<<CRs.second<<"\n";
		avgZCR_1 += CRs.first/10;
		avgLCR_1 += CRs.second/10;
	}
	cout<<" Avg ZCR : "<<avgZCR_1<<" Avg LCR : "<<avgLCR_1<<"\n";
	double avgZCR_6 = 0, avgLCR_6 = 0;
	for(int i=0;i<10;i++){
		string filename1 = "150101042_6_" + to_string((long long)i) + ".txt";
		pair<double,double> CRs = getavgCRs(filename1);
		cout<<filename1<<" Avg ZCR : "<<CRs.first<<" Avg LCR : "<<CRs.second<<"\n";
		avgZCR_6 += CRs.first/10;
		avgLCR_6 += CRs.second/10;
	}
	cout<<" Avg ZCR : "<<avgZCR_6<<" Avg LCR : "<<avgLCR_6<<"\n";
	cout<<"Say Something now!";
	system("Recording_Module.exe 1 ../../test.wav ../../test.txt");
	cout<<"\n\nEvaluating the test file\n\n";
	//Evaluating the model on the test file
	string testfile = "test.txt";
	normalise(testfile);
	pair<double,double> CRs = getavgCRs(testfile);
	cout<<testfile<<" Avg ZCR : "<<CRs.first<<" Avg LCR : "<<CRs.second<<"\n";

	double conf_1 = pow(2,-1*abs(CRs.first - avgZCR_1)/25)*pow(2,-1*abs(CRs.second - avgLCR_1)/25);
	double conf_6 = pow(2,-1*abs(CRs.first - avgZCR_6)/25)*pow(2,-1*abs(CRs.second - avgLCR_6)/25);

	cout<<"Confidence for 1 : "<<conf_1<<" Confidence for 6 : "<<conf_6<<"\n";
	//Deciding whether its 1 or 6 or not sure.
	if((conf_1 > 0.5 && conf_6 > 0.5) || (conf_1 < 0.5 && conf_6 < 0.5)) cout<<"Not sure!\n";
	else cout<<(conf_1>conf_6?"Detected 1\n":"Detected 6\n");
	return 0;
}

