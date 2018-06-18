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

    Point operator - (Point p)
    {return Point(this->x - p.x, this->y - p.y, this->z - p.z);}

    friend Point cross(Point p1, Point p2)
    {
        double a = p1.y * p2.z - p1.z * p2.y;
        double b = p1.z * p2.x - p1.x * p2.z;
        double c = p1.x * p2.y - p1.y * p2.x;

        return Point(a, b, c);
    }
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

    Cell(Color color, double z)
    {this->color = color, this->z = z;}

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
    // let the equation of the plane of the triangle be ax + by + cy + d = 0
    double a, b, c, d;

    Triangle(Point p1, Point p2, Point p3, Color color)
    {
        this->p1 = p1; this->p2 = p2; this->p3 = p3;
        this -> color = color;

        // calculating a, b, c, d
        // using parametric form
        Point A = p1 - p3;
        Point B = p2 - p3;

        Point V = cross(A, B);

        this->a = V.x;
        this->b = V.y;
        this->c = V.z;
        this->d = -(this->a*p3.x + this->b*p3.y + this->c*p3.z);
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

    Cell** z_buffer = new Cell*[int(screen_height)];
    for (int i = 0; i < screen_height; i++)
    {
        z_buffer[i] = new Cell[int(screen_width)];
        for (int j = 0; j < screen_width; j++)
        {
            z_buffer[j][i] = Cell(Color(0.0, 0.0, 0.0), z_rear_limit);
        }
    }


    double dx = (x_right_limit - x_left_limit) / screen_width;
    double dy = (y_top_limit - y_bottom_limit) / screen_height;

    double left_x = x_left_limit + dx / 2.0;
    double top_y = y_top_limit - dy / 2.0;

    for (int t = 0; t < triangles.size(); t++)
    {

        Triangle tr = triangles[t];

        // let, the equation of the triangle (plane of the triangle) be
        // ax + by + cz + d = 0
        // the coefficients are determined when a triangle is constructed

        double x0, x1, x2, y0, y1, y2;
        if(tr.p1.y >= tr.p2.y && tr.p1.y >= tr.p3.y)
        {
            x0 = tr.p1.x; y0 = tr.p1.y; 
            x1 = tr.p2.x; y1 = tr.p2.y;
            x2 = tr.p3.x; y2 = tr.p3.y;
        }
        else if(tr.p2.y >= tr.p1.y && tr.p2.y >= tr.p3.y)
        {
            x0 = tr.p2.x; y0 = tr.p2.y; 
            x1 = tr.p1.x; y1 = tr.p1.y;
            x2 = tr.p3.x; y2 = tr.p3.y;
        }
        else
        {
            x0 = tr.p3.x; y0 = tr.p3.y; 
            x1 = tr.p1.x; y1 = tr.p1.y;
            x2 = tr.p2.x; y2 = tr.p2.y;
        }
    }
}