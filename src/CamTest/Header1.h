// Header1.h : Defines the entry point for the camera interface.
//

#include "stdafx.h"
#include <highgui.h>
#include <cv.h>
#include <stdio.h>
#include <cmath>
#include "zenBox.h"

using namespace std;
using namespace cv;

// Define some evil global variables mainly due to OpenCV event handlers
double			g_rho = 128;
// int				curx = -1, cury = -1;
int				curx, cury;

const double	rhoScale = 2.55;

#define PREVIEW 0

static void calcRho(int valueSlider)
{
	g_rho = valueSlider * rhoScale;

	return;
}

// Callback for Mouse events in the video window
static void mouseHandler(int event, int x, int y, int, void*)
{
	// We only care about Left Clicks
	if (event != CV_EVENT_LBUTTONDOWN)
		return;

	// Get the image coordinates of the left click
	curx = x;
	cury = y;

	//Puts in vales in to the strutct from zenBox.h
	globalPD.x = x;
	globalPD.y = y;
	// Print the position
	//	fprintf(stderr, "Mouse: (%d, %d)\n", curx, cury);

	return;
}


/********************************************
* The main execution loop.					*
* Created by Christopher Smith				*
* Created on 10/26/2012						*
* Modifications:							*
*											*
********************************************/

//Changed to pointer
int cam() {

	CvCapture		*capture;
	IplImage		*frame,
		*result,
		*grey;
	int				value,
		valueV,
		valueH,
		kernel[3][3] = { { 1, 0, -1 },{ 2, 0, -2 },{ 1, 0, -1 } },
		kernelResultScale = 4,
		widthStep,
		channels,
		width,
		height,
		counter = 2,
		valSlider = 1;
	char			str[20];
	capture = cvCaptureFromCAM(CV_CAP_ANY);
	if (!capture)
	{
		fprintf(stderr, "There is no camera plugged in, please try again");
		getchar();
		return 1;
	}

	// Test to see if we can get frames from the cam
	frame = cvQueryFrame(capture);
	if (!frame)
	{
		fprintf(stderr, "ERROR: frame is null...\n");
		getchar();
		exit(-1);
	}

	// Allocate space for a couple of 8-bit, 1 channel images of the
	// same height and width of our camera frame
	grey = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

	result = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

	// Get height and width of camera frame
	height = frame->height;
	width = frame->width;

	// Get the stride of the image (width in pixels X number of channels)
	widthStep = frame->widthStep;

	// Number of channels (3 for color, 1 for grayscale)
	channels = frame->nChannels;

	// Set the current mouse click coords to the center of the image
	curx = width / 2;
	cury = height / 2;

	// Create a window in which the captured images will be presented
#if PREVIEW
	cvNamedWindow("mypreview", CV_WINDOW_AUTOSIZE);
#endif
	cvNamedWindow("myresult", CV_WINDOW_AUTOSIZE);
	// Create a window for all interface gadgets
	cvNamedWindow("myguis", CV_WINDOW_NORMAL);

	// Create interface gadgets
	sprintf_s(str, "*%f", rhoScale);
	cvCreateTrackbar(str, "myguis", &valSlider, 100, &calcRho);
	cvSetMouseCallback("myresult", mouseHandler, NULL);

	// Show the image captured from the camera in the window and repeat
	//If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
	//remove higher bits using AND operator

	widthStep = width;  // Only one channel
	while ((cvWaitKey(10) & 255) != 27)
	{
		// Get one frame. Added photo = frame so that we retrieve last image before ESC is hit 
		frame = cvQueryFrame(capture);
		//photo = frame;

		// Convert to grayscale and store in grey
		cvCvtColor(frame, grey, CV_RGB2GRAY);
#if PREVIEW
		cvShowImage("mypreview", grey);
#endif

		cvShowImage("myresult", frame);

		// REMINDER: Do not release the frame!
	}
	// Release the capture device housekeeping
	cvReleaseCapture(&capture);
#if PREVIEW
	cvDestroyWindow("mypreview");
#endif
	cvDestroyWindow("mywindow");
	cvDestroyWindow("myguis");
	return 0;

}
#pragma once
