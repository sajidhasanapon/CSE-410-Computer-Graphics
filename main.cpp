#include <bits/stdc++.h>
#include <windows.h>
#include "openGL.hpp"

using namespace std;

#define pi (2*acos(0.0))
int recursion_level;

#include "bitmap_image.hpp"
#include "base.hpp"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define VIEW_ANGLE 80

Point3 eye, l, r, u;
int imageWidth, imageHeight;




void loadTestData() {

    imageWidth = imageHeight = 768;

    recursion_level = 3;

    Point3 center(20,20,10);
    double radius = 10;

    Object *temp;

    temp = new Sphere(center, radius);
    temp->setColor(1,0,0);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);

    center = {0,15,15};
    radius = 5;

    temp = new Sphere(center, radius);
    temp->setColor(1,0,1);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    // objects.push_back(temp);


    Point3 a(20.0, 20.0, 20.0);
    Point3 b(40.0, 30.0, 10.0);
    Point3 c(50.0, 40.0, 0.0);

    temp = new Triangle(a, b, c);
    temp->setColor(0, 1, 0);
    temp->setCoEfficients(0.4, 0.2, 0.1, 0.3);
    temp->setShine(5);
    // objects.push_back(temp);

    Point3 p1(-20.0, 20.0, 20.0);
    Point3 p2(40.0, 30.0, 10.0);
    Point3 p3(50.0, 40.0, 0.0);

    temp = new Triangle(p1, p2, p3);
    temp->setColor(0, 1, 0);
    temp->setCoEfficients(0.4, 0.2, 0.1, 0.3);
    temp->setShine(5);
    objects.push_back(temp);


    double coeff[] = {1, 1, 1, 0, 0, 0, -20, -20, -20, 200};
    Point3 reff(0, 0, 0);

    temp = new GeneralQuadratic(coeff, reff, 0, 0, 5);
    temp->setColor(1, 0, 1);
    temp->setCoEfficients(0.4, 0.2, 0.1, 0.3);
    temp->setShine(3);
    //objects.push_back(temp);


    Point3 light1(-50,50,50);
    lights.push_back(light1);

    temp = new Floor(1000, 20);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);

}


void loadActualData() {

    freopen("scene.txt", "r", stdin);

    cin>>recursion_level;
    cin>>imageWidth;
    imageHeight = imageWidth;

    int numOfObjects;
    cin>>numOfObjects;

    string command;
    double a, b, c, radius;

    Object *temp;

    for (int i=0; i<numOfObjects; i++) {
        cin>>command;

        if (command == "sphere") {

            cin>>a>>b>>c;
            Point3 center(a, b, c);

            cin>>radius;
            temp = new Sphere(center, radius);

            cin>>a>>b>>c;
            temp->setColor(a, b, c);

            cin>>a>>b>>c>>radius;
            temp->setCoEfficients(a, b, c, radius);

            cin>>a;
            temp->setShine(a);

            objects.push_back(temp);
        }

        else if (command == "triangle") {

            cin>>a>>b>>c;
            Point3 A(a, b, c);

            cin>>a>>b>>c;
            Point3 B(a, b, c);

            cin>>a>>b>>c;
            Point3 C(a, b, c);

            temp = new Triangle(A, B, C);

            cin>>a>>b>>c;
            temp->setColor(a, b, c);

            cin>>a>>b>>c>>radius;
            temp->setCoEfficients(a, b, c, radius);

            cin>>a;
            temp->setShine(a);

            objects.push_back(temp);

        }

        else if (command == "general") {

            double coeff[10];
            for (int c=0; c<10; c++) {
                cin>>coeff[c];
            }

            cin>>a>>b>>c;
            Point3 reff(a, b, c);

            cin>>a>>b>>c;
            temp = new GeneralQuadratic(coeff, reff, a, b, c);

            cin>>a>>b>>c;
            temp->setColor(a, b, c);

            cin>>a>>b>>c>>radius;
            temp->setCoEfficients(a, b, c, radius);

            cin>>a;
            temp->setShine(a);

            objects.push_back(temp);

        }

    }

    cin>>numOfObjects; // numOfLights ???
    for (int i=0; i<numOfObjects; i++) {
        cin>>a>>b>>c;

        Point3 light(a, b, c);
        lights.push_back(light);
    }


    temp = new Floor(1000, 20);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);


}

