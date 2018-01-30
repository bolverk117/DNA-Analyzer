/************************************************************
 * curve.c													*
 *----------------------------------------------------------*
 *	Copyright (C) 2000 Douglas Perrin, Christopher E. Smith	*
 *															*
 *	This program is free software; you can redistribute it	*
 *	and/or modify it under the terms of the GNU General		*
 *	Public License as published by the Free Software		*
 *	Foundation; either version 2 of the License, or (at your*
 *	option) any later version.								*
 *															*
 *	This program is distributed in the hope that it will be	*
 *	useful, but WITHOUT ANY WARRANTY; without even the		*
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A	*
 *	PARTICULAR PURPOSE.  See the GNU General Public License	*
 *	for more details.										*
 *															*
 *	You should have received a copy of the GNU General		*
 *	Public License along with this program; if not, write to*
 *	the Free Software Foundation, Inc., 59 Temple Place,	*
 *	Suite 330, Boston, MA  02111-1307  USA					*
 *															*
 *	You may contact the authors at dperrin@cs.umn.edu or	*
 *	chsmith@eece.unm.edu									*
 *----------------------------------------------------------*
 *			Do Not Remove this notice.						*
 ************************************************************/

/*
	changes made by Hanspeter Schaub
	- changed "vector" structure to be called "curve_vector"
	to avoid issues with the C++ vector class
*/


#include "stdafx.h"
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "curve.h"


#define M_PI 3.1415927
#define xor(a,b) (a||b)&&(!(a&&b))




long int uid(){
	static long int lastid=-1;
	lastid++;
	return (lastid);
}


_inline double crossprodz(curve_vector v1, curve_vector v2)
{
	return v1.x*v2.y-v2.x*v1.y;
}


double p3ang(curve *a,curve *b,curve *c){
	double v1x,v1y,v2x,v2y;
	double dot,len1,len2;
    double angle;
	v1x = b->x - a->x;
	v1y = b->y - a->y;
	v2x = c->x - b->x;
	v2y = c->y - b->y;

	dot=v1x*v2x+v1y*v2y;

	len1=sqrt(v1x*v1x+v1y*v1y);
	len2=sqrt(v2x*v2x+v2y*v2y);
    
	if (dot/(len1*len2) > 1.0)
		angle = acos(1.0);
	else if(fabs(len1*len2) > 0.0001)
		angle=acos(dot/(len1*len2));
	else
		angle = M_PI/2.;
	return(angle);	
}

int left(curve *a,curve *b,curve *c)
{
	return(area2(a,b,c)>0);
}


_inline int xorold(int a,int b){
	if (a + b ==0)
		return(0);
	else
		return((a+b)<2);
}
_inline int intersectProp(curve *a,curve *b,curve *c,curve *d)
{
	if(
		collinear(a,b,c) ||
		collinear(a,b,d) ||
		collinear(c,d,a) ||
		collinear(c,d,b)) return(0);
	return(
		( xor(left(a,b,c),left(a,b,d)) ) &&
		( xor(left(c,d,a),left(c,d,b)) ));
}


_inline int between(curve *a,curve *b,curve *c)
{
	if(!collinear(a,b,c))
		return(0);
	if(a->x!=b->x)
		return(((a->x<=c->x)&&(c->x<=b->x))||
		((a->x>=c->x)&&(c->x>=b->x)));
	else
		return(((a->y<=c->y)&&(c->y<=b->y))||
		((a->y>=c->y)&&(c->y>=b->y)));
}



int intersect(curve *a,curve *b,curve *c,curve *d)
{
	if(intersectProp(a,b,c,d))
		return(1);
	else if(
		between(a,b,c) ||
		between(a,b,d) ||
		between(c,d,a) ||
		between(c,d,b)) return(1);
	else
		return(0);
}




