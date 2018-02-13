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
	vector<Scalar> columnAverage;

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
	// What can we do with the image?
	sizeCols = grayScaleImg.cols;
	sizeRows = grayScaleImg.rows;
	


	for (int i = 0; i < grayScaleImg.cols; i++)
	{
		columnAverage.push_back( mean(grayScaleImg.col(i)) );
	}
	/*
	for (i = 1; i < grayScaleImg.cols - 1; i++)
	{
		columnAverage[i - 1];
		columnAverage[i];
		columnAverage[i + 1];
	}
	*/
	for (vector<Scalar>::iterator i = columnAverage.begin(); i != columnAverage.end(); ++i)
	{
		//i.val is of type cvmatrix, need to know how to interpet that data.
		cout << (*i).val << endl;
	}

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