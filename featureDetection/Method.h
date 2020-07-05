#pragma once
#include "opencv2\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
using namespace cv;

const float PI = 3.141592654;
const vector<float> sobelX = { 1,0,-1,2,0,-2,1,0,-1 };
const vector<float> sobelY = { 1,2,1,0,0,0,-1,-2,-1 };

const float k = 0.04;

void castUchar(Mat& img);
vector<float> createGaussKernel(int height, int width, float sigma);
void convolutionKernel3(Mat& src, Mat& dst, vector<float> kernel);
vector<float> solveEquation2(int a, int b, int c);
vector<float> calcEigenValue(vector<int> arr);
void sobelGradient(Mat& src, Mat& dst, int xAxis, int yAxis);
bool isMax(Mat img, int x, int y, int sizeWindow);
void detectHarris(Mat& src, Mat& dst, int threshHold);

vector<float> createNormLoG(int height, int width, float sigma);
int findMax(Mat img, int x, int y, int sizeWindow);
bool isMaxIn3Img(vector<Mat> img, int i, int x, int y, int sizeWindow);
void detectBlob(Mat& src, Mat& dst, int numOfConvolution);

void detectDOG(Mat& src);
void matchBySIFT(Mat& img1, Mat& img2, int detector);