#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glui.h"

//MARK Inserted code
#define TEMP	7
#define SQR(x)		( (x) * (x) )

const float TEMPMIN = { 0.f };
const float TEMPMAX = { 100.f };
const float GRAYMIN = { 0.20f };
const float GRAYMAX = { 1.00f };
const float DISTMIN = { 0.f };
const float DISTMAX = { 1.7f };

const char *	TEMPFORMAT = { "Temperature: %5.2f - %5.2f" };
const char *	DISTFORMAT = { "Distance: %5.2f - %5.2f" };
float			TempLowHigh[2] = { TEMPMIN, TEMPMAX };
float			DistLowHigh[2] = { DISTMIN, DISTMAX };
GLUI_StaticText *	TempLabel;
GLUI_StaticText *	DistLabel;

// size of wings as fraction of length:

#define WINGS	0.10


// axes:

#define X	1
#define Y	2
#define Z	3

// x, y, z, axes:

static float axx[3] = { 1., 0., 0. };
static float ayy[3] = { 0., 1., 0. };
static float azz[3] = { 0., 0., 1. };



// function declarations:

void	Arrow(float[3], float[3]);
void	Cross(float[3], float[3], float[3]);
float	Dot(float[3], float[3]);
float	Unit(float[3], float[3]);


void
Arrow(float tail[3], float head[3])
{
	float u[3], v[3], w[3];		// arrow coordinate system

	// set w direction in u-v-w coordinate system:

	w[0] = head[0] - tail[0];
	w[1] = head[1] - tail[1];
	w[2] = head[2] - tail[2];

	// determine major direction:

	int axis = X;
	float mag = fabs(w[0]);
	float f = fabs(w[1]);
	if (f > mag)
	{
		axis = Y;
		mag = f;
	}
	f = fabs(w[2]);
	if (f > mag)
	{
		axis = Z;
		mag = f;
	}

	// set size of wings and turn w into a unit vector:

	float d = WINGS * Unit(w, w);

	// draw the shaft of the arrow:

	glBegin(GL_LINE_STRIP);
	glVertex3fv(tail);
	glVertex3fv(head);
	glEnd();

	// draw two sets of wings in the non-major directions:

	float x, y, z;			// points to plot to
	if (axis != X)
	{
		Cross(w, axx, v);
		(void)Unit(v, v);
		Cross(v, w, u);
		x = head[0] + d * (u[0] - w[0]);
		y = head[1] + d * (u[1] - w[1]);
		z = head[2] + d * (u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
		x = head[0] + d * (-u[0] - w[0]);
		y = head[1] + d * (-u[1] - w[1]);
		z = head[2] + d * (-u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
	}

	if (axis != Y)
	{
		Cross(w, ayy, v);
		(void)Unit(v, v);
		Cross(v, w, u);
		x = head[0] + d * (u[0] - w[0]);
		y = head[1] + d * (u[1] - w[1]);
		z = head[2] + d * (u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
		x = head[0] + d * (-u[0] - w[0]);
		y = head[1] + d * (-u[1] - w[1]);
		z = head[2] + d * (-u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
	}

	if (axis != Z)
	{
		Cross(w, azz, v);
		(void)Unit(v, v);
		Cross(v, w, u);
		x = head[0] + d * (u[0] - w[0]);
		y = head[1] + d * (u[1] - w[1]);
		z = head[2] + d * (u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
		x = head[0] + d * (-u[0] - w[0]);
		y = head[1] + d * (-u[1] - w[1]);
		z = head[2] + d * (-u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
	}
}



float
Dot(float va[3], float vb[3])
{
	return(va[0] * vb[0] + va[1] * vb[1] + va[2] * vb[2]);
}



void
Cross(float va[3], float vb[3], float vout[3])
{
	float tmp[3];

	tmp[0] = va[1] * vb[2] - vb[1] * va[2];
	tmp[1] = vb[0] * va[2] - va[0] * vb[2];
	tmp[2] = va[0] * vb[1] - vb[0] * va[1];

	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}



float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrt(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}

	return dist;
}

struct node
{
	float x, y, z;          // location
	float t;                // temperature
	float r, g, b;			// the assigned color
	float rad;              // radius
	float dist;				//Distance from center
	float dTdx, dTdy, dTdz;	// can store these if you want, or not
	float grad;             // total gradient
};


class vector
{
	float x, y, z;
	float vxp, vyp, vzp;
	float r, g, b;
	float curl, speed;
	float head[3], tail[3];
public: 
	vector()
	{

	}

	vector(float xIn, float yIn, float zIn, int step)
	{
		const float maxSpeed = 2.0f;

		x = xIn;
		y = yIn;
		z = zIn;
		vxp = y * z * (y*y + z * z);
		vyp = x * z * (x*x + z * z);
		vzp = x * y * (x*x + y * y);

		float dvxdy = 3.*y*y*z + z * z*z;
		float dvxdz = 3.*y*z*z + y * y*y;
		float dvydx = 3.*x*x*z + z * z*z;
		float dvydz = 3.*x*z*z + x * x*x;
		float dvzdx = 3.*x*x*y + y * y*y;
		float dvzdy = 3.*x*y*y + x * x*x;

		float cx = dvzdy - dvydz;
		float cy = dvxdz - dvzdx;
		float cz = dvydx - dvxdy;

		curl = sqrt(cx*cx + cy * cy + cz * cz);
		speed = sqrt(SQR(vxp) + SQR(vyp) + SQR(vzp));
		
		if (speed > .66)
		{
			r = 1.f;
			if (speed - .66f > .66f)
			{
				g = 1.f;
				b = (speed - 1.33f) / .66f;
			}
			else
			{
				g = (speed - .66f) / .66f;
				b = 0.f;
			}
				
		}
		else
		{
			r = speed / .66f;
			g = 0.f;
			b = 0.f;
		}

		tail[0] = x - .5 * 1 / ((float)step) * vxp;
		tail[1] = y - .5 * 1 / ((float)step) * vyp;
		tail[2] = z - .5 * 1 / ((float)step) * vzp;
		head[0] = x + .5 * 1 / ((float)step) * vxp;
		head[1] = y + .5 * 1 / ((float)step) * vyp;
		head[2] = z + .5 * 1 / ((float)step) * vzp;
	}

	float getCurl()
	{
		return curl;
	}

	float getSpeed()
	{
		return speed;
	}

	float * getTail()
	{
		return tail;
	}

	float * getHead()
	{
		return head;
	}

	void colorVect()
	{
		glColor3f(r, g, b);
	}
};



//	This is a sample OpenGL / GLUT / GLUI program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "Assignment 5 - Richard Cunard" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };


// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


// line widths:

const float ISO_WIDTH = { 2. };
const float CONTOUR_WIDTH = { 2. };


// ranges and scalar function:
/*
//project 4
const float XMIN = { -36.56f };
const float XMAX = {  36.56f };
const float YMIN = { -22.65f };
const float YMAX = {  22.65f };
*/

//project 1 & 2
const float XMIN = { -1.00f };
const float XMAX = { 1.00f };
const float YMIN = { -1.00f };
const float YMAX = { 1.00f };
const float ZMIN = { -1.00f };
const float ZMAX = {  1.00f };
const float RMIN = {  0.00f };
const float RMAX = {  1.732f };
const float GMIN = {  0.00f };
const float GMAX = { 300.00f };
const float KMIN = { 1.00f };
const float KMAX = { 100.00f };
const float curlMin = { 0.00f };
const float curlMax = { 3.00f };
const float speedMin = { 0.f };
const float speedMax = { 2.4f };


const float SMIN = {   0.0 };
const float SMAX = { 100.0 };

const int GRIDSKIP = { 4 };

#define S	0
#define X	1
#define Y	2
#define Z	3
#define R	4
#define G	5
#define DIST 8
#define K	9
#define CurlID 10
#define SpeedID	11

const char *SFORMAT = { "S: %.3f - %.3f" };
const char *XFORMAT = { "X: %.3f - %.3f" };
const char *YFORMAT = { "Y: %.3f - %.3f" };
const char *ZFORMAT = { "Z: %.3f - %.3f" };
const char *RFORMAT = { "R: %.3f - %.3f" };
const char *GFORMAT = { "G: %.3f - %.3f" };
const char *KFORMAT = { "K: %.3f - %.3f" };
const char *CurlFORMAT = { "Curl: %.3f - %.3f" };
const char *SpeedFORMAT = { "Speed: %.3f - %.3f" };

const int SLIDERWIDTH = { 200 };


// how much to divide the plane and volume:

#define N	12
#define NX	N
#define NY	N
#define NZ	N

struct node  Nodes[NX][NY][NZ];
vector vect[N][N][N];
// non-constant global variables

int		ActiveButton;			// current button that is down
GLuint		AxesList;			// list to hold the axes
int		AxesOn;				// != 0 means to draw the axes
int		DebugOn;			// != 0 means to print debugging info
int		DepthCueOn;			// != 0 means to use intensity depth cueing
GLUI *		Glui;				// instance of glui window
int		GluiWindow;			// the glut id for the glui window
int		Grayscale;			// display in grayscale
int		Cartesian;
int		draw;
int		MainWindow;			// window id for main graphics window
int		PointsOn;			// display points
int		jitter;
float		RotMatrix[4][4];	// set by glui rotation widget
float		Scale, Scale2;			// scaling factor
float		TransXYZ[3];		// set by glui translation widgets
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float		Xrot, Yrot;			// rotation angles in degrees


float		SLowHigh[2];
float		XLowHigh[2];
float		YLowHigh[2];
float		ZLowHigh[2];
float		RLowHigh[2];
float		GLowHigh[2];
float		KLowHigh[2];
float		CurlLowHigh[2];
float		SpeedLowHigh[2];

GLUI_StaticText * SLabel;
GLUI_StaticText * XLabel;
GLUI_StaticText * YLabel;
GLUI_StaticText * ZLabel;
GLUI_StaticText * RLabel;
GLUI_StaticText * GLabel;
GLUI_StaticText * KLabel;
GLUI_StaticText * CurlLabel;
GLUI_StaticText * SpeedLabel;


// function prototypes:

void	Animate( );
void	Buttons(int);
void	Display( );
void	DoAxesMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGlui( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Sliders( int );
float	Temperature( float, float, float );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );

int   Npts[68];
float Xdat[68][263];
float Ydat[68][263];

// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

	InitLists( );


	// init all the global variables used by Display( ):
	// this will also post a redisplay
	// it is important to call this before InitGlui():
	// so that the variables that glui will control are correct
	// when each glui widget is created

	Reset( );


	// setup the user interface stuff:

	InitGlui( );


	// setup all the user interface stuff:

	InitMenus( );


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );


	// this is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


//
// glui buttons callback:
///

void
Buttons( int id )
{
	switch( id )
	{
		case RESET:
			Reset();
			break;

		case QUIT:
			/* gracefully close the glui window:				*/
			/* gracefully close out the graphics:				*/
			/* gracefully close the graphics window:			*/
			/* gracefully exit the program:					*/

			Glui->close();
			glutSetWindow( MainWindow );
			glFinish();
			glutDestroyWindow( MainWindow );
			exit( 0 );
	}

	Glui->sync_live();
}


//Hyperbolic t-intersection fix

void drawHyp(float x0, float x1, float y0, float y1)
{
	float AX = (x1 + x0) / 2.f;
	float AY = (y1 + y0) / 2.f;

	float r = sqrt(SQR(AX + XLowHigh[0]) + SQR(AY + YLowHigh[0]));
	float rPrime = r / (r + KLowHigh[0]);
	float AXPrime = (AX + XLowHigh[0]) / (r + KLowHigh[0]);
	float AYPrime = (AY + YLowHigh[0]) / (r + KLowHigh[0]);

	r = sqrt(SQR(x0 + XLowHigh[0]) + SQR(y0 + YLowHigh[0]));
	float x0Prime = (x0 + XLowHigh[0]) / (r + KLowHigh[0]);
	float y0Prime = (y0 + YLowHigh[0]) / (r + KLowHigh[0]);

	r = sqrt(SQR(x1 + XLowHigh[0]) + SQR(y1 + YLowHigh[0]));
	float x1Prime = (x1 + XLowHigh[0]) / (r + KLowHigh[0]);
	float y1Prime = (y1 + YLowHigh[0]) / (r + KLowHigh[0]);

	float BX = (x1Prime + x0Prime) / 2.f;
	float BY = (y1Prime + y0Prime) / 2.f;
	
	if (sqrt(SQR(BX - AXPrime) + SQR(BY - AYPrime)) < .000001f)
	{
		if (Cartesian)
		{
			x0Prime = x0Prime / sqrt(SQR(x0Prime) + SQR(KLowHigh[0]));
			y0Prime = y0Prime / sqrt(SQR(y0Prime) + SQR(KLowHigh[0]));
			x1Prime = x1Prime / sqrt(SQR(x1Prime) + SQR(KLowHigh[0]));
			y1Prime = y1Prime / sqrt(SQR(y1Prime) + SQR(KLowHigh[0]));
		}
		glVertex2f(x0Prime, y0Prime);
		glVertex2f(x1Prime, y1Prime);
	}
	else
	{
		drawHyp(x0, AX, y0, AY);
		drawHyp(AX, x1, AY, y1);
	}
}

// draw the complete scene:

void
Display( )
{
	if (DebugOn != 0)
	{
		fprintf(stderr, "Display\n");
	}


	// set which window we want to do the graphics into:

	glutSetWindow(MainWindow);


	// erase the background:

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);


	// specify shading to be flat:

	glShadeModel(GL_FLAT);


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(90., 1., 0.1, 1000.);


	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// set the eye position, look-at position, and up-vector:

	gluLookAt(0., 0., 3., 0., 0., 0., 0., 1., 0.);


	// rotate the scene:

	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);


	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	float scale2 = 1.f + Scale2;           // because glui translation starts at 0
	if (scale2 < MINSCALE)
		scale2 = MINSCALE;
	glScalef(scale2, scale2, scale2);



	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}


	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3f(1., 1., 1.);
		glCallList(AxesList);
	}


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable(GL_NORMALIZE);


	// draw the current object:

	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;

	// create the object:

	/*
	//Mark Project 1/2
	glPointSize(1. / (5. * N));
	glBegin(GL_POINTS);
	for (int i = 0; i < N; i++)
	{
		if (Nodes[i][0][0].x < XLowHigh[0] || Nodes[i][0][0].x > XLowHigh[1])
			continue;
		for (int j = 0; j < N; j++)
		{
			if (Nodes[0][j][0].y < YLowHigh[0] || Nodes[0][j][0].y > YLowHigh[1])
				continue;
			for (int k = 0; k < N; k++)
			{
				if (Nodes[0][0][k].z < ZLowHigh[0] || Nodes[0][0][k].z > ZLowHigh[1])
					continue;

				if (Nodes[i][j][k].t <= TempLowHigh[0] || Nodes[i][j][k].t >= TempLowHigh[1])
					continue;

				if (Nodes[i][j][k].grad <= GLowHigh[0] || Nodes[i][k][j].grad > GLowHigh[1])
					continue;

				if (Nodes[i][j][k].dist < DistLowHigh[0] || Nodes[i][k][j].dist > DistLowHigh[1])
					continue;


				if (Grayscale)
				{
					float gray = GRAYMIN + (GRAYMAX - GRAYMIN) * (Nodes[i][j][k].t - TEMPMIN) / (TEMPMAX - TEMPMIN);
					glColor3f(gray, gray, gray);		// r = g = b gives gray
				}
				else
				{
					glColor3f(Nodes[i][j][k].r, Nodes[i][j][k].g, Nodes[i][j][k].b);
				}

				if (jitter)
				{
					float xDist = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					float yDist = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					float zDist = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					glVertex3f(xDist * Nodes[i][j][k].rad + Nodes[i][j][k].x, yDist * Nodes[i][j][k].rad + Nodes[i][j][k].y, zDist * Nodes[i][j][k].rad + Nodes[i][j][k].z);
				}
				else
					glVertex3f(Nodes[i][j][k].x, Nodes[i][j][k].y, Nodes[i][j][k].z);
			}
		}
	}
	glEnd();
	*/
	/*
	//MARK Project 4

	float x, y, xPrime, yPrime;

	for (int i = 0; i < 68; i++)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < Npts[i]; j++)
		{
			x = Xdat[i][j];
			y = Ydat[i][j];

			if (draw)
			{
				if (j < Npts[i] - 1)
					drawHyp(x, Xdat[i][j + 1], y, Ydat[i][j + 1]);
			}
			else
			{
				float r = sqrt(SQR(Xdat[i][j] + XLowHigh[0]) + SQR(Ydat[i][j] + YLowHigh[0]));
				xPrime = (x + XLowHigh[0]) / (r + KLowHigh[0]);
				yPrime = (y + YLowHigh[0]) / (r + KLowHigh[0]);

				if (Cartesian)
				{
					xPrime = xPrime / sqrt(SQR(xPrime) + SQR(KLowHigh[0]));
					yPrime = yPrime / sqrt(SQR(yPrime) + SQR(KLowHigh[0]));
				}

				glVertex2f(xPrime, yPrime);
			}
		}
		glEnd();
	}
	*/

	//project 5

	glBegin(GL_LINE_STRIP);
	glColor3f(1., 1., 1.);

	glVertex3f(-1., -1., -1.);
	glVertex3f(1., -1., -1.);
	glVertex3f(1., -1., 1.);
	glVertex3f(-1., -1., 1.);
	glVertex3f(-1., -1., -1.);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-1., 1., -1.);
	glVertex3f(1., 1., -1.);
	glVertex3f(1., 1., 1.);
	glVertex3f(-1., 1., 1.);
	glVertex3f(-1., 1., -1.);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-1., -1., -1.);
	glVertex3f(-1., 1., -1.);
	glVertex3f(-1., 1., 1.);
	glVertex3f(-1., -1., 1.);
	glVertex3f(-1., -1., -1.);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(1., -1., -1.);
	glVertex3f(1., 1., -1.);
	glVertex3f(1., 1., 1.);
	glVertex3f(1., -1., 1.);
	glVertex3f(1., -1., -1.);
	glEnd();

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for (int k = 0; k < N; k++)
			{
				if (vect[i][j][k].getCurl() < CurlLowHigh[0] || vect[i][j][k].getCurl() > CurlLowHigh[1])
					continue;
				if (vect[i][j][k].getSpeed() < SpeedLowHigh[0] || vect[i][j][k].getSpeed() > SpeedLowHigh[1])
					continue;

				vect[i][j][k].colorVect();
				Arrow(vect[i][j][k].getTail(), vect[i][j][k].getHead());
			}
		}
	}

	/*
	glBegin( GL_QUADS );

	glColor3f( 0., 0., 1. );
	glNormal3f( 0., 0.,  1. );
	glVertex3f( -dx, -dy,  dz );
	glVertex3f(  dx, -dy,  dz );
	glVertex3f(  dx,  dy,  dz );
	glVertex3f( -dx,  dy,  dz );

	glNormal3f( 0., 0., -1. );
	glTexCoord2f( 0., 0. );
	glVertex3f( -dx, -dy, -dz );
	glTexCoord2f( 0., 1. );
	glVertex3f( -dx,  dy, -dz );
	glTexCoord2f( 1., 1. );
	glVertex3f(  dx,  dy, -dz );
	glTexCoord2f( 1., 0. );
	glVertex3f(  dx, -dy, -dz );

	glColor3f( 1., 0., 0. );
	glNormal3f(  1., 0., 0. );
	glVertex3f(  dx, -dy,  dz );
	glVertex3f(  dx, -dy, -dz );
	glVertex3f(  dx,  dy, -dz );
	glVertex3f(  dx,  dy,  dz );

	glNormal3f( -1., 0., 0. );
	glVertex3f( -dx, -dy,  dz );
	glVertex3f( -dx,  dy,  dz );
	glVertex3f( -dx,  dy, -dz );
	glVertex3f( -dx, -dy, -dz );

	glColor3f( 0., 1., 0. );
	glNormal3f( 0.,  1., 0. );
	glVertex3f( -dx,  dy,  dz );
	glVertex3f(  dx,  dy,  dz );
	glVertex3f(  dx,  dy, -dz );
	glVertex3f( -dx,  dy, -dz );

	glNormal3f( 0., -1., 0. );
	glVertex3f( -dx, -dy,  dz );
	glVertex3f( -dx, -dy, -dz );
	glVertex3f(  dx, -dy, -dz );
	glVertex3f(  dx, -dy,  dz );

	glEnd( );
	*/

	// draw some gratuitous text:
	/*
	glDisable( GL_DEPTH_TEST );
	glColor3f( 0., 1., 1. );
	DoRasterString( 0., 1., 0., "Text That Moves" );
	*/

	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates
	/*
	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	DoRasterString( 5., 5., 0., "Text That Doesn't" );
	*/

	// swap the double-buffered framebuffers:

	glutSwapBuffers();


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:

			Glui->close();
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	Glui->sync_live();

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}