void capture() {

    eye.print();
    Point3** frameBuffer;
    frameBuffer = new Point3* [imageWidth];
    Point3 color(0, 0, 0);
    for (int i=0; i<imageWidth; i++) {
        frameBuffer[i] = new Point3[imageHeight];
        for (int j=0; j<imageHeight; j++) {
            frameBuffer[i][j] = color;
        }
    }



    double planeDistance = (WINDOW_HEIGHT/2)/tan(VIEW_ANGLE*pi/360);

    Point3 topLeft = eye + (l * planeDistance - r * (WINDOW_WIDTH/2) + u * (WINDOW_HEIGHT/2));
    topLeft.print();

    double du = (WINDOW_WIDTH*1.0) / imageWidth;
    double dv = (WINDOW_HEIGHT*1.0) / imageHeight;

    //cout<<du<<','<<dv<<endl;

    for (int i=0; i<imageWidth; i++) {
        for (int j=0; j<imageHeight; j++) {

            Point3 cornerDir = topLeft + r*i*du - u*j*dv;
            //cout<<cornerDir;

            Ray ray(eye, cornerDir - eye);

            int nearest=-1;
            double t_min = 9999999;
            double dummy_color[3] = {0,0,0};

            for (int k=0; k < objects.size(); k++)
            {
                double t = objects[k]->getIntersectionT(&ray, false);

                if(t <= 0) continue;
                else if(t < t_min)
                {
                    t_min = t;
                    nearest = k;
                }
            }

            if(nearest!=-1) {
                double t = objects[nearest]->intersect(&ray, dummy_color, 1);
                frameBuffer[i][j].x = dummy_color[0];
                frameBuffer[i][j].y = dummy_color[1];
                frameBuffer[i][j].z = dummy_color[2];
            }
        }
    }



    bitmap_image image(imageWidth, imageHeight);

    for (int i=0; i<imageWidth; i++) {
        for (int j=0; j<imageHeight; j++) {
            image.set_pixel(i, j, frameBuffer[i][j].x*255, frameBuffer[i][j].y*255, frameBuffer[i][j].z*255);
        }
    }

    image.save_image("output.bmp");

    cout<<"saved\n";
    exit(0);
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
    case '0':
        capture();
        break;
	default:
		break;
	}
}


void specialKeyListener(int key, int x,int y)
{
    switch(key) {
    case GLUT_KEY_DOWN:
		eye = eye - l;
		break;
	case GLUT_KEY_UP:
		eye = eye + l;
		break;
	case GLUT_KEY_RIGHT:
		eye = eye + r;
		break;
	case GLUT_KEY_LEFT:
		eye= eye - r;
		break;
	case GLUT_KEY_PAGE_UP:
		eye = eye + u;
		break;
	case GLUT_KEY_PAGE_DOWN:
		eye = eye - u;
		break;
	case GLUT_KEY_INSERT:
		break;

    default:
        break;
    }
}


void display()
{

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
    gluLookAt(eye.x, eye.y, eye.z, eye.x + l.x, eye.y + l.y, eye.z + l.z, u.x, u.y, u.z);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    drawAxes();

    for (int i=0; i < objects.size(); i++)
        objects[i]->draw();

    for (int i=0; i < lights.size(); i++)
        drawLightSource(lights[i].x, lights[i].y, lights[i].z);

    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate()
{
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init() {

    eye = {100, 100, 10};
    l = { 0.0, 1.0, 0.0 };
	r = { 1.0, 0.0, 0.0 };
	u = { 0.0, 0.0, 1.0 };


    //codes for initialization
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
    gluPerspective(VIEW_ANGLE,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance

}

void freeMemory() {
    vector<Point3>().swap(lights);
    vector<Object*>().swap(objects);
}


int main(int argc, char **argv) {

    //freopen("out.txt", "w", stdout);
    loadTestData();
    // loadActualData();

    glutInit(&argc,argv);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("Ray Tracer");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);

    glutMainLoop();		//The main loop of OpenGL

    freeMemory();

    return 0;
}
