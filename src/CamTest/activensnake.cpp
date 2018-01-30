/*
	
	activesnake.c

	Author:	Hanspeter Schaub
	Date:	January 22, 2003
	Org.:	Sandia National Labs

	Provides the routines to interface with the Active Snake 
	routines developed by Chris Smith.  His sample code is used
	as a base for this code.  The image library Open CV is used
	here to store the image format.  


	- the functions pressureX() and pressureY have been 
	  combined into one function pressure().  This allows for 
	  only one call to compute the image statistical properties
	  about the (x,y) image location.

*/
#include "stdafx.h"
#include <cv.h>
#include "curve.h"
#include "activesnake.h"
#include <time.h>

#define LOGIT 0

/*-----------------------------------------------------------
| 
|  Routine Name: iterateSnake
| 
|       Purpose: run one iteration of the snake
| 
|         Input: the image the snake is on
|                    imge rows and colums
|                    an edge accumlator for intersection detection
|                    snake parameters alpha,beta,rho, and k
|                    pointer to the snake 
|                    mean and std of the seed region
|        Output: None
|    Written By: Doug Perrin
|          Date: Aug 22, 1997
| Modifications: 
| 
------------------------------------------------------------*/
/*
	modified by Hanspeter Schaub, Jan. 22, 2003
	Sandia National Labs
	- image is provided as an Open CV image structure
	- theSnake curve structure is an input to the routine
	- corrected spelling of "current" and "pressure"
	- fixed box_mean() routine to true average over a box 
	- enhanced readability of the routines 
	- changed the routine to return the number of curve
	  element points that have moved during this iteration 
	- routine now receives the OpenCV CvScalar class of
	  the image HSV mean and standard diviation values 
	modified by Christopher Smith July 28, 2003
	University of New Mexico/Sandia National Labs
	- added support for faster intersection checking in X
	modified by Christopher Smith February 1, 2004
	University of New Mexico/Sandia National Labs
	- added support for faster intersection checking in Y
*/
int  iterateSnake(IplImage			*img,
				  IplImage			*img2,
				  IplImage			*img3,
				  curve				*theSnake,
				  double			beta,
				  double			rho,
				  double			*fdim,
				  double			*fslop,
				  int				nodel,
				  int				Init)
{
	static curve *head,*current,*last,*temp;
	static int snakecount = 0;
	int check=0;
	int del=0,added=0,delang=0,x,y;

	double dx,dy,oldx,oldy, hold, *max_x_delta, new_max_x_delta=0.0;
	double minx, miny, maxx, maxy, *max_y_delta, new_max_y_delta=0.0;
	double curveX,curveY,pressX,pressY, curdim;
	double *spreadx, *spready;
	//double hue_value, val_value;
	CvScalar image_value;
	curve *holdprevx, *holdnextx, *holdprevy, *holdnexty;
	
	static FILE	*outfile;
	int		skip=0;  
	int		force=0;
	int		backoff=10;
	int		cols,rows;
	int		num_moved;				/* number of curve points moved */
	double	moved_tolerance,		/* tolerance to declare points
									   as having moved */
			distance_moved;			/* distance moved by curve element */
	double	tempx,
			tempy,
			angle,
			dist;
	curve  *t1,*tc1;
	
	curve_vector curveforce;
	

	/* run initialization routine */
	if (Init) {
//		outfile = fopen("output.txt","w");
//		fprintf(outfile, "Initializing with Means: %lf %lf %lf Std Dev: %lf %lf %lf\n", mean[0], mean[1], mean[2], sd[0], sd[1], sd[2]);
		snakecount = 0;

		return 1;
	}


	head      = theSnake;		/* store the snake beginning */
	current   = head;			/* set the current element to snake start */
	cols      = img->width;		/* x spans horizontal image width */
	rows      = img->height;	/* y spans vertical image height */
	num_moved = 0;				/* initialize the number of elements
								   moved to zero */
	moved_tolerance = 0.5;		/* set the elements moved tolerance
								   in units of pixels */
	max_x_delta = current->max_seg_distx;
	max_y_delta = current->max_seg_disty;
	spreadx = current->spreadx;
	spready = current->spready;
	maxx = 0.0;
	minx = 640.0;
	maxy = 0.0;
	miny = 480.0;

	/*go through elements moving and testing elements */
	do{
		if((current->x < cols-2) && (current->x > 2) &&
		   (current->y < rows-2) && (current->y > 2)){

			oldx = current->x;
			oldy = current->y;
//			fprintf(outfile, "Point: (%lf, %lf)\n", current->x, current->y);
			
			/* compute the force due to curvature error */
			curveforce = curvature(current);
//			fprintf(outfile, "Unscaled Curvature: (%lf, %lf)\n", curveforce.x, curveforce.y);
			/* compute the mean L, a, and b values 
			   within +/- 1 pixels neighborhood */

            x = (int)floor(current->x+0.5)-1;
            y = (int)floor(current->y+0.5)-1;
            curdim = fractalDimension(img, 5, x, y);
			
			/* compute the image pressure force */
			pressure(current, 
					&curdim,
					 fdim, fslop,
					 &pressX, &pressY);
//			fprintf(outfile, "Unscaled Pressure: (%lf, %lf)\n", pressX, pressY);

			/* scale the (x,y) force components */
			curveX = beta*curveforce.x;
			curveY = beta*curveforce.y;
			pressX = rho *pressX;
			pressY = rho *pressY;
//			fprintf(outfile, "Curvature: (%lf, %lf)\n", curveX, curveY);
//			fprintf(outfile, "Pressure: (%lf, %lf)\n", pressX, pressY);

			/* compute the total force acting on a curve point */   
			dx = curveX + pressX;
			dy = curveY + pressY;
		
			/* test the ranges */
			if((current->x+dx < (double) (cols-2)) && (current->x+dx > 2.0) && 
			   (current->y+dy < (double) (rows-2)) && (current->y+dy > 2.0))
			{
				current->x += dx;
				current->y += dy;
//				fprintf(outfile, "Final Forces: dx = %lf dy = %lf\n\n", dx, dy);
			}
			else
			{
//				fprintf(stderr,"%f %f %f %f %f\n",
//					current->x,current->y,curveforce.x, curveforce.y, beta);
			}
			if (current->x > maxx)
				maxx = current->x;
			if (current->x < minx)
				minx = current->x;
			if (current->y > maxy)
				maxy = current->y;
			if (current->y < miny)
				miny = current->y;

			/* Handle the online max_x_delta's */
			hold = fabs(current->x - current->prev->x);
			if (*max_x_delta < hold)
				*max_x_delta = hold;
			if (hold > new_max_x_delta)
				new_max_x_delta = hold;
			hold = fabs(current->x - current->next->x);
			if (*max_x_delta < hold)
				*max_x_delta = hold;
			if (hold > new_max_x_delta)
				new_max_x_delta = hold;

			/* Maintain the x-sorted list */
			if (current->lowerx != NULL)
			{
				if (current->x < current->lowerx->x)
				{
					temp = current->lowerx;
					if (current->higherx != NULL)
						current->higherx->lowerx = temp;
					temp->higherx = current->higherx;
					current->lowerx = current->higherx = NULL;
					insertptX(temp, current);
				}
			}
			if (current->higherx != NULL)
			{
				if (current->x > current->higherx->x)
				{
					temp = current->higherx;
					if (current->lowerx != NULL)
						current->lowerx->higherx = temp;
					temp->lowerx = current->lowerx;
					current->lowerx = current->higherx = NULL;
					insertptX(temp, current);
				}
			}

			/* Handle the online max_y_delta's */
			hold = fabs(current->y - current->prev->y);
			if (*max_y_delta < hold)
				*max_y_delta = hold;
			if (hold > new_max_y_delta)
				new_max_y_delta = hold;
			hold = fabs(current->y - current->next->y);
			if (*max_y_delta < hold)
				*max_y_delta = hold;
			if (hold > new_max_y_delta)
				new_max_y_delta = hold;

			/* Maintain the y-sorted list */
			if (current->lowery != NULL)
			{
				if (current->y < current->lowery->y)
				{
					temp = current->lowery;
					if (current->highery != NULL)
						current->highery->lowery = temp;
					temp->highery = current->highery;
					current->lowery = current->highery = NULL;
					insertptY(temp, current);
				}
			}
			if (current->highery != NULL)
			{
				if (current->y > current->highery->y)
				{
					temp = current->highery;
					if (current->lowery != NULL)
						current->lowery->highery = temp;
					temp->lowery = current->lowery;
					current->lowery = current->highery = NULL;
					insertptY(temp, current);
				}
			}

			/* avoid the snake from crossing over onto itself */
			if(xovercheck(theSnake,current))
			{
				dx = -pressX;
				dy = -pressY;
				
				dx = dx/100;
				dy = dy/100;
				if((current->x+dx < cols-2) && (current->x+dx > 2) &&
				   (current->y+dy < rows-2) && (current->y+dy > 2)){
					current->x += dx;
					current->y += dy;
				}		
			}
			
			/* if cross over problem persists, use old value */
			if(xovercheck(theSnake,current))
			{
				current->x = oldx;
				current->y = oldy;
			}
			

			
			/* 
				add points if there is space
			*/
			dist=sqrt(
				 (current->x-(current->next)->x)*
				 (current->x-(current->next)->x)
				+(current->y-(current->next)->y)*
				 (current->y-(current->next)->y));
			
			if(dist > DIST_THRESH_MAX	// check distance
				&& current != last		
				&& snakecount < MAX_ADD_SNAKE_POINTS // maintain max snake point limit
				&& !nodel){			// flag indicating to add points
		
			    /* compute new point */
				tempx = (-current->x + current->next->x)/2.0;
				tempy = (-current->y + current->next->y)/2.0;
				
				/* if new point is in-bounds, then add */
				if((current->x+tempx < cols-3) && (current->x+dx > 2) &&
				   (current->y+tempy < rows-3) && (current->y+dy > 2)){
					
					/* setup new point */
					temp= (curve *) malloc(sizeof(curve));
					temp->next      = current->next;
					temp->prev      = current;
					temp->id        = uid();
					temp->depth_est = 0;	
					temp->forthx    = 0.01;
					temp->forthy    = 0.01;
					temp->x         = current->x+tempx;
					temp->y         = current->y+tempy;
					temp->max_seg_distx = current->max_seg_distx;
					temp->max_seg_disty = current->max_seg_disty;
					temp->spreadx = current->spreadx;
					temp->spready = current->spready;
					temp->higherx = temp->lowerx = NULL;
					temp->highery = temp->lowery = NULL;
					temp->mflag = 0;
					
					/* link new point to neighbor points */
					tc1                 = current->next;
					t1                  = current;
					current->next->prev = temp;
					current->next       = temp;
					insertptX(current, temp);
					insertptY(current, temp);

					/* check if new point will cross over */
					if(xovercheck(theSnake,temp)){
						/* reset the snake */
						tc1->prev = t1;
						t1->next  = tc1;
						temp->higherx->lowerx = temp->lowerx;
						temp->lowerx->higherx = temp->higherx;
						temp->highery->lowery = temp->lowery;
						temp->lowery->highery = temp->highery;
						free(temp);
					}else{
						snakecount++;
						last = temp;
					}
				}
			}
			else
			{
			    /*
			    	see if points can be removed 
			    */
				dist=sqrt(
					 ((current->prev)->x-(current->next)->x)*
					 ((current->prev)->x-(current->next)->x)
					+((current->prev)->y-(current->next)->y)*
					 ((current->prev)->y-(current->next)->y));
				angle=p3ang(current->prev,current,current->next);			
				if(((dist <= DIST_THRESH_MIN
						|| angle > 3.1) 
						&& !nodel
						&& snakecount > 0
						&& current != head 
						&& current!=last)){
				
					/* store current point and reset neighbor links */
					temp                = current;
					current->next->prev = current->prev;
					current->prev->next = current->next;
					current             = current->prev;

					/* fast intersection */
					hold = fabs(current->x - current->next->x);
					if (*max_x_delta < hold)
						*max_x_delta = hold;
					hold = fabs(current->y - current->next->y);
					if (*max_y_delta < hold)
						*max_y_delta = hold;
					holdnextx = holdprevx = holdnexty = holdprevy = NULL;
					if (temp->higherx != NULL)
					{
						holdnextx = temp->higherx;
						temp->higherx->lowerx = temp->lowerx;
					}
					if (temp->lowerx != NULL)
					{
						holdprevx = temp->lowerx;
						temp->lowerx->higherx = temp->higherx;
					}
					if (temp->highery != NULL)
					{
						holdnexty = temp->highery;
						temp->highery->lowery = temp->lowery;
					}
					if (temp->lowery != NULL)
					{
						holdprevy = temp->lowery;
						temp->lowery->highery = temp->highery;
					}
				
					//should test to see if it is safe to remove
					if(xovercheck(theSnake,current)){
						/* reset the snake */
						current->next->prev = temp;
						current->next       = temp;
						current             = temp;
						if (holdprevx != NULL)
							holdprevx->higherx = temp;
						if (holdnextx != NULL)
							holdnextx->lowerx = temp;
						if (holdprevy != NULL)
							holdprevy->highery = temp;
						if (holdnexty != NULL)
							holdnexty->lowery = temp;
					} else {
						/* delete the point */
						free(temp);
						last = current->next;
						snakecount--;
					}
				}
			}
			
			
			
			/* check if current snake element has moved sufficiently */
			distance_moved = sqrt(pow(current->x-oldx,2) 
								+ pow(current->y-oldy,2));
			if (distance_moved > moved_tolerance)
			{
				num_moved++;
				current->mflag = 0;
			}
		}
		current = current->next;

	} while (head!=current && current->next!=NULL );
	*max_x_delta = new_max_x_delta;
	*max_y_delta = new_max_y_delta;
	*spreadx = maxx - minx;
	*spready = maxy - miny;
	
	return num_moved;
}