int xovercheck(curve *theCurve, curve *curvePoint){
	curve *current,*center,*back,*forward,*c1,*c2;
	int count = 0;
	int xover=0;
	int temp=0;
	double boundx, minx, maxx, lowx, highx;
	double boundy, miny, maxy, lowy, highy;


	center=curvePoint;
	forward=curvePoint->next;
	back=curvePoint->prev;

#if 1
	if (*(center->spreadx) > *(center->spready))
	{
		/* Faster (we hope) intersection detection */
		maxx = (center->x > forward->x) ? center->x : forward->x;
		maxx = (maxx > back->x) ? maxx : back->x;
		minx = (center->x < forward->x) ? center->x : forward->x;
		minx = (minx < back->x) ? minx : back->x;
		
		boundx = (*(center->max_seg_distx))/2.0;
		lowx = minx - boundx;
		highx = maxx + boundx;
		current = center->lowerx;
		xover = 0;
		while (current != NULL && current->x >= lowx && !xover)
		{
			c1 = current->prev;
			c2 = current->next;
			if (c1 != center)
			{
				if (c1 != forward && current != forward)
					xover = intersect(center, forward, current, c1);
				if (xover == 0 && c1 != back && current != back)
					xover = intersect(center, back, current, c1);
			}
			if (c2 != center && !xover)
			{
				if (c2 != forward && current != forward)
					xover = intersect(center, forward, current, c2);
				if (xover == 0 && c2 != back && current != back)
					xover = intersect(center, back, current, c2);
			}
			current = current->lowerx;
		}
		current = center->higherx;
		while (current != NULL && current->x <= highx && !xover)
		{
			c1 = current->prev;
			c2 = current->next;
			if (c1 != center)
			{
				if (c1 != forward && current != forward)
					xover = intersect(center, forward, current, c1);
				if (xover == 0 && c1 != back && current != back)
					xover = intersect(center, back, current, c1);
			}
			if (c2 != center && !xover)
			{
				if (c2 != forward && current != forward)
					xover = intersect(center, forward, current, c2);
				if (xover == 0 && c2 != back && current != back)
					xover = intersect(center, back, current, c2);
			}
			current = current->higherx;
		}
	}
	else
	{
		/* Faster (we hope) intersection detection */
		maxy = (center->y > forward->y) ? center->y : forward->y;
		maxy = (maxy > back->y) ? maxy : back->y;
		miny = (center->y < forward->y) ? center->y : forward->y;
		miny = (miny < back->y) ? miny : back->y;
		
		boundy = (*(center->max_seg_disty))/2.0;
		lowy = miny - boundy;
		highy = maxy + boundy;
		current = center->lowery;
		xover = 0;
		while (current != NULL && current->y >= lowy && !xover)
		{
			c1 = current->prev;
			c2 = current->next;
			if (c1 != center)
			{
				if (c1 != forward && current != forward)
					xover = intersect(center, forward, current, c1);
				if (xover == 0 && c1 != back && current != back)
					xover = intersect(center, back, current, c1);
			}
			if (c2 != center && !xover)
			{
				if (c2 != forward && current != forward)
					xover = intersect(center, forward, current, c2);
				if (xover == 0 && c2 != back && current != back)
					xover = intersect(center, back, current, c2);
			}
			current = current->lowery;
		}
		current = center->highery;
		while (current != NULL && current->y <= highy && !xover)
		{
			c1 = current->prev;
			c2 = current->next;
			if (c1 != center)
			{
				if (c1 != forward && current != forward)
					xover = intersect(center, forward, current, c1);
				if (xover == 0 && c1 != back && current != back)
					xover = intersect(center, back, current, c1);
			}
			if (c2 != center && !xover)
			{
				if (c2 != forward && current != forward)
					xover = intersect(center, forward, current, c2);
				if (xover == 0 && c2 != back && current != back)
					xover = intersect(center, back, current, c2);
			}
			current = current->highery;
		}
	}


#else 
	head=curvePoint->prev;
	current=curvePoint->next;
	do{
		
		c1=current;
		c2=current->next;
		
		if(current!=curvePoint->next){		
			temp=intersect(center,forward,c1,c2);			
		}
		if(temp==0){
			if(current!=curvePoint->prev->prev)	{
				temp=intersect(back,center,c1,c2);
			}
			
		}
		
		if(temp!=0)
		{	xover=1;
		//    printf("Move denied\n");
		}
		current=(*current).next;
	}while(head!=current && head!=NULL&&xover==0);
#endif

	return(xover); 
}