//
// initialize the glui window:
//

void
InitGlui( )
{
	char str[256];

	if( DebugOn )
		fprintf( stderr, "InitGlui\n" );

	// setup the glui window:

	glutInitWindowPosition( INIT_WINDOW_SIZE + 50, 0 );
	Glui = GLUI_Master.create_glui( (char *) GLUITITLE );

	Glui->add_statictext( (char *) GLUITITLE );
	Glui->add_separator();

	//Project 1 & 2
	GLUI_Panel * panel = Glui->add_panel(  "" );
	//Glui->add_checkbox_to_panel( panel, "Points", &PointsOn );
	//Glui->add_checkbox_to_panel(panel, "Jitter", &jitter);


	GLUI_Rollout * rollout = Glui->add_rollout("Range Sliders", true);

	//Project 4
	//Glui->add_checkbox( "Grayscale", &Grayscale );
	//Glui->add_checkbox("Cartesian", &Cartesian);
	//Glui->add_checkbox("Fix T-Intersections", &draw);
	/*
	GLUI_HSlider * slider = Glui->add_slider_to_panel(rollout, false, GLUI_HSLIDER_FLOAT, KLowHigh, K, (GLUI_Update_CB)Sliders);
	slider->set_float_limits(KMIN, KMAX);
	slider->set_slider_val(KLowHigh[0], KLowHigh[1]);
	slider->set_w(SLIDERWIDTH);
	sprintf(str, KFORMAT, KLowHigh[0], KLowHigh[1]);
	DistLabel = Glui->add_statictext_to_panel(rollout, str);
	Glui->add_separator_to_panel(rollout);
	
	GLUI_HSlider * slider = Glui->add_slider_to_panel( rollout, false, GLUI_HSLIDER_FLOAT, SLowHigh, S, (GLUI_Update_CB) Sliders );
	slider->set_float_limits( SLowHigh[0], SLowHigh[1] );
	slider->set_slider_val( SLowHigh[0], SLowHigh[1] );
	slider->set_w( SLIDERWIDTH );
	sprintf( str, SFORMAT, SLowHigh[0], SLowHigh[1] );
	SLabel = Glui->add_statictext_to_panel( rollout, str );
	Glui->add_separator_to_panel( rollout );

	slider = Glui->add_slider_to_panel( rollout, false, GLUI_HSLIDER_FLOAT, XLowHigh, X, (GLUI_Update_CB) Sliders );
	slider->set_float_limits( XMIN, XMAX );
	slider->set_slider_val( XLowHigh[0], XLowHigh[1] );
	slider->set_w( SLIDERWIDTH );
	sprintf( str, XFORMAT, XLowHigh[0], XLowHigh[1] );
	XLabel = Glui->add_statictext_to_panel( rollout, str );
	Glui->add_separator_to_panel( rollout );
	*/

	/*
	//Project 1 & 2
	GLUI_HSlider * slider = Glui->add_slider_to_panel( rollout, true, GLUI_HSLIDER_FLOAT, SLowHigh, S, (GLUI_Update_CB) Sliders );
	slider->set_float_limits( SLowHigh[0], SLowHigh[1] );
	slider->set_slider_val( SLowHigh[0], SLowHigh[1] );
	slider->set_w( SLIDERWIDTH );
	sprintf( str, SFORMAT, SLowHigh[0], SLowHigh[1] );
	SLabel = Glui->add_statictext_to_panel( rollout, str );
	Glui->add_separator_to_panel( rollout );
	
	slider = Glui->add_slider_to_panel( rollout, true, GLUI_HSLIDER_FLOAT, XLowHigh, X, (GLUI_Update_CB) Sliders );
	slider->set_float_limits( XMIN, XMAX );
	slider->set_slider_val( XLowHigh[0], XLowHigh[1] );
	slider->set_w( SLIDERWIDTH );
	sprintf( str, XFORMAT, XLowHigh[0], XLowHigh[1] );
	XLabel = Glui->add_statictext_to_panel( rollout, str );
	Glui->add_separator_to_panel( rollout );

	slider = Glui->add_slider_to_panel( rollout, true, GLUI_HSLIDER_FLOAT, YLowHigh, Y, (GLUI_Update_CB) Sliders );
	slider->set_float_limits( YMIN, YMAX );
	slider->set_slider_val( YLowHigh[0], YLowHigh[1] );
	slider->set_w( SLIDERWIDTH );
	sprintf( str, YFORMAT, YLowHigh[0], YLowHigh[1] );
	YLabel = Glui->add_statictext_to_panel( rollout, str );
	Glui->add_separator_to_panel( rollout );

	slider = Glui->add_slider_to_panel( rollout, true, GLUI_HSLIDER_FLOAT, ZLowHigh, Z, (GLUI_Update_CB) Sliders );
	slider->set_float_limits( ZLowHigh[0], ZLowHigh[1] );
	slider->set_slider_val( ZLowHigh[0], ZLowHigh[1] );
	slider->set_w( SLIDERWIDTH );
	sprintf( str, ZFORMAT, ZLowHigh[0], ZLowHigh[1] );
	ZLabel = Glui->add_statictext_to_panel( rollout, str );
	Glui->add_separator_to_panel( rollout );

	slider = Glui->add_slider_to_panel( rollout, true, GLUI_HSLIDER_FLOAT, RLowHigh, R, (GLUI_Update_CB) Sliders );
	slider->set_float_limits( RLowHigh[0], RLowHigh[1] );
	slider->set_slider_val( RLowHigh[0], RLowHigh[1] );
	slider->set_w( SLIDERWIDTH );
	sprintf( str, RFORMAT, RLowHigh[0], RLowHigh[1] );
	RLabel = Glui->add_statictext_to_panel( rollout, str );
	Glui->add_separator_to_panel( rollout );

	slider = Glui->add_slider_to_panel( rollout, true, GLUI_HSLIDER_FLOAT, GLowHigh, G, (GLUI_Update_CB) Sliders );
	slider->set_float_limits( GLowHigh[0], GLowHigh[1] );
	slider->set_slider_val( GLowHigh[0], GLowHigh[1] );
	slider->set_w( SLIDERWIDTH );
	sprintf( str, GFORMAT, GLowHigh[0], GLowHigh[1] );
	GLabel = Glui->add_statictext_to_panel( rollout, str );
	Glui->add_separator_to_panel( rollout );

	slider = Glui->add_slider_to_panel(rollout, true, GLUI_HSLIDER_FLOAT, TempLowHigh, TEMP, (GLUI_Update_CB)Sliders);
	slider->set_float_limits(TempLowHigh[0], TempLowHigh[1]);
	slider->set_slider_val(TempLowHigh[0], TempLowHigh[1]);
	slider->set_w(SLIDERWIDTH);
	sprintf(str, TEMPFORMAT, TempLowHigh[0], TempLowHigh[1]);
	TempLabel = Glui->add_statictext_to_panel(rollout, str);
	Glui->add_separator_to_panel(rollout);

	slider = Glui->add_slider_to_panel(rollout, true, GLUI_HSLIDER_FLOAT, DistLowHigh, DIST, (GLUI_Update_CB)Sliders);
	slider->set_float_limits(DistLowHigh[0], DistLowHigh[1]);
	slider->set_slider_val(DistLowHigh[0], DistLowHigh[1]);
	slider->set_w(SLIDERWIDTH);
	sprintf(str, DISTFORMAT, DistLowHigh[0], DistLowHigh[1]);
	DistLabel = Glui->add_statictext_to_panel(rollout, str);
	Glui->add_separator_to_panel(rollout);
	*/
	
	//project 5
	GLUI_HSlider * slider = Glui->add_slider_to_panel(rollout, true, GLUI_HSLIDER_FLOAT, CurlLowHigh, CurlID, (GLUI_Update_CB)Sliders);
	slider->set_float_limits(curlMin, curlMax);
	slider->set_slider_val(CurlLowHigh[0], CurlLowHigh[1]);
	slider->set_w(SLIDERWIDTH);
	sprintf(str, CurlFORMAT, CurlLowHigh[0], CurlLowHigh[1]);
	CurlLabel = Glui->add_statictext_to_panel(rollout, str);
	Glui->add_separator_to_panel(rollout);

	slider = Glui->add_slider_to_panel(rollout, true, GLUI_HSLIDER_FLOAT, SpeedLowHigh, SpeedID, (GLUI_Update_CB)Sliders);
	slider->set_float_limits(speedMin, speedMax);
	slider->set_slider_val(SpeedLowHigh[0], SpeedLowHigh[1]);
	slider->set_w(SLIDERWIDTH);
	sprintf(str, SpeedFORMAT, SpeedLowHigh[0], SpeedLowHigh[1]);
	SpeedLabel = Glui->add_statictext_to_panel(rollout, str);
	Glui->add_separator_to_panel(rollout);

	Glui->add_checkbox( "Perspective", &WhichProjection );

	Glui->add_checkbox( "Axes", &AxesOn );

	Glui->add_checkbox( "Intensity Depth Cueing", &DepthCueOn );



	panel = Glui->add_panel( "Transformations" );

		GLUI_Rotation * rot = Glui->add_rotation_to_panel( panel, "Rotation", (float *) RotMatrix );
		rot->set_spin( 1. );

		Glui->add_column_to_panel( panel, FALSE );
		GLUI_Translation * scale = Glui->add_translation_to_panel( panel, "Scale",  GLUI_TRANSLATION_Y , &Scale2 );
		scale->set_speed( 0.01f );

		Glui->add_column_to_panel( panel, FALSE );
		GLUI_Translation * trans = Glui->add_translation_to_panel( panel, "Trans XY", GLUI_TRANSLATION_XY, &TransXYZ[0] );
		trans->set_speed( 0.1f );

		Glui->add_column_to_panel( panel, FALSE );
		trans = Glui->add_translation_to_panel( panel, "Trans Z",  GLUI_TRANSLATION_Z , &TransXYZ[2] );
		trans->set_speed( 0.1f );

	Glui->add_checkbox( "Debug", &DebugOn );


	panel = Glui->add_panel( "", FALSE );

	Glui->add_button_to_panel( panel, "Reset", RESET, (GLUI_Update_CB) Buttons );

	Glui->add_column_to_panel( panel, FALSE );

	Glui->add_button_to_panel( panel, "Quit", QUIT, (GLUI_Update_CB) Buttons );


	// tell glui what graphics window it needs to post a redisplay to:

	Glui->set_main_gfx_window( MainWindow );


	// set the graphics window's idle function:

	GLUI_Master.set_glutIdleFunc( NULL );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions


void
InitGraphics( )
{
	//projects 1 & 2
	/*
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for (int k = 0; k < N; k++)
			{
				Nodes[i][j][k].x = 2. *(float)i / (float)N - 1.;
				Nodes[i][j][k].y = 2. * (float)j / (float)N - 1.;
				Nodes[i][j][k].z = 2. * (float)k / (float)N - 1.;
				Nodes[i][j][k].t = Temperature(Nodes[i][j][k].x, Nodes[i][j][k].y, Nodes[i][j][k].z);
				Nodes[i][j][k].rad = 1. /  (1.f * N);
				Nodes[i][j][k].dist = sqrt(SQR(Nodes[i][j][k].x) + SQR(Nodes[i][j][k].y) + SQR(Nodes[i][j][k].z));

				float hsv[3], rgb[3];
				hsv[0] = 240. - (240 * (Nodes[i][j][k].t - TEMPMIN) / (TEMPMAX - TEMPMIN));
				hsv[1] = hsv[2] = 1.;
				HsvRgb(hsv, rgb);
				Nodes[i][j][k].r = rgb[0];
				Nodes[i][j][k].g = rgb[1];
				Nodes[i][j][k].b = rgb[2];
			}
		}
	}
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for (int k = 0; k < N; k++)
			{

				//x gradient
				if (i != 0 && i != N - 1)
					Nodes[i][j][k].dTdx = (Nodes[i + 1][j][k].t - Nodes[i - 1][j][k].t) / (Nodes[i + 1][j][k].x - Nodes[i - 1][j][k].x);
				else if (i == 0)
					Nodes[i][j][k].dTdx = (Nodes[i + 1][j][k].t - Nodes[i][j][k].t) / (Nodes[i + 1][j][k].x - Nodes[i][j][k].x);
				else
					Nodes[i][j][k].dTdx = (Nodes[i][j][k].t - Nodes[i - 1][j][k].t) / (Nodes[i][j][k].x - Nodes[i - 1][j][k].x);
				//y gradient
				if (j != 0 && j != N - 1)
					Nodes[i][j][k].dTdy = (Nodes[i][j + 1][k].t - Nodes[i][j - 1][k].t) / (Nodes[i][j + 1][k].y - Nodes[i][j - 1][k].y);
				else if (j == 0)
					Nodes[i][j][k].dTdy = (Nodes[i][j + 1][k].t - Nodes[i][j][k].t) / (Nodes[i][j + 1][k].y - Nodes[i][j][k].y);
				else
					Nodes[i][j][k].dTdy = (Nodes[i][j][k].t - Nodes[i][j - 1][k].t) / (Nodes[i][j][k].y - Nodes[i][j - 1][k].y);
				//z gradient
				if (k != 0 && k != N - 1)
					Nodes[i][j][k].dTdz = (Nodes[i][j][k + 1].t - Nodes[i][j][k - 1].t) / (Nodes[i][j][k + 1].z - Nodes[i][j][k - 1].z);
				else if (k == 0)
					Nodes[i][j][k].dTdz = (Nodes[i][j][k + 1].t - Nodes[i][j][k].t) / (Nodes[i][j][k + 1].z - Nodes[i][j][k].z);
				else
					Nodes[i][j][k].dTdz = (Nodes[i][j][k].t - Nodes[i][j][k - 1].t) / (Nodes[i][j][k].z - Nodes[i][j][k - 1].z);

				Nodes[i][j][k].grad = sqrt(SQR(Nodes[i][j][k].dTdx) + SQR(Nodes[i][j][k].dTdy) + SQR(Nodes[i][j][k].dTdz));
			}
		}
	}

	*/
	
	//project 4
	/*
	FILE *fp = fopen("proj04.dat", "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open 'proj07.dat' !\n");
		exit(1);
	}

	for (int i = 0; i < 68; i++)
	{
		fscanf(fp, "%d", &Npts[i]);
		for (int j = 0; j < Npts[i]; j++)
		{
			fscanf(fp, "%f %f", &Xdat[i][j], &Ydat[i][j]);
		}
	}
	fclose(fp);
	*/


	//project 5
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for (int k = 0; k < N; k++)
			{
				vect[i][j][k] = vector(2. *(float)i / (float)N - 1., 2. *(float)j / (float)N - 1., 2. *(float)k / (float)N - 1., N);
			}
		}
	}

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);


	// DO NOT SET THE GLUT IDLE FUNCTION HERE !!
	// glutIdleFunc( NULL );
	// let glui take care of it in InitGlui()


	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	 Glui->sync_live();

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthCueOn = 0;
	Grayscale = GLUIFALSE;
	Cartesian = GLUIFALSE;
	draw = GLUIFALSE;
	jitter = GLUIFALSE;
	PointsOn = GLUITRUE;
	Scale  = 1.0;
	Scale2 = 0.0;

	WhichProjection = PERSP;
	Xrot = Yrot = 0.;

	TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;

	                  RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
	RotMatrix[1][0]                   = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
	RotMatrix[2][0] = RotMatrix[2][1]                   = RotMatrix[2][3] = 0.;
	RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3]                   = 0.;
	RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;


	SLowHigh[0] = SMIN;
	SLowHigh[1] = SMAX;
	XLowHigh[0] = XMIN;
	XLowHigh[1] = XMAX;
	YLowHigh[0] = YMIN;
	YLowHigh[1] = YMAX;
	ZLowHigh[0] = ZMIN;
	ZLowHigh[1] = ZMAX;
	RLowHigh[0] = RMIN;
	RLowHigh[1] = RMAX;
	GLowHigh[0] = GMIN;
	GLowHigh[1] = GMAX;
	KLowHigh[0] = 10.f;
	KLowHigh[1] = KMAX;
	CurlLowHigh[0] = curlMin;
	CurlLowHigh[1] = curlMax;
	SpeedLowHigh[0] = speedMin;
	SpeedLowHigh[1] = speedMax;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