/*
	stablize_snake()

	This calls the iterateSnake() routine until the convergence
	criteria are met.  If only a few curve points have been moved, 
	or the maximum number of iterations has been reached, then 
	this routine will exit.  
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
	int				Init)
{
	int j = 0,
		elements_moved;
	static int max_loop;

	/* run initialization routine */
	if (Init) {
		max_loop = 100;

		iterateSnake(img,img2,img3,theSnake,
			beta,rho,
			fdim,fslop,
			noadd,
			1);	// initialize this subroutine

		return;
	}

	//	fprintf(stderr,"In Stabilize Mean: %lf %lf %lf, SD: %lf %lf %lf\n", mean->val[0], mean->val[1], mean->val[2], sd->val[0], sd->val[1], sd->val[2]);

	/* iterate over the snake curve */
	while (++j < max_loop) {

		elements_moved = iterateSnake(img,img2,img3,theSnake,
			beta,rho,
			fdim,fslop,
			noadd,0); 

		/*  check if number of snake elements moved 
		satisfies the convergence criteria */
		if ((criteria->type & CV_TERMCRIT_EPS) == CV_TERMCRIT_EPS) {
			if (elements_moved < criteria->epsilon) {
				j = max_loop;	// break out of the loop
			}
		}

		/*  check if number of iterations satisfies
		the convergence criteria */
		if ((criteria->type & CV_TERMCRIT_ITER) == CV_TERMCRIT_ITER) {
			if (++j >= criteria->max_iter) {
				j = max_loop;	// break out of the loop
			}
		}

	}


	return;
}


