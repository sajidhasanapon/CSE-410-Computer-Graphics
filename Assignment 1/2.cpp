#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct point
{
	double x, y, z;
};

double bx=0, by=0, vx=1, vy=0, ex, ey, px, py, qx, qy, sx, sy;

void drawBoundary()
{

	glColor3f(1, 1, 1);

	glBegin(GL_LINES); {
		glVertex3f(100.0, 100.0, 0.0);
		glVertex3f(-100.0, 100.0, 0.0);

		glVertex3f(-100.0, 100.0, 0.0);
		glVertex3f(-100.0, -100.0, 0.0);

		glVertex3f(-100.0, -100.0, 0.0);
		glVertex3f(100.0, -100.0, 0.0);

		glVertex3f(100.0, -100.0, 0.0);
		glVertex3f(100.0, 100.0, 0.0);
	}glEnd();
}

void drawArrow()
{
	glColor3f(0.7, 0.7, 0.7);

	glBegin(GL_LINES); {
		glVertex3f(bx, by, 0);
		glVertex3f(bx + 20 * vx, by + 20 * vy, 0);
	}glEnd();

	glBegin(GL_TRIANGLES);
	{
		glVertex3f(px, py, 0);
		glVertex3f(qx, qy, 0);
		glVertex3f(sx, sy, 0);
	}
	glEnd();
}

void rotate(double sign)
{
	double angle = pi * 3.0 / 180.0 * sign;

	double tx = vx * cos(angle) - vy * sin(angle);
	double ty = vx * sin(angle) + vy * cos(angle);

	vx = tx;
	vy = ty;
}

//////////////////////////////////////////////////////

void keyboardListener(unsigned char key, int x, int y) {
	switch (key) {

	case '1':
		drawgrid = 1 - drawgrid;
		break;

	default:
		break;
	}
}


void specialKeyListener(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_DOWN:		//down arrow key
		cameraHeight -= 3.0;
		break;
	case GLUT_KEY_UP:		// up arrow key
		cameraHeight += 3.0;
		break;

	case GLUT_KEY_RIGHT:
		rotate(-1);
		break;
	case GLUT_KEY_LEFT:
		rotate(1);
		break;

	case GLUT_KEY_PAGE_UP:
		break;
	case GLUT_KEY_PAGE_DOWN:
		break;

	case GLUT_KEY_INSERT:
		break;

	case GLUT_KEY_HOME:
		break;
	case GLUT_KEY_END:
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
	//gluLookAt(200 * cos(cameraAngle), 200 * sin(cameraAngle), cameraHeight, 0, 0, 0, 0, 0, 1);
	gluLookAt(0, 0, 150, 0, 0, 0, 0, 1, 0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	//drawAxes();
	//drawGrid();

	drawBoundary();
	drawArrow();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate()
{
	bx = bx + 0.06 * vx;
	by = by + 0.06 * vy;

	if (bx >= 100 || bx <= -100) vx = -vx;
	if (by >= 100 || by <= -100) vy = -vy;

	px = bx + 20 * vx - 5 * vy;
	py = by + 20 * vy + 5 * vx;

	qx = bx + 20 * vx + 5 * vy;
	qy = by + 20 * vy - 5 * vx;

	sx = bx + 20 * vx + 5 * vx;
	sy = by + 20 * vy + 5 * vy;

	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init() {
	//codes for initialization

	ex = bx + 25 * vx;
	ey = by + 25 * vx;

	drawgrid = 0;
	drawaxes = 1;
	cameraHeight = 150.0;
	cameraAngle = 1.0;
	angle = 0;

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


///////////////////////////////////////////////////////////////////////////

