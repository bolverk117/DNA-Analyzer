#include "stdafx.h"
#include <highgui.h>
#include <cv.h>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <iterator>
#include "Header1.h"

using namespace std;
using namespace cv;

int main()
{
	int entry, sizeRows=0, sizeCols=0;
	Mat originalImg;
	Mat grayScaleImg;
	cv::Scalar pixelValue;
	cury = 0, curx = 0;

	cout << "Welcome to the Protein Gell Analysis App." << endl << "Please enter whether you would like to either upload an image by typing 1, or take a photo by typing 2.";
	cin >> entry;
	if (entry == 1)
	{
		//Read image ( same size, same type )
		String imagename;
		cout << "Please enter the name of the image.";
		cin >> imagename;
		originalImg = imread("../CamTest/Images/" + imagename);
		if (!originalImg.data)
		{
			cout << " Error loading image \n";
			return -1;
		}

	}
	else if (entry == 2)
	{
		VideoCapture cap(0);
		if (!cap.isOpened()) // check if we succeeded
			return -1;
		cap >> originalImg;
	}
	// Convert to graysacale
	cvtColor(originalImg, grayScaleImg, CV_BGR2GRAY);
	
	//Create Windows
	namedWindow("Gell", CV_WINDOW_AUTOSIZE);
	imshow("Gell", grayScaleImg);

	// .cols and .rows are opencv functions. 
	//sizeCols and sizeRows hold the value of how many columns and rows there are in grayScaleImg 
	sizeCols = grayScaleImg.cols;
	sizeRows = grayScaleImg.rows;

	//vector<Scalar> columnAverage;
	//Double pointer that dynamically sets the size of the array colAves by initializing sizeCols
	double *colAves;
	colAves = (double *)malloc(sizeof(double)*sizeCols);

	//Put values in to globalPD from zenBox.h
	globalPD.pixelArray = colAves;
	globalPD.grayImg = grayScaleImg;

	

	//Calles the mouse function from Header1.h
	cvSetMouseCallback("Gell", mouseHandler, NULL);
	cout << " passing the mouse " << endl;



	waitChecker();

	//---------------------------Testing Output Below this Line---------------------
	// This section is for testing and trouble shooting only
		
	//process pixelArrey
	//reset to zero 

	////Test output to see if globalPD.pixelArray is working
	//for(int i = 0; i < sizeCols; i++)
	//{
	//	cout << i << ": " << globalPD.pixelArray[i] << endl;
	//}
	// while goblePD.x == 0 wait

	// Test output
	cout << "this "  << globalPD.x << " is the value of x " << endl;

	//Iderates equal to the number of columns in grayScaleImg
	//Sets pixelValue to the mean of all the pixel in each column
	//.val return the double from pixelValue
	//[0] limites the returns to just the first field the other fields values are zero in gray scale images 
	/*for (int i = 0; i < sizeCols; i++)
	{
		pixelValue = mean(grayScaleImg.col(i));
		colAves[i] = pixelValue.val[0];
	}*/

	/*
	int key = waitKey(0) & 0xFF;

	if (102 == key)
		cout << "F Was pressed" << endl;
		*/
	//Test output that shows the column number next to each column mean
	/*for (int i = 0; i < sizeCols; i++)
	{
		cout << i << ": " << colAves[i] << endl;
	}*/
	////Starting at the fourth column and then increaces buy one
	////absolute value of the difference between every fourth column
	//for (int i = 4; i < sizeCols; i++)
	//{
	//	if (abs(colAves[i]-colAves[i-4]) > 10)
	//	{
	//		//add to edge array
	//		cout << "Lane edge at " << i - 2 << endl;
	//	}
	//	
	//}
	//
	//// Testing output that shows how many columns and rows are in the image
	//cout << "Image size (cols, rows): " << sizeCols << " " << sizeRows << endl;
	
	waitKey(0);
	return 0;
} 