curve *makeCircle(int OX,int OY,int radius,int elements){
	curve *head;
	curve *current,*prev;
	double theta,dTheta;
	int i=0;
	dTheta=2.0*M_PI/(elements);
	theta=0.0;
	head= (curve *) malloc(sizeof(curve));
	current=head;
	prev=head;
	
	for (i=0;i<elements;i++){
		//	printf("i:%d of %d\tx->%lf\ty->%lf\n",i,elements,
		//   (double)OX+(double)radius*cos(theta),
		//       (double)OY+(double)radius*sin(theta));
		
		(*current).x=(double)OX+(double)radius*cos(theta);
		(*current).y=(double)OY+(double)radius*sin(theta);
		(*current).id=uid();
		//(*current).id=i;
		(*current).depth_est=0;	
		
		(*current).forthx=0.01;
		(*current).forthy=0.01;
		
		(*current).prev=prev;
		(*current).next= (curve *) malloc(sizeof(curve));
		prev=current;
		current=(*current).next;
		theta+=dTheta;
	}
	
	//	printf("i:%d of %d\tx->%lf\ty->%lf\n",i,elements,
	//	   (double)OX+(double)radius*cos(theta),
	//      (double)OY+(double)radius*sin(theta));
	current=prev;
	//  (*current).x=(double)OX+(double)radius*cos(theta);
	//  (*current).y=(double)OY+(double)radius*sin(theta);
	//(*current).id=uid();
	//(*current).depth_est=0;	
	//(*current).forthx=0.01;
	//(*current).forthy=0.01;
	//(*current).prev=prev;
	
	(*current).next=head;
	
	/*complets the circle*/
	
    head->prev=current;
	
    return(head);
}


/* ------------------------------------------------------------ */
/*	routine added to support faster polygonal self-intersection
	checking based upon a list ordered by X-coordinates of control
	points
*/
/*
	insertptX()

	Author:	Christopher Smith
	Date:	July 28, 2003
	Org:	University of New Mexico/Sandia National Labs
	Note:	routine assumes that the starting point is in the middle of the list

*/
void insertptX(curve *current, curve *insert)
{
	curve *temp;

	temp = current;
	if (temp->x > insert->x)
	{ /* Insertion moving backward */
		while (temp->x > insert->x && temp->lowerx != NULL)
			temp = temp->lowerx;
		if (temp->x > insert->x)
		{ /* Insertion before head */
			temp->lowerx = insert;
			insert->higherx = temp;
			insert->lowerx = NULL;
		}
		else
		{ /* Normal insertion after temp */
			insert->lowerx = temp;
			insert->higherx = temp->higherx;
			if (temp->higherx != NULL)
				temp->higherx->lowerx = insert;
			temp->higherx = insert;
		}
	}
	else
	{ /* Insertion moving forward */
		while (temp->x < insert->x && temp->higherx != NULL)
			temp = temp->higherx;
		if (temp->x < insert->x)
		{ /* Insertion after tail */
			temp->higherx = insert;
			insert->lowerx = temp;
			insert->higherx = NULL;
		}
		else
		{ /* Normal insertion before temp */
			insert->lowerx = temp->lowerx;
			insert->higherx = temp;
			if (temp->lowerx != NULL)
				temp->lowerx->higherx = insert;
			temp->lowerx = insert;
		}
	}
}


/* ------------------------------------------------------------ */
/*	routine added to support faster polygonal self-intersection
	checking based upon a list ordered by Y-coordinates of control
	points
*/
/*
	insertptY()

	Author:	Christopher Smith
	Date:	February 1, 2004
	Org:	University of New Mexico/Sandia National Labs
	Note:	routine assumes that the starting point is in the middle of the list

*/
void insertptY(curve *current, curve *insert)
{
	curve *temp;

	temp = current;
	if (temp->y > insert->y)
	{ /* Insertion moving backward */
		/* Find the position to insert */
		while (temp->y > insert->y && temp->lowery != NULL)
			temp = temp->lowery;
		if (temp->y > insert->y)
		{ /* Insertion before head */
			temp->lowery = insert;
			insert->highery = temp;
			insert->lowery = NULL;
		}
		else
		{ /* Normal insertion after temp */
			insert->lowery = temp;
			insert->highery = temp->highery;
			if (temp->highery != NULL)
				temp->highery->lowery = insert;
			temp->highery = insert;
		}
	}
	else
	{ /* Insertion moving forward */
		/* Find the position to insert */
		while (temp->y < insert->y && temp->highery != NULL)
			temp = temp->highery;
		if (temp->y < insert->y)
		{ /* Insertion after tail */
			temp->highery = insert;
			insert->lowery = temp;
			insert->highery = NULL;
		}
		else
		{ /* Normal insertion before temp */
			insert->lowery = temp->lowery;
			insert->highery = temp;
			if (temp->lowery != NULL)
				temp->lowery->highery = insert;
			temp->lowery = insert;
		}
	}
}

/* ------------------------------------------------------------ */
/*
	routines added by Hanspeter Schaub, Jan. 22, 2003
	Sandia National Labs
*/

/*
	makeEllipse()

	Author:	Hanspeter Schaub
	Date:	January 22, 2003
	Org.:	Sandia National Labs
	Note:	- changed makeCircle routine to set an ellipse.
	Mod:	Christopher Smith
	Date:	July 28, 2003, February 1, 2004
	Note:	Added code for faster intersection support
			
*/
curve *makeEllipse(int OX,int OY,int a, int b, int elements, double *max_x_delta, double *spreadx,
				   double *max_y_delta, double *spready)
{
	curve *head;
	curve *current,*prev;
	double theta,dTheta,hold,minx=640.0,miny=480.0,maxx=0.0,maxy=0.0;
	int i=0;
	dTheta=2.0*M_PI/(elements);
	theta=0.0;
	head = (curve *) malloc(sizeof(curve));
	current = head;
	prev = head;
	current->lowerx = NULL;
	current->higherx = NULL;
	current->lowery = NULL;
	current->highery = NULL;
	
	/*	loop through all points and set the previous 
		and next curve points */
	for (i=0;i<elements;i++)
	{
		current->x=(double)OX+(double)a*cos(theta);
		current->y=(double)OY+(double)b*sin(theta);
//		fprintf(stderr, "(%lf, %lf)\n", current->x, current->y);
		minx = (current->x < minx) ? current->x : minx;
		maxx = (current->x > maxx) ? current->x : maxx;
		miny = (current->y < miny) ? current->y : miny;
		maxy = (current->y > maxy) ? current->y : maxy;
		current->id=uid();
		current->depth_est=0;
		current->max_seg_distx = max_x_delta;
		current->spreadx = spreadx;
		current->max_seg_disty = max_y_delta;
		current->spready = spready;
		hold = fabs(current->x - prev->x);
		if ((*max_x_delta) < hold)
			(*max_x_delta) = hold;
		hold = fabs(current->y - prev->y);
		if ((*max_y_delta) < hold)
			(*max_y_delta) = hold;
		
		current->forthx=0.01;
		current->forthy=0.01;
		
		current->prev=prev;
		current->next= (curve *) malloc(sizeof(curve));
		if (current != head)
		{
			insertptX(prev, current);
			insertptY(prev, current);
		}
		prev=current;
		current=(*current).next;
		current->lowerx = current->higherx = NULL;
		current->lowery = current->highery = NULL;
		theta+=dTheta;
	}
	
	current=prev;
	(*current).next=head;
	
	/*completes the circle*/
    head->prev=current;
	hold = fabs(current->x - current->next->x);
	if ((*max_x_delta) < hold)
		*max_x_delta = hold;
	hold = fabs(current->y - current->next->y);
	if ((*max_y_delta) < hold)
		*max_y_delta = hold;
	current = head;
	*spreadx = maxx - minx;
	*spready = maxy - miny;
/*
	while (current->lowerx != NULL)
		current = current->lowerx;
	while (current->higherx != NULL)
	{
		if (current->x > current->higherx->x)
			exit(-1);
		current = current->higherx;
	}
	while (current->lowerx != NULL)
	{
		if (current->x < current->lowerx->x)
			exit(-1);
		current = current->lowerx;
	}
*/
    return(head);
}
