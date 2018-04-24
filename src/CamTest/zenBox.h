
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

// --list and indxer
// need int to keep track of list
void mouseValues()
{
	
	//store well cor
}

//This gives the user time to 
void waitChecker()
{
	int totalLadderEdges, ladderEdges;
	//test output
	cout << " press f key " << endl;
	//While q key is not pressed wait
	while (113 != waitKey(0) & 0xFF)
	{
		cout << "  " << endl;

		//if w is pressed
		if (119 == waitKey(0) & 0xFF)
		{
			cout << " Please enter the known values for the ladder " << endl;
			for (int i = 0; i < totalLadderEdges; i++)
			{
				cout << " Enter the value for the first ladder ban  " << endl;
			}
			

		}
		//wait
		//if lower case w is pressed exit out       
		if (113 == waitKey(0) & 0xFF)
		{
			return;
		}

	}
	cout << " f key has been pressed " << endl;

	//As long as the user has not clicked it will not go passed this point
	while (globalPD.x == 0)
	{
		//wait
	}
	mouseValues();
}

//use this function to pull the from pixelArray
//based on the column from the x value
void placeHolder()
{
	
	
}


#pragma once
//I need a function that measures edges in the selected column. And store the values
//I need a function that measures the distance between the point clicked and the each of the edges in the selected column, And store the values
//I need a function that compares ladder edges to the other edges
//I need a function to display all stored values