/*
	computes the force due to the curvature change of the snake
	curve.
*/
curve_vector curvature(curve *current)
{
	curve_vector force;
	curve *a,*b,*c,*d,*e,*perp,*mid;
	curve Cperp,Cmid;
	double linex,liney,length,px,py;
	curve_vector v1;
	double tvx,tvy;
	double angle1,angle2,angle3,targetangle;
	
	a = current->prev->prev;
	b = current->prev;
	c = current;
	d = current->next;
	e = current->next->next;  
	
	
	angle1 = p3ang(a,b,c);
	if(p3turn(a,b,c)>0)
		angle1=-angle1;
	
	
	angle2 = p3ang(b,c,d);
	if(p3turn(b,c,d)>0)
		angle2=-angle2;
	
	
	angle3 = p3ang(c,d,e);
	if(p3turn(c,d,e)>0)
		angle3=-angle3;
	
	
	targetangle = (angle1+angle2+angle3)/3;

	
	linex = b->x/2+d->x/2;
	liney = b->y/2+d->y/2;
	
	v1.x  = b->x-d->x;
	v1.y  = b->y-d->y;
	
	tvx   = -v1.y/vlen(v1);
	tvy   = v1.x/vlen(v1);
	
	
	if(fabs(sin((M_PI-targetangle)/2))<.001)
	{
		length = 0.0;
		px = py = 0.0;
	}
	else
	{
		length = sqrt(SQR(b->x-linex)+SQR(b->y-liney))
				* sin(targetangle/2)/sin((M_PI-targetangle)/2);
		px = (length)*tvx;
		py = (length)*tvy;
	}
		
	Cmid.x  = linex;
	Cmid.y  = liney;
	Cperp.x = tvx;
	Cperp.y = tvy;
	mid     = &Cmid;
	perp    = &Cperp;
	
	force.x=(linex-c->x)-px;
	force.y=(liney-c->y)-py;
		
	if(fabs(force.x)>.1 || fabs(force.y)>.1) {
		angle1 = angle1;
	}
	
	return(force);
}


