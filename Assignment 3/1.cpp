#include <bits/stdc++.h>
#include "bitmap_image.hpp"

using namespace std;

class Point
{
public:
    double x, y, z;


    Point(){}

    Point(double x, double y, double z)
    {this->x = x; this->y = y; this->z = z;}
};

class Color
{
public:
    double r, g, b;


    Color(){}

    Color(double r, double g, double b)
    {this->r = r; this->g = g; this->b = b;}

    void print()
    {
        cout << "RGB : " << r << " " << g << " " << b << endl;
    }
};

class Cell
{
public:
    Color color;
    double z;

    Cell(){}

    void set_color(Color color)
    {this->color = color;}

    void set_z(double z)
    {this->z = z;}
};

class Triangle
{
public:
    Point p1, p2, p3;
    Color color;

    Triangle(){}

    Triangle(Point p1, Point p2, Point p3)
    {this->p1 = p1; this->p2 = p2; this->p3 = p3;}

    Triangle(Point p1, Point p2, Point p3, Color color)
    {
        this->p1 = p1; this->p2 = p2; this->p3 = p3;
        this -> color = color;
    }

    void print()
    {
        
        cout << "Vertex 1 : " << p1.x << " " << p1.y << " " << p1.z << endl;
        cout << "Vertex 2 : " << p2.x << " " << p2.y << " " << p2.z << endl;
        cout << "Vertex 3 : " << p3.x << " " << p3.y << " " << p3.z << endl;
        color.print();
        cout << endl;
    }
};



int main()
{
    double screen_width,    screen_height;
    double x_left_limit,    x_right_limit;
    double y_bottom_limit,  y_top_limit;
    double z_rear_limit,    z_front_limit;

    ifstream config("config.txt");
    config >> screen_width >> screen_height;
    config >> x_left_limit;     x_right_limit = - x_left_limit;
    config >> y_bottom_limit;   y_top_limit = - y_bottom_limit;
    config >> z_front_limit >> z_rear_limit;

    ifstream input("stage3.txt");
    vector<Triangle> triangles;

    double x, y, z;
    while(input >> x >> y >> z)
    {
        Point p1(x, y, z);
        input >> x >> y >> z; Point p2(x, y, z);
        input >> x >> y >> z; Point p3(x, y, z);

        Color random_color(rand()%256, rand()%256, rand()%256);

        Triangle triangle(p1, p2, p3, random_color);

        triangles.push_back(triangle);

        triangle.print();
    }
}