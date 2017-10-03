#pragma once
#ifndef MODEL2_H
#define MODEL2_H
#include<stdlib.h>
#include<fstream>
#include<iostream>
using namespace std;
const int n = 100000;
class Model2
{
private:
	
	int mod = 1;
	int num1 = 0;
	int num2 = 0;
	int num3 = 0;
	int numvt1 = 0;
	int numvt2 = 0;
	int numvn1 = 0;
	int numvn2 = 0;
	int numvn3 = 0;
	float tab1[n], tab2[n], tab3[n], tabvt1[n], tabvt2[n], tabvn1[n], tabvn22[n], tabvn33[n];
	int fv[n], ft[n], fn[n];
	int nfv = 0;
	int nft = 0;
	int nfn = 0;
public: 
	float *Vertices, *Textures, *NormalVertices;
	int numVertices, numTextures, normalNumVertices;
	bool loadOBJ(const char * path, float *&vert, float *&tex, float *&normalrvert, int *numver, int *numtex, int *numnormv);
};

#endif