// slider callback:

void
Sliders( int id )
{
	char str[256];

	switch( id )
	{
		case S:
			sprintf( str, SFORMAT, SLowHigh[0], SLowHigh[1] );
			SLabel->set_text( str );
			break;

		case X:
			sprintf( str, XFORMAT, XLowHigh[0], XLowHigh[1] );
			XLabel->set_text( str );
			break;

		case Y:
			sprintf( str, YFORMAT, YLowHigh[0], YLowHigh[1] );
			YLabel->set_text( str );
			break;

		case Z:
			sprintf( str, ZFORMAT, ZLowHigh[0], ZLowHigh[1] );
			ZLabel->set_text( str );
			break;

		case R:
			sprintf( str, RFORMAT, RLowHigh[0], RLowHigh[1] );
			RLabel->set_text( str );
			break;

		case G:
			sprintf( str, GFORMAT, GLowHigh[0], GLowHigh[1] );
			GLabel->set_text( str );
			break;

		case TEMP:
			sprintf(str, TEMPFORMAT, TempLowHigh[0], TempLowHigh[1]);
			TempLabel->set_text(str);
			break;

		case DIST:
			sprintf(str, DISTFORMAT, DistLowHigh[0], DistLowHigh[1]);
			DistLabel->set_text(str);
			break;

		case K:
			sprintf(str, KFORMAT, KLowHigh[0], KLowHigh[1]);
			DistLabel->set_text(str);
			break;

		case CurlID:
			sprintf(str, CurlFORMAT, CurlLowHigh[0], CurlLowHigh[1]);
			CurlLabel->set_text(str);
			break;

		case SpeedID:
			sprintf(str, SpeedFORMAT, SpeedLowHigh[0], SpeedLowHigh[1]);
			SpeedLabel->set_text(str);
			break;
	}


	glutSetWindow( MainWindow );
	glutPostRedisplay();
}


// produce the scalar temperature value:


struct centers
{
	float xc, yc, zc;	/* center location			*/
	float a;		/* amplitude				*/
} Centers[] =
{
	{	 1.00f,	 0.00f,	 0.00f,	 90.00f	},
	{	-1.00f,	 0.30f,	 0.00f,	120.00f	},
	{	 0.00f,	 1.00f,	 0.00f,	120.00f	},
	{	 0.00f,	 0.40f,	 1.00f,	170.00f	},
};

const int NUMCENTERS = { sizeof(Centers) / sizeof(struct centers) };



float
Temperature( float x, float y, float z )
{
	float s = 0.0;
	for( int i = 0; i < NUMCENTERS; i++ )
	{
		float dx = x - Centers[i].xc;
		float dy = y - Centers[i].yc;
		float dz = z - Centers[i].zc;
		float dsqd = SQR(dx) + SQR(dy) + SQR(dz);
		s += Centers[i].a * (float)exp( -5.*dsqd );
	}

	return s;
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
