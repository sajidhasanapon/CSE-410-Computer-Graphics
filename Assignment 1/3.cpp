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
	double x,y,z;
};

struct point p, l, r, u;
double rUpper, rLower, rFinger;
double shoulderAngleX, elbowAngleX, shoulderAngleY, palmAngle, fingerAngle1, fingerAngle2;
double scaleUpper, scaleLower, scaleFinger;
double numWires;
double a;





void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawTriangle()
{
    glColor3f(1,1,1);
	glBegin(GL_TRIANGLES);{
		glVertex3f( 0, 0, 0);
		glVertex3f( 12/2, 0,-(sqrt(3)/2)*12);
		glVertex3f(-12/2, 0,-(sqrt(3)/2)*12);
	}glEnd();
}

void rotateShoulderX(double sign)
{
	if (shoulderAngleX >= -45 && sign == -1)
	{
		shoulderAngleX -= 3;
	}

	else if (shoulderAngleX < 45 && sign == 1)
	{
		shoulderAngleX += 3;
	}
}

void rotateShoulderY(double sign)
{
	if (shoulderAngleY < 90 && sign == 1)
	{
		shoulderAngleY += 3;
	}
	else if(shoulderAngleY > 0 && sign == -1)
		{
			shoulderAngleY -= 3;
		}
}

void rotateElbowX(double sign)
{

	if (elbowAngleX > -90 && sign == -1)
	{
		elbowAngleX -= 3;
	}
	else if (elbowAngleX < 0 && sign == 1)
	{
		elbowAngleX += 3;
	}
}

void rotatePalm(double sign)
{

	if (palmAngle < 45 && sign == 1)
	{
		palmAngle += 3;
	}

	else if (palmAngle > -45 && sign == -1)
	{
		palmAngle -= 3;
	}
}

void rotateFinger1(double sign)
{
	if (fingerAngle1 < 90 && sign == 1)
	{
		fingerAngle1 += 3;
	}
	else if (fingerAngle1 > -90 && sign == -1)
	{
		fingerAngle1 -= 3;
	}
}

void rotateFinger2(double sign)
{
	if (fingerAngle2 > -90 && sign == -1)
	{
		fingerAngle2 -= 3;
	}
	else if (fingerAngle2 < 0 && sign == 1)
	{
		fingerAngle2 += 3;
	}
}


void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			rotateShoulderX(-1);
			break;

        case '2':
			rotateShoulderX(1);
			break;

        case '3':
			rotateElbowX(-1);
			break;

        case '4':
			rotateElbowX(1);
			break;

        case '5':
			rotatePalm(1);
            break;

        case '6':
			rotatePalm(-1);
            break;

        case '7':
			rotateFinger1(1);
            break;

        case '8':
			rotateFinger1(-1);
            break;

        case '9':
			rotateFinger2(-1);
            break;

        case '0':
			rotateFinger2(1);
            break;

        case 'q':
			rotateShoulderY(1);
            break;

        case 'w':
			rotateShoulderY(-1);
            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			p.z -= 5;
			break;
		case GLUT_KEY_UP:		// up arrow key
			p.z += 5;
			break;

		case GLUT_KEY_RIGHT:
			p =	{p.x * cos(2 * pi / 180) - p.y * sin(2 * pi / 180),
				 p.x * sin(2 * pi / 180) + p.y * cos(2 * pi / 180),
				 p.z
				};
			break;

		case GLUT_KEY_LEFT:
			p = {p.x * cos(-2 * pi / 180) - p.y * sin(-2 * pi / 180),
				 p.x * sin(-2 * pi / 180) + p.y * cos(-2 * pi / 180),
				 p.z
				};
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


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
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



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
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
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);
    gluLookAt(p.x, p.y, p.z,        	0, 0, 0,    	   u.x, u.y, u.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	glTranslated(0, 0, 50);
	drawAxes();
	
	glRotated(shoulderAngleY, 0, 1, 0);
	glRotated(shoulderAngleX, 1, 0, 0);

	glTranslated(0, 0, -scaleUpper * rUpper);
	glScaled(1, 1, scaleUpper);
	glutWireSphere(rUpper, numWires, numWires);

	glScaled(1, 1, 1 / scaleUpper);
	glTranslated(0, 0, scaleUpper*rUpper);

	glTranslated(0, 0, -2 * scaleUpper*rUpper);
	glRotated(elbowAngleX, 1, 0, 0);
	glTranslated(0, 0, -scaleLower * rLower);
	glScaled(1, 1, scaleLower);
	glutWireSphere(rLower, numWires, numWires);

	glScaled(1, 1, 1 / scaleLower);
	glTranslated(0, 0, scaleLower*rLower + 2 * scaleUpper*rUpper);

	glTranslated(0, 0, -2 * scaleLower*rLower - 2 * scaleUpper*rUpper);
	glRotated(palmAngle, 0, 0, 1);
	drawTriangle();

	glTranslated(0, 0, 2 * scaleLower*rLower + 2 * scaleUpper*rUpper);

	glPushMatrix();
	{
		glTranslated(-12 / 2, 0, -scaleFinger * rFinger - 2 * scaleLower*rLower - 2 * scaleUpper*rUpper - (sqrt(3) / 2) * 12);
		glRotated(fingerAngle1, 0, 0, 1);
		glScaled(1, 1, scaleFinger);
		glutWireSphere(rFinger, numWires / 2, numWires / 2);
	}
	glPopMatrix();

	glTranslated(12 / 2, 0, -2 * scaleLower*rLower - 2 * scaleUpper*rUpper - (sqrt(3) / 2) * 12);
	glRotated(fingerAngle2, 1, 0, 0);
	glTranslated(0, 0, -scaleFinger * rFinger);
	glScaled(1, 1, scaleFinger);
	glutWireSphere(rFinger, 8, 8);
	


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}



void animate(){
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization

	shoulderAngleX = 0;
	elbowAngleX = 0;
	shoulderAngleY = 0;
	palmAngle = 0;
	fingerAngle1 = 0;
	fingerAngle2 = 0;

	rUpper = 6;
	scaleUpper = 4;

	rLower = 5;
	scaleLower = 5;

	rFinger = 2;
	scaleFinger = 2;

	numWires = 10;

	p = {70, 70, 70};

	l = {-1/sqrt(2), -1/sqrt(2), 0};
	r = {-1/sqrt(2), 1/sqrt(2), 0};
    u = {0, 0, 1};

	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
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
