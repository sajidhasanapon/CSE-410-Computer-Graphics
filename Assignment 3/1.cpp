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
    // let the equation of the plane of the triangle be ax + by + cy = d
    double a, b, c, d;

    Triangle(Point p1, Point p2, Point p3, Color color)
    {
        this->p1 = p1; this->p2 = p2; this->p3 = p3;
        this -> color = color;

        // calculating a, b, c, d
        // using parametric form
        Point A = p1 - p2;
        Point B = p1 - p3;

        Point V = cross(A, B);

        this->a = V.x;
        this->b = V.y;
        this->c = V.z;
        this->d = this->a*p1.x + this->b*p1.y + this->c*p1.z;
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

float sign (Point p1, Point p2, Point p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle (Point pt, Triangle tr)
{
    bool b1, b2, b3;

    Point v1 = tr.p1;
    Point v2 = tr.p2;
    Point v3 = tr.p3;

    b1 = sign(pt, v1, v2) < 0.0f;
    b2 = sign(pt, v2, v3) < 0.0f;
    b3 = sign(pt, v3, v1) < 0.0f;

    return ((b1 == b2) && (b2 == b3));
}

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

        srand(time(NULL));
        Color random_color(rand()%256, rand()%256, rand()%256);

        Triangle triangle(p1, p2, p3, random_color);

        triangles.push_back(triangle);

        //triangle.print();
    }

    cout << "Creating z_buffer" << endl << endl;
    Cell **z_buffer = new Cell*[int(screen_width)];
    for(int i = 0; i < screen_height; i++)
    {
        z_buffer[i] = new Cell[int(screen_height)];
        for (int j = 0; j < screen_height; j++)
        {
            z_buffer[i][j].set_color(Color(0.0, 0.0, 0.0));
            z_buffer[i][j].set_z(z_rear_limit);
        }
    }
    cout << "z_buffer creation complete" << endl << endl;

    double dx = (x_right_limit - x_left_limit) / screen_width;
    double dy = (y_top_limit - y_bottom_limit) / screen_height;

    cout << "scanning" << endl << endl;


    for (int t = 0; t < triangles.size(); t++)
    {
        // double tr_max_y = max(max(tr.p1.y, tr.p2.y), tr.p3.y); // max_y of triangle
        // double max_y = min(y_top_limit, tr_max_y + dy / 2.0);
        // int top_cell = (max_y - y_bottom_limit) / dy;
        // double top_y = (top_cell)*dx + y_bottom_limit - dy/2.0;

        // double tr_min_y = min(min(tr.p1.y, tr.p2.y), tr.p3.y); // min_y of triangle
        // double min_y = max(y_bottom_limit, tr_min_y);
        Triangle tr = triangles[t];

        int c, r;
        double x, y;
        for(c = 0, x = x_left_limit + dx / 2.0; x <= x_right_limit; c++, x += dx)
        {
            for(r = 0, y = y_bottom_limit + dy / 2.0; y <= y_top_limit; r++, y += dy)  
            {
                if(PointInTriangle(Point(x,y,0), tr) == true)
                {
                    double z = (tr.d - tr.a*x - tr.b*y ) / tr.c;
                    if(z < z_buffer[c][r].z)
                    {
                        z_buffer[c][r].set_color(tr.color);
                        z_buffer[c][r].set_z(z);
                    }
                }
            }
        }
    }
    cout << "Creating image..." << endl << endl;
    int sw = screen_width;
    int sh = screen_height;
    bitmap_image image(sw, sh);

    for(int i=0; i<sw; i++){
        for(int j=0; j<sh; j++){
            image.set_pixel(i, sh-j, z_buffer[i][j].color.r, z_buffer[i][j].color.g, z_buffer[i][j].color.b);
        }
    }

    image.save_image("test.bmp");

    Point p1 = Point(1,-2,0);
    Point p2 = Point(3,1,4);
    Point p3 = Point(0,-1,2);

    Color random_color(rand()%256, rand()%256, rand()%256);

    Triangle triangle(p1, p2, p3, random_color);

    Triangle t = Triangle(p1,p2,p3, random_color);

    Point res = cross(p1-p2, p1-p3);

    cout << "(" << res.x << "," << res.y << "," << res.z << ")" << endl;
    cout <<  t.a << "," << t.b << "," << t.c << "," << t.d  << endl;
}