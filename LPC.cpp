// ConsoleApplication8.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <fstream>
#include <string>
#include <queue>
#include <iostream>
#include <math.h>

#define M_PI 3.14159265358979323846
#define log2(a) log((double)a)/log((double)2)
using namespace std;
ifstream inFile;
ofstream outFile;
string vowels[] = {"_A_","_E_","_I_","_O_","_U_"};
double maxamp = 5000;	//Maximum Normalised Amplitude
int framesize = 1000;	//Frame Size
int w[]=[1,3,7,13,19,22,25,33,42,50,56,61]

//Skipping the first 5 lines
int skipbeginning(){
	string line;
	for(int i=0;i<5;i++){
		getline(inFile,line);
		//cout<<line<<"\n";
	}
	return 0;
}

//Getting the next sample
double getnextsample(){
	string line;
	getline(inFile,line);
	//cout<<line<<"\n";
	if(line=="") return 0;
	return stof(line);
}

vector<double> normalise(string filename){
	inFile.open("../../" + filename);
	skipbeginning();
	double maxn = 0;
	while(!inFile.eof()){
		//cout<<maxn<<"\n";
		maxn = max(maxn,abs(getnextsample()));
	}
	inFile.close();
	inFile.open("../../" + filename);
	skipbeginning();
	vector<double> data;
	while(!inFile.eof()) data.push_back(getnextsample()*(maxamp/maxn));
	inFile.close();
	return data;
}

double abs_max(double a, double b)//return the value of absolute max of given parameters;
{
	if (abs(a) > abs(b))return abs(a);
	return abs(b);
}

vector<double> calcRs(vector<double>signal, long long int p)//gets the Ris from the signals
{
	vector<double> R;
	for (long long int i = 0; i <= p; i++)
	{
		double value = 0;
		for (long long int j = 0; j < signal.size() - i; j++)
		{
			//if((signal[j+i] < 0 && signal[j] < 0) || (signal[j+i]>0 && signal[j]>0)){
			//	if(signal[j+i] > DBL_MAX/signal[j] || signal[j] > DBL_MAX/signal[j+i] || DBL_MAX - signal[j] *signal[j + i] < value)
			//	{
			//		cout<<DBL_MAX/signal[j]<<" "<<DBL_MAX/signal[j+i]<<" "<<DBL_MAX - signal[j]*signal[j+i]<<"\n";
			//		cout<<"Overflow occured!\n";	
			//	}
			//}
			//else{
			//	if(signal[j+i] < DBL_MIN/signal[j] || signal[j] < DBL_MIN/signal[j+i] || DBL_MIN + signal[j]*signal[j + i] > value)
			//		cout<<"Overflow occured!\n";	
			//}
			value += signal[j] *signal[j + i];
		}
		R.push_back(value);
	}
	/*
	for(long long int i = 1; i<=p ; i++){
		R[i] = R[i]/R[0];
	}
	R[0]=1;*/
	return R;
}
vector<double> calcAs(vector<double> R){
	double E = R[0];
	vector<double> a;
	a.push_back(-1);
	for(int i=1; i<R.size(); i++){
		a.push_back(0);
		double x = 0;
		for(int j = 0; j< i; j++){
			x += a[j] * R[i-j];
		}
		double lambda = x/E;
		vector<double> atmp;
		for(int j = 0; j<a.size(); j++)	atmp.push_back(a[j] + lambda*a[a.size()-1-j]);
		swap(a,atmp);
		E = (1 - lambda*lambda)*E;
	}
	return a;
}

vector<double> calcCs(vector<double> R, vector<double> A){
	double g = R[0];
	for(int i = 1; i<R.size();i++) g-=A[i]*R[i];
	vector<double> C;
	C.push_back(log2(g));
	for(int i = 1; i<A.size(); i++){
		C.push_back(A[i]);
		for(int k = 1; k<i;k++) C.back() += (k/(double)i)*C[k]*A[i-k];
	}
	return C;
}

//Voice Activity Detection
int getvoiced(vector<double> &data)
{
	double pos = 0, neg = 0, cpos = 0, cneg = 0, noisesamples = 1000, threshold;
	for (int i = 0; i < data.size(); i++)
	{
		if (data[i] < 0 && (i <= noisesamples || i >= (data.size() - noisesamples)))
		{
			pos += data[i];
			cpos++;
		}
		else if (data[i]>0 && (i <= noisesamples || i >= (data.size() - noisesamples)))
		{
			neg += data[i];
			cneg++;
		}
	}
	pos = pos / cpos;//average ambient sound for positive and negative sides
	neg = neg / cneg;

	threshold = 2 * max(abs(pos), abs(neg));//defining threshold with the help of average ambient sound for positive and negative sides
	while(data.size() && abs(data.back())<threshold) data.pop_back();
	reverse(data.begin(),data.end());
	while(data.size() && abs(data.back())<threshold) data.pop_back();
	reverse(data.begin(),data.end());
	return 0;
}

int frames = 5;
int _tmain(int argc, _TCHAR* argv[])
{
	//Normalising the files
	for(int i=0;i<5;i++)
		for(int j=0;j<5;j++)
		{
			vector<double> data = normalise("150101042" + vowels[j] + to_string((long long)i) + ".txt");
			/*inFile.open("../../trimmed.txt");
			while(!inFile.eof()) data.push_back(getnextsample());
			inFile.close();*/
			getvoiced(data);
			for(int k = 0; k<frames;k++){
				vector<double> frame;
				for(int i=80*k;i<320 + 80*k;i++) frame.push_back(data[i]);
				for(int i=0;i<frame.size();i++){
					frame[i] = frame[i] * (0.54 - 0.46* cos(2*M_PI*i/(frame.size())));
				}
				vector<double> R = calcRs(frame,12);
				cout<<"Rs : \n";
				for(int k = 0; k<R.size(); k++) cout<<R[k]<<"\n";
				vector<double> A = calcAs(R);
				cout<<"As : \n";
				for(int k = 0; k<A.size(); k++) cout<<A[k]<<"\n";
				vector<double> C = calcCs(R,A);
				cout<<"Cs : \n";
				for(int k = 0; k<C.size(); k++) cout<<C[k]<<"\n";
				cout<<"\n";
			}
		}
	return 0;
}

