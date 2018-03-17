#pragma once
#include <cv.h>
#include <stdio.h>

using namespace std;
using namespace cv;

// A data type that has an x and y value for the mouse clicker
// Has a double for pixel values, and a matrix for an image
struct pictureData
{
	int x, y;
	double *pixelArray;
	Mat grayImg;
};

//sets a varable to contain all the data from pictureData
pictureData globalPD;

void placeHolder()
{
	//use this function to pull the from pixelArray
	//based on the column from the x value
}

