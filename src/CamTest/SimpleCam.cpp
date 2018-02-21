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
	//vector<Scalar> columnAverage;
	//Double pointer that dynamically sets the size of the array colAves by initializing sizeCols
	double *colAves;
	colAves = (double *)malloc(sizeof(double)*sizeCols);

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
	
	// ****need to replace mean with somthing that finds the highest value in a column****
	//Iderates equal to the number of columns in grayScaleImg
	//Sets pixelValue to the mean of all the pixel in each column
	//.val return the double from pixelValue
	//[0] limites the returns to just the first field the other fields values are zero in gray scale images 
	for (int i = 0; i < sizeCols; i++)
	{
		pixelValue = mean(grayScaleImg.col(i));
		colAves[i] = pixelValue.val[0];
	}
	//Test output that shows the column number next to each column mean
	for (int i = 0; i < sizeCols; i++)
		cout << i << ": " << colAves[i] << endl;
	//Starting at the fourth column and then increaces buy one
	//absolute value of the difference between 
	for (int i = 4; i < sizeCols; i++)
	{
		if (abs(colAves[i]-colAves[i-4]) > 10)
		{
			cout << "Lane edge at " << i - 2 << endl;
		}
		
	}
	
	//for (vector<Scalar>::iterator i = columnAverage.begin(); i != columnAverage.end(); ++i)
	//{
	//	//i.val is of type cvmatrix, need to know how to interpet that data.
	//	cout << (*i).val << endl;
	//}

	/*
		int my_vector_size = columnAverage.size();

		// Increase vector before operations to some size 'n'
		int n = 2000;
		columnAverage.reserve(n);
	
	*/

	cout << "Image size (cols, rows): " << sizeCols << " " << sizeRows << endl;
	waitKey(0);

	return 0;
}