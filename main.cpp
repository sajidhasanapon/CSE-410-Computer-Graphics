#include <bits/stdc++.h>
#include <windows.h>
#include "include/glut.h"
using namespace std;

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double speed;
double speed_mult;

struct point
{
	double x, y, z;
};

struct point p, l, r, u;

struct point add(struct point a, struct point b)
{
	struct point ret;

	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	ret.z = a.z + b.z;

	return ret;
}

struct point sub(struct point a, struct point b)
{
	struct point ret;

	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	ret.z = a.z - b.z;

	return ret;
}

struct point rotate(struct point vec, struct point ref, double sign)
{
	struct point n, ret;
	double angle = 0.05 * sign;

	n.x = ref.y*vec.z - ref.z*vec.y;
	n.y = ref.z*vec.x - ref.x*vec.z;
	n.z = ref.x*vec.y - ref.y*vec.x;

	ret.x = vec.x * cos(angle) + n.x * sin(angle);
	ret.y = vec.y * cos(angle) + n.y * sin(angle);
	ret.z = vec.z * cos(angle) + n.z * sin(angle);

	return ret;
}

void drawAxes()
{
	if (drawaxes == 1)
	{
		glBegin(GL_LINES); {
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(100, 0, 0);
			glVertex3f(-100, 0, 0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0, -100, 0);
			glVertex3f(0, 100, 0);

			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0, 0, 100);
			glVertex3f(0, 0, -100);
		}glEnd();
	}
}

void drawFilledCircle(double radius, int segments)
{
    int i, j;
    struct point points[segments];
    
    //generate points
    for(i = 0; i < segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }

    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
		for(j = 0; j<segments;j++)
		{
			glBegin(GL_LINES);
       		{
				glVertex3f(points[i].x,points[i].y,0);
				glVertex3f(points[j].x,points[j].y,0);
       		}
        	glEnd();
		}  
    }
}

void draw_flower()
{	
	glTranslatef(100, 0, 0);
	glRotatef(speed * 2, 0, 0, 1);
	glColor3f(1, 0, 0);
	drawFilledCircle(10, 100);

	for(int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glTranslatef(20 * sin(60*i*pi/180.0), 20 * cos(60*i*pi/180.0), 0);
		glColor3f(0, !(i%2), i%2);
		drawFilledCircle(10, 100);
		glPopMatrix();
	}
}

void draw_all_flowers()
{
	for(int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glRotatef(i*60, 0, 1, 0);
		draw_flower();
		glPopMatrix();
	}
}


void online()
{
	glRotatef(angle, 0, 1, 0);
	draw_all_flowers();
	//draw_flower();
}


void keyboardListener(unsigned char key, int x, int y)
{

	switch (key)
	{
	case '1':
		l = rotate(l, u, 1);
		r = rotate(r, u, 1);
		break;
	case '2':
		l = rotate(l, u, -1);
		r = rotate(r, u, -1);
		break;
	case '3':
		u = rotate(u, r, -1);
		l = rotate(l, r, -1);
		break;
	case '4':
		u = rotate(u, r, 1);
		l = rotate(l, r, 1);
		break;
	case '5':
		u = rotate(u, l, -1);
		r = rotate(r, l, -1);
		break;
	case '6':
		u = rotate(u, l, 1);
		r = rotate(r, l, 1);
		break;
	default:
		break;
	}
}


void specialKeyListener(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_DOWN:
		p = sub(p, l);
		break;
	case GLUT_KEY_UP:
		p = add(p, l);
		break;
	case GLUT_KEY_RIGHT:
		p = sub(p, r);
		break;
	case GLUT_KEY_LEFT:
		p = add(p, r);
		break;
	case GLUT_KEY_PAGE_UP:
		//p = add(p, u);
		speed_mult = min(speed_mult+0.25, 100.0);
		break;
	case GLUT_KEY_PAGE_DOWN:
		//p = sub(p, u);
		speed_mult = max(speed_mult-0.25, -100.0);
		break;
	default:
		break;
	}
}


void mouseListener(int button, int state, int x, int y) {	//x, y is the x-y of the screen (2D)
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			drawaxes = 1 - drawaxes;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		//........
		break;

	case GLUT_MIDDLE_BUTTON:
		//........
		break;

	default:
		break;
	}
}



void display() {

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	gluLookAt(p.x, p.y, p.z, p.x + l.x, p.y + l.y, p.z + l.z, u.x, u.y, u.z);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	online();




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate() {
	angle += 3;
	speed += 2*speed_mult;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init() {
	//codes for initialization

	p = { 0.0, 0.0, 200.0 };
	l = { 0.0, 0.0, -1.0 };
	r = { 1.0, 0.0, 0.0 };
	u = { 0.0, 1.0, 0.0 };

	drawgrid = 0;
	drawaxes = 1;
	cameraHeight = 150.0;
	cameraAngle = 1.0;
	angle = 0;
	speed = 0;
	speed_mult = 0;

	//clear the screen
	glClearColor(0, 0, 0, 0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80, 1, 1, 1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