/*
	computes the image pressure force.  

	if the current image values at (x,y) are equal to the target
	image values, then the snake point will push outward to try
	to expand the curve to encompass the entire image.  If the 
	current image value is not equal to the target image value,
	then the snake curve point will contract to try to find the
	target image.  
*/
void pressure(curve *current,
                          double *curdim,
                          double *fdim,
                          double *fslop,
                          double *pressureX, double *pressureY)
{
        double x1,x2,y1,y2;
        double tmp;
        double length;
#if LOGIT
        FILE *logfile;
#endif

        x1 = current->prev->x;
        x2 = current->next->x;
        y1 = current->prev->y;
        y2 = current->next->y;

        length = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));

        *pressureX = -(y2-y1)/length;
        *pressureY =  (x2-x1)/length;


        //tmp = *fslop - fabs(*fdim-*curdim);
        /* We expect this difference to be really small.
           0.0 to 2.0 is the range, but practically it
           should be more like 0.0 to 0.5 */
        tmp = fabs(*fdim-*curdim);
#if LOGIT
        logfile = fopen("frac.log","a");
		fseek(logfile, 0, SEEK_END);
        fprintf(logfile,"Reference Dimension %f Current Dimension %f Slop %f\n", *fdim, *curdim, *fslop);
#endif
        if (tmp <= *fslop)
        {  //positive pressure
                tmp /= *fslop;
                tmp = 1 - tmp;
        }
        else
        {  //negative pressure
                tmp -= *fslop;
                tmp = -tmp;
                tmp = tmp/ *fslop;
        }
        tmp = -tmp;
