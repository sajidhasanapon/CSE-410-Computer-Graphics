#include "include/glut.h"

void drawAxes() {
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    {
        glVertex3f( 100,0,0);
        glVertex3f(-100,0,0);

        glVertex3f(0,-100,0);
        glVertex3f(0, 100,0);

        glVertex3f(0,0, 100);
        glVertex3f(0,0,-100);
    }
    glEnd();
}


void drawSquare(double a) {
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,0);
        glVertex3f( a,-a,0);
        glVertex3f(-a,-a,0);
        glVertex3f(-a, a,0);
    }
    glEnd();
}

void drawLightSource(double x, double y, double z)
{
    glColor3f(1.0, 1.0, 0);
    glPushMatrix();
	glTranslatef (x, y, z);
	glutSolidSphere(1.0, 50, 50);
    glPopMatrix();

    glBegin(GL_LINES);
    {
        for(int i = -1; i <=1; i++)
            for(int j = -1; j <=1; j++)
                for(int k = -1; k <=1; k++)
                {
                    glVertex3f(x, y, z);
                    glVertex3f(x+i*2.0, y+j*2.0, z+k*2.0);
                }
    }
    glEnd();
}