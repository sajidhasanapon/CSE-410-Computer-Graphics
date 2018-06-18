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


double area(double x1, double y1, double x2, double y2, double x3, double y3)
{
   return abs((x1*(y2-y3) + x2*(y3-y1)+ x3*(y1-y2))/2.0);
}

bool is_inside_triangle(Triangle tr, double x, double y)
{
    double A = area(tr.p1.x, tr.p1.y, tr.p2.x, tr.p2.y, tr.p3.x, tr.p3.y);

    double A1 = area(x, y, tr.p2.x, tr.p2.y, tr.p3.x, tr.p3.y);
    double A2 = area(tr.p1.x, tr.p1.y, x, y, tr.p3.x, tr.p3.y);
    double A3 = area(tr.p1.x, tr.p1.y, tr.p2.x, tr.p2.y, x, y);

    return (A == A1 + A2 + A3);
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

        Color random_color(rand()%256, rand()%256, rand()%256);

        Triangle triangle(p1, p2, p3, random_color);

        triangles.push_back(triangle);

        //triangle.print();
    }

    cout << "Creating z_buffer" << endl << endl;
    Cell z_buffer[int(screen_width)][int(screen_height)];
    for (int i = 0; i < screen_width; i++)
    {
        for (int j = 0; j < screen_height; j++)
        {
            z_buffer[i][j].set_color(Color(0.0, 0.0, 0.0));
            z_buffer[i][j].set_z(z_rear_limit);
        }
    }
    

    // Cell** z_buffer = new Cell*[int(screen_height)];
    // for (int i = 0; i < screen_height; i++)
    // {
    //     z_buffer[i] = new Cell[int(screen_width)];
    //     for (int j = 0; j < screen_width; j++)
    //     {
    //         z_buffer[j][i] = Cell(Color(0.0, 0.0, 0.0), z_rear_limit);
    //     }
    // }


    double dx = (x_right_limit - x_left_limit) / screen_width;
    double dy = (y_top_limit - y_bottom_limit) / screen_height;

    cout << "scanning" << endl << endl;

    for (int t = 0; t < triangles.size(); t++)
    {
        Triangle tr = triangles[t];

        // double tr_max_y = max(max(tr.p1.y, tr.p2.y), tr.p3.y); // max_y of triangle
        // double max_y = min(y_top_limit, tr_max_y + dy / 2.0);
        // int top_cell = (max_y - y_bottom_limit) / dy;
        // double top_y = (top_cell)*dx + y_bottom_limit - dy/2.0;

        // double tr_min_y = min(min(tr.p1.y, tr.p2.y), tr.p3.y); // min_y of triangle
        // double min_y = max(y_bottom_limit, tr_min_y);

        // int c, r;
        // double x, y;
        // for(r = top_cell - 1, y = top_y; y >= min_y; r--, y -= dy)
        // {
        //     for(c = 0, x = x_left_limit + dx / 2.0; x <= x_right_limit; c++, x += dx)
        //     {
        //         if(is_inside_triangle(tr, x, y) == true)
        //         {
        //             double z = -(tr.a*x + tr.b*y + tr.d) / tr.c;
        //             if(z < z_buffer[c][r].z)
        //             {
        //                 z_buffer[c][r].set_color(tr.color);
        //                 z_buffer[c][r].set_z(z);
        //             }
        //         }
        //     }
        // }


        int c, r;
        double x, y;
        for(r = screen_height-1, y = y_top_limit - dy / 2.0; y >= y_bottom_limit; r--, y -= dy)
        {
            for(c = 0, x = x_left_limit + dx / 2.0; x <= x_right_limit; c++, x += dx)
            {
                if(is_inside_triangle(tr, x, y) == true)
                {
                    double z = -(tr.a*x + tr.b*y + tr.d) / tr.c;
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
    bitmap_image image(500,300);

    for(int i=0; i<200; i++){
        for(int j=0; j<100; j++){
            image.set_pixel(i,j,255,255,0);
        }
    }

    image.save_image("test.bmp");
}