#if LOGIT
        fprintf(logfile,"Pressure %f\n", tmp);
        fclose(logfile);
#endif
        /* scale the pressure force depending on the mean image value */
        if (tmp < -1.)
        {
                *pressureX *= -1.;
                *pressureY *= -1.;
        }
        else if (tmp < 1.)
        { /* if -1 < tmp < 1 */
                *pressureX *= tmp;
                *pressureY *= tmp;
        }
        else /* Added to balance the limits on positive pressure */
        {
                *pressureX *= 1.;
                *pressureY *= 1.;
        }


        return;
}



/*
	computes the length of a vector
*/
double vlen(curve_vector v1)
{
	return(sqrt(fabs(SQR(v1.x))+fabs(SQR(v1.y))));
}




/*
	computes the mean value of an image point by averaging
	over a 3x3 box centered about the image point of interest.
*/
double box_mean(IplImage *img, curve *p1)
{
	double val=0;
	int x, y;
	
	
	x = (int)floor(p1->x+0.5)-1;
	y = (int)floor(p1->y+0.5)-1;
	
	val+=cvGetReal2D(img,y-1,x-1);
	val+=cvGetReal2D(img,y-1,x);
	val+=cvGetReal2D(img,y-1,x+1);
	
	val+=cvGetReal2D(img,y,  x-1);
	val+=cvGetReal2D(img,y,  x);
	val+=cvGetReal2D(img,y,  x+1);
	
	val+=cvGetReal2D(img,y+1,x-1);
	val+=cvGetReal2D(img,y+1,x);
	val+=cvGetReal2D(img,y+1,x+1);
	
	return(val/9.0);
}







