/*
	
	activesnake.h

	Author:	Hanspeter Schaub
	Date:	January 22, 2003
	Org.:	Sandia National Labs

	Provides the declarations to interface with the Active Snake 
	routines developed by Chris Smith.  His sample code is used
	as a base for this code.

*/
#include <stdlib.h>
#include "stdafx.h"
#include <math.h>
#include <cv.h>
#include "curve.h"

#define LOGTOFILE 0
#ifndef ACTIVESNAKE
#define ACTIVESNAKE

#ifdef __cplusplus
extern "C"  {
#endif


/* 
	declare the definitions 
*/
/*max distance before point is added*/ 
#define DIST_THRESH_MAX			12
/*min distance before a point is subtracted*/
#define DIST_THRESH_MIN			5
/*max number of snake points that are allowed to be added */
#define MAX_ADD_SNAKE_POINTS	400
/*min number of snake points used to setup snake */
#define MIN_SNAKE_POINTS		10


#define SQR(a)				(fabs((a)*(a)))
#define M_PI				3.1415927


/*
	declare the subroutines
*/
void stablize_snake(IplImage	    *img,
		IplImage		*img2,
		IplImage		*img3,
		curve		    *theSnake,
		double			beta,
		double			rho,
		double			*fdim,
		double			*fslop,
		CvTermCriteria *criteria,
		int			    noadd,
		int				Init);
int  iterateSnake(IplImage     *img,
					IplImage	   *img2,
					IplImage	   *img3,
				  curve		   *theSnake,
				  double		beta,
				  double		rho,
				  CvScalar	   *mean,
				  CvScalar	   *sd,
				  int			nodel,
				  int			Init);
curve_vector curvature(curve *current);
double vlen(curve_vector v1);
double box_mean(IplImage *img, curve *p1);
double fractalDimension(IplImage *grey, int max_steps, int xx, int yy);
void pressure(curve		*current,
			  double	*curdim,
			  double	*fdim,
			  double	*fslop,
			  double	*pressureX, double *pressureY);
double snakeMoments(curve *theSnake,
				  double *xc,  double *yc,
				  double *L1,  double *L2,
				  double *v1x, double *v1y,
				  double *v2x, double *v2y);



#ifdef __cplusplus
}
#endif


#endif
