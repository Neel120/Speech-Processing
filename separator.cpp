// separator.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <fstream>
#include <string>
#include <queue>
#include <iostream>
using namespace std;
ifstream inFile;
ofstream outFile;
double dcshift=0;
//Skipping the first 5 lines
int skipbeginning(){
	string line;
	for(int i=0;i<5;i++){
		getline(inFile,line);
		cout<<line<<"\n";
	}
	return 0;
}

//Getting the next sample
int getnextsample(){
	string line;
	getline(inFile,line);
	//cout<<line<<"\n";
	if(line=="") return 0;
	return stoi(line);
}


int _tmain(int argc, _TCHAR* argv[])
{
	inFile.open("../../Muffled Mic.txt");
	skipbeginning();
	long long numsamples = 0;
	while(!inFile.eof()){
		numsamples++;
		dcshift+=getnextsample();
	}
	inFile.close();
	if(numsamples) dcshift = dcshift/numsamples;
	queue<int> samples;
	long long int sum=0;
	for(int i=0;i<10;i++){
		string filename = "../../150101042_";
		filename += to_string((long long)i);
		filename += ".txt";
		inFile.open(filename);
		skipbeginning();
		int flag=0;
		int k=0;
		for(int i=0;i<5000;i++) getnextsample();
		int threshold = 100;
		int framesize = 3000;
		while(!inFile.eof()){
			if(samples.size()==framesize){
				if((sum/framesize)>threshold){
					outFile.open("150101042_" + to_string((long long)k) + "_" + to_string((long long)i) + ".txt");
					while((sum/framesize)>threshold/5 && !inFile.eof()){
						outFile<<samples.front()-dcshift<<"\n";
						sum-=abs(samples.front());
						samples.pop();
						samples.push(getnextsample());
						sum+=abs(samples.back());
					}
					outFile.close();
					k++;
				}
				else{
					sum-=abs(samples.front());
					samples.pop();
					samples.push(getnextsample());
					sum+=abs(samples.back());
				}
			}
			else{
				samples.push(getnextsample());
				sum+=abs(samples.back());
			}
		}
		inFile.close();
	}
	return 0;
}