/*	snakeMoments()

	Author:		Hanspeter Schaub
	Date:		Jan. 27, 2003
	Org.:		Sandia National Labs

	Computes the center of mass of the snake, as well
	as the eigenvectors and the eigenvalues.  Function returns
	the snake image area.  Note that the true image area 
	moments are being computed here described by the snake curve.  
*/
double snakeMoments(curve *theSnake,
				  double *xc,  double *yc,
				  double *L1,  double *L2,
				  double *v1x, double *v1y,
				  double *v2x, double *v2y)
{	
	curve		   *current;
	double			M00,			// snake image area
					dI,				// minimum inertia parameter
					dum,
					angle,			// heading angle of primary eigenvector
					Ix,				// image inertia
					Iy,				// image inertia
					Ixy;			// image inertia
	static double	v1x0,v1y0;		// old eigenvector values
	CvSeq		   *polygon;		// OpenCV point structure
	CvMemStorage   *storage;		
	CvPoint         point;
	CvMoments		moments;		// stores the OpenCV moments 
	//FILE *fp;
	//fp = fopen("test.dat","a");

	/* set minimum inertia parameter */
	dI = 0.00001;

	/* stuff snake curve into an Open CV cvSeq structure.  This 
	   allows us to use the Open CV area moment calculation 
	   routines.  */
	storage = cvCreateMemStorage(0);
	polygon = cvCreateSeq( CV_32SC2,		/* sequence of double (x,y) elements */
                          sizeof(CvSeq),	/* header size - no extra fields */
                          sizeof(CvPoint),	/* element size */
                          storage			/* the container storage */ );
	polygon->flags = polygon->flags | CV_SEQ_POLYGON;
	current=theSnake;
	do {
		point = cvPoint((int) current->x, (int) current->y);
		cvSeqPush( polygon, &point); 
		current=current->next;
	} while (theSnake!=current && current!=NULL);
	

	/* compute image moments using Open CV routines */
	cvMoments(polygon,&moments,0);
	
	/* free the temp CvSeq storage */
	cvReleaseMemStorage(&storage);



	/* find image total mass */
	M00 = cvGetSpatialMoment(&moments,0,0);
	
	/* find image center of mass coordinates (xc,yc) */
	*xc = cvGetSpatialMoment(&moments,1,0)/M00;
	*yc = cvGetSpatialMoment(&moments,0,1)/M00;
	
	/* get moments about center of image mass (scaled by the image mass) */
	Ix  = cvGetCentralMoment(&moments,2,0);
	Ixy = cvGetCentralMoment(&moments,1,1);
	Iy  = cvGetCentralMoment(&moments,0,2);
	
	
	/* find eigenvalues with L1 > L2 */
	dum  = sqrt((Ix-Iy)*(Ix-Iy) + 4*Ixy*Ixy);
	*L1  = 0.5*(Ix+Iy+dum);
	*L2  = 0.5*(Ix+Iy-dum);
	
	if ((Ix < 0) || (Iy < 0) || (*L1 < 0) || (*L2 < 0)) {
		/*  image snake must be near zero, these erroneous image
			inertias only seems to occur then. There might be some
			slight cross-overs happening with the snake points which
			would cause such negative values */
		*L1  = 0.;
		*L2  = 0.;
		*v1x = 1.;
		*v1y = 0.;
		*v2x = 0.;
		*v2y = 1.;

		return M00;
	}

	//fprintf(fp,"M00=%f xc=%f yc=%f Ix=%f Ixy=%f Iy=%f L1=%f L2=%f\n",
	//	M00, *xc, *yc, Ix, Ixy, Iy, *L1, *L2);
	//fclose(fp);
	
	
	/* compute primary heading angle */
	if ((fabs(2*Ixy) > dI) && (fabs(Ix - Iy) > dI)) {
		angle = atan2(2*Ixy,Ix-Iy)/2.;
	} else {
		angle = 0.;
	}
	
	/* compute primary eigenvector */
	*v1x =  cos(angle);
	*v1y =  sin(angle);
	/* keep sign of eigenvectors (avoid sign reversal) */
	if ((*v1x)*v1x0 + (*v1y)*v1y0 < -.8) {
		*v1x = -(*v1x);
		*v1y = -(*v1y);
	}
	/* compute secondary eigenvector */
	*v2x = -(*v1y);
	*v2y =  (*v1x);
	
	/* store old primary eigenvector values to make sure that the
	   next primary eigenvector is roughly in the same direction */
	v1x0 = *v1x;
	v1y0 = *v1y;
	
	return M00;
}




