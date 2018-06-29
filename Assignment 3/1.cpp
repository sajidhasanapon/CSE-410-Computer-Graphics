#include <bits/stdc++.h>
#include "bitmap_image.hpp"

using namespace std;


double screen_width,    screen_height;
double x_left_limit,    x_right_limit;
double y_bottom_limit,  y_top_limit;
double z_rear_limit,    z_front_limit;

double dx, dy;




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


int map_y_to_grid_row(double y)
{
    double r = (y - y_bottom_limit) / dy + 0.5;
    return int(r);
}

double map_row_to_y(int r)
{
    return double(r)*dy + y_bottom_limit;
}

int map_x_to_grid_col(double x)
{
    double c = (x - x_left_limit) / dx + 0.5;
    return int(c);
}

double map_col_to_x(int c)
{
    return double(c)*dx + x_left_limit;
}

pair<double, double> get_intersetion_x(Triangle tr, double y)
{
    double x1 = tr.p1.x;
    double y1 = tr.p1.y;

    double x2 = tr.p2.x;
    double y2 = tr.p2.y;

    double x3 = tr.p3.x;
    double y3 = tr.p3.y;

    double res_x1, res_x2;

    if(y == y1 && y == y2) return make_pair(min(x1, x2), max(x1, x2));
    if(y == y1 && y == y3) return make_pair(min(x1, x3), max(x1, x3));
    if(y == y2 && y == y3) return make_pair(min(x2, x3), max(x2, x3));

    if( (y2 - y) * (y3 - y) >= 0) // p2 and p3 on the same side
    {
        res_x1 = (x1-x2)/(y1-y2)*(y-y1) + x1; // p1, p2
        res_x2 = (x1-x3)/(y1-y3)*(y-y1) + x1; // p1, p3
        return make_pair(min(res_x1, res_x2), max(res_x1, res_x2));
    }
    if( (y1 - y) * (y3 - y) >= 0) // p1 and p3 on the same side
    {
        res_x1 = (x1-x2)/(y1-y2)*(y-y1) + x1; // p2, p1
        res_x2 = (x2-x3)/(y2-y3)*(y-y2) + x2; // p2, p3
        return make_pair(min(res_x1, res_x2), max(res_x1, res_x2));
    }
    if( (y1 - y) * (y2 - y) >= 0) // p1 and p2 on the same side
    {
        res_x1 = (x1-x3)/(y1-y3)*(y-y1) + x1; // p3, p1
        res_x2 = (x2-x3)/(y2-y3)*(y-y2) + x2; // p3, p2
        return make_pair(min(res_x1, res_x2), max(res_x1, res_x2));
    }
        
}

int main()
{
    ifstream config("config.txt");
    config >> screen_width >> screen_height;
    config >> x_left_limit;     x_right_limit = - x_left_limit;
    config >> y_bottom_limit;   y_top_limit = - y_bottom_limit;
    config >> z_front_limit >> z_rear_limit;
    config.close();

    dx = (x_right_limit - x_left_limit) / screen_width;
    dy = (y_top_limit - y_bottom_limit) / screen_height;

    ifstream input("stage3.txt");
    vector<Triangle> triangles;
    vector<Triangle>emptyVector;

    double x, y, z;
    while(input >> x >> y >> z)
    {
        Point p1(x, y, z);
        input >> x >> y >> z; Point p2(x, y, z);
        input >> x >> y >> z; Point p3(x, y, z);

        //srand(time(NULL));
        Color random_color(rand()%256, rand()%256, rand()%256);

        Triangle triangle(p1, p2, p3, random_color);

        triangles.push_back(triangle);

        //triangle.print();
    }
    input.close();

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

    cout << "scanning" << endl << endl;
    for (int t = 0; t < triangles.size(); t++)
    {
        Triangle tr = triangles[t];

        double tr_max_y = max(max(tr.p1.y, tr.p2.y), tr.p3.y); // max_y of triangle
        int r_up = map_y_to_grid_row(tr_max_y);
        int upper_scanline_row = min(r_up, int(screen_height));
        double upper_scanline = map_row_to_y(upper_scanline_row);

        double tr_min_y = min(min(tr.p1.y, tr.p2.y), tr.p3.y); // max_y of triangle
        int r_down = map_y_to_grid_row(tr_min_y);
        int lower_scanline_row = max(r_down, 0);
        double lower_scanline = map_row_to_y(lower_scanline_row);

        int c, r;
        double x, y;
        for(r = lower_scanline_row, y = lower_scanline + dy / 2.0; y <= upper_scanline; r++, y += dy)  
        {
            pair<double, double> intersection_x = get_intersetion_x(tr, y);
            double x1 = intersection_x.first;
            double x2 = intersection_x.second;

            int c_left = map_x_to_grid_col(x1);
            int left_scanline_col = max(c_left, 0);
            double left_scanline = map_col_to_x(left_scanline_col);

            int c_right = map_x_to_grid_col(x2);
            int right_scanline_col = min(c_right, int(screen_width));
            double right_scanline = map_col_to_x(right_scanline_col);

            for(c = left_scanline_col, x = left_scanline + dx / 2.0; x <= right_scanline; c++, x += dx)
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
    vector<Triangle>().swap(triangles);
    triangles.clear();
    triangles.shrink_to_fit();
    
    cout << "Creating image..." << endl << endl;
    int sw = int(screen_width);
    int sh = int(screen_height);
    bitmap_image image(sw, sh);

    for(int i=0; i<sw; i++){
        for(int j=0; j<sh; j++){
            image.set_pixel(i, sh-j, z_buffer[i][j].color.r, z_buffer[i][j].color.g, z_buffer[i][j].color.b);
        }
    }
    cout << "Success!" << endl;

    image.save_image("1.bmp");

    return 0;
}