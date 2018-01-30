/************************************************************
 * curve.h													*
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

#ifndef CURVE
#define CURVE

#ifdef __cplusplus
extern "C"  {
#endif

typedef struct curve_rec {
  double x;
  double y;
  double movex;
  double movey;
  double id;
  double depth_est;
  double forthx;
  double forthy;
  double rx;
  double ry;
  int	 tag, mflag;
  struct curve_rec *next;
  struct curve_rec *prev;
  struct curve_rec *higherx;
  struct curve_rec *lowerx;
  struct curve_rec *highery;
  struct curve_rec *lowery;
  double *max_seg_distx;
  double *spreadx;
  double *max_seg_disty;
  double *spready;
} curve;

typedef struct curve_vec {
	double x,y;
} curve_vector;

typedef struct {
	unsigned int count;
	double x,y;
} axis;



curve *makeCircle(int OX,int OY,int radius,int elements);
long int uid();
_inline int between(curve *a,curve *b,curve *c);
_inline int xorold(int a,int b);
int left(curve *a,curve *b,curve *c);
double p3ang(curve *a,curve *b,curve *c);
_inline double crossprodz(curve_vector v1, curve_vector v2);
_inline int intersectProp(curve *a,curve *b,curve *c,curve *d);
int intersect(curve *a,curve *b,curve *c,curve *d);
int xovercheck(curve *theCurve, curve *curvePoint);
void insertptX(curve *current, curve *insert);
void insertptY(curve *current, curve *insert);

#define SGN(a)          (((a)<0) ? -1 : ((a>0) ? 1 :0))
#define p3turn(a,b,c) SGN(((a->x-b->x)*(b->y-c->y)-(b->x-c->x)*(a->y-b->y)))

#define area2(a,b,c) ((b->x-a->x)*(c->y-a->y)-(c->x-a->x)*(b->y-a->y))

#define collinear(a,b,c) (area2(a,b,c)==0)


/* routines added by Hanspeter Schaub, Jan 22, 2003 */
curve *makeEllipse(int OX,int OY,int a, int b, int elements, double *max_x_delta,
				   double *max_y_delta, double *spreadx, double *spready);


#ifdef __cplusplus
}
#endif


#endif