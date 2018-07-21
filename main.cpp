#include <bits/stdc++.h>
#include <windows.h>
#include "include/glut.h"
using namespace std;

#define pi (2*acos(0.0))

int recursion_level;

#include "bitmap_image.h"
#include "lib.h"

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700
#define VIEW_ANGLE 50

Point3 eye, l, r, u;
int image_width, image_height;

void create_floor()
{
    Object *floor = new Floor(1000, 20);
    floor->set_lighting_coefficients(0.3, 0.3, 0.3, 0.1);
    floor->setShine(1.0);
    objects.push_back(floor);
}

void loadTestData()
{
    image_width = image_height = 768;
    recursion_level = 3;
}   

void loadActualData() {

    int n_objects, n_lights;
    string object_type;
    Object *obj;

    freopen("scene.txt", "r", stdin);

    cin >> recursion_level >> image_width >> n_objects;
    image_height = image_width;

    for (int i = 0; i < n_objects; i++)
    {
        cin >> object_type;

        if (object_type == "sphere") {

            double x, y, z; // center
            double radius; // radius
            double red, green, blue;
            double ambient, diffuse, specular, reflection;
            double shine; 

            Point3 center;

            cin >> x >> y >> z >> radius;
            center = Point3(x, y, z);
            obj = new Sphere(center, radius);

            cin >> red >> green >> blue;
            obj->setColor(red, green, blue);

            cin >> ambient >> diffuse >> specular >> reflection;
            obj->set_lighting_coefficients(ambient, diffuse, specular, reflection);

            cin >> shine;
            obj->setShine(shine);

            obj->eta = 10.0;

            objects.push_back(obj);
        }

        else if (object_type == "triangle") {

            double x, y, z; // a vertex
            double red, green, blue;
            double ambient, diffuse, specular, reflection;
            double shine;

            Point3 A, B, C;

            cin >> x >> y >> z;
            A = Point3(x, y, z);

            cin >> x >> y >> z;
            B = Point3(x, y, z);

            cin >> x >> y >> z;
            C = Point3(x, y, z);

            obj = new Triangle(A, B, C);

            cin >> red >> green >> blue;
            obj->setColor(red, green, blue);

            cin >> ambient >> diffuse >> specular >> reflection;
            obj->set_lighting_coefficients(ambient, diffuse, specular, reflection);

            cin >> shine;
            obj->setShine(shine);

            objects.push_back(obj);

        }

        else if (object_type == "general") {

            double coeff[10];
            double x, y, z;
            double length, width, height;
            double red, green, blue;
            double ambient, diffuse, specular, reflection;
            double shine;

            Point3 base_point;

            for (int c = 0; c < 10; c++) {
                cin>>coeff[c];
            }

            cin >> x >> y >> z;
            base_point = Point3(x, y, z);

            cin >> length >> width >> height;
            obj = new GeneralQuadratic(coeff, base_point, length, width, height);

            cin >> red >> green >> blue;
            obj->setColor(red, green, blue);

            cin >> ambient >> diffuse >> specular >> reflection;
            obj->set_lighting_coefficients(ambient, diffuse, specular, reflection);

            cin >> shine;
            obj->setShine(shine);

            objects.push_back(obj);
        }
    }

    cin >> n_lights;
    for (int i = 0; i < n_lights; i++) {
        double x, y, z;
        cin >> x >> y >> z;

        Point3 light(x, y, z);
        lights.push_back(light);
    }
}

void capture() {

    Color** frameBuffer;
    frameBuffer = new Color* [image_width];
    for (int i=0; i<image_width; i++) {
        frameBuffer[i] = new Color[image_height];
    }

    double plane_distance = (WINDOW_HEIGHT/2)/tan(VIEW_ANGLE*pi/360);
    Point3 top_left = eye + (l * plane_distance - r * (WINDOW_WIDTH/2) + u * (WINDOW_HEIGHT/2));

    cout << "Eye : "; eye.print();
    cout << "Plane distance : " << plane_distance << endl;
    cout << "top_left : "; top_left.print();
    cout << "Saving...";

    double du = (WINDOW_WIDTH*1.0) / image_width;
    double dv = (WINDOW_HEIGHT*1.0) / image_height;

    for (int i = 0; i < image_width; i++) {
        for (int j = 0; j < image_height; j++) {

            Point3 direction_to_top_left = top_left + r*i*du - u*j*dv;

            Ray ray(eye, direction_to_top_left - eye);
            double dummy_color[3] = {0.0, 0.0, 0.0};

            pair<double, double> pair = get_nearest(ray);
            int nearest = pair.first;
            double t_min = pair.second;

            if(nearest!=-1) {
                objects[nearest]->fill_color(ray, t_min, dummy_color, 1);
            }
            frameBuffer[i][j].r = dummy_color[0];
            frameBuffer[i][j].g = dummy_color[1];
            frameBuffer[i][j].b = dummy_color[2];
        }
    }

    bitmap_image image(image_width, image_height);

    for (int i=0; i<image_width; i++) {
        for (int j=0; j<image_height; j++) {
            double r = frameBuffer[i][j].r;
            double g = frameBuffer[i][j].g;
            double b = frameBuffer[i][j].b;
            image.set_pixel(i, j, r*255, g*255, b*255);
        }
    }

    image.save_image("out.bmp");

    cout << "\tSaved\n";
    cout << "\a";
}

void freeMemory() {
    vector<Point3>().swap(lights);
    vector<Object*>().swap(objects);
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
		eye = eye - l*3.0;
		break;
	case GLUT_KEY_UP:
		eye = eye + l*3.0;
		break;
	case GLUT_KEY_RIGHT:
		eye = eye + r*3.0;
		break;
	case GLUT_KEY_LEFT:
		eye= eye - r*3.0;
		break;
	case GLUT_KEY_PAGE_UP:
		eye = eye + u*3.0;
		break;
	case GLUT_KEY_PAGE_DOWN:
		eye = eye - u*3.0;
		break;
	case GLUT_KEY_INSERT:
		break;

    default:
        break;
    }
}

void mouseListener(int button, int state, int x, int y) {	//x, y is the x-y of the screen (2D)
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			//drawaxes = 1 - drawaxes;
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
	gluLookAt(eye.x, eye.y, eye.z, eye.x + l.x, eye.y + l.y, eye.z + l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects
    drawAxes();

    for (int i = 0; i < objects.size(); i++)
        objects[i]->draw();

    for (int i = 0; i < lights.size(); i++)
        drawLightSource(lights[i].x, lights[i].y, lights[i].z);

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate() {
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init() {
	//codes for initialization
    eye = {0, -200, 30};
    l = { 0.0, 1.0, 0.0 };
	r = { 1.0, 0.0, 0.0 };
	u = { 0.0, 0.0, 1.0 };

    create_floor();
    //loadTestData();
    loadActualData();

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