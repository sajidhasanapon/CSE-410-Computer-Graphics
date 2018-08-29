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


int get_row_index_from_scanline_y(double y)
{
    double r = (y - y_bottom_limit) / dy - 0.5;
    return int(r);
}

double get_scanline_y_from_row_index(int r)
{
    return double(r)*dy + y_bottom_limit + dy/2.0;
}

int get_column_index_from_scanline_x(double x)
{
    double c = (x - x_left_limit) / dx - 0.5;
    return int(c);
}

double get_scanline_x_from_column_index(int c)
{
    return double(c)*dx + x_left_limit + dx/2.0;
}


void get_intersetion_x(Triangle tr, double y, double& x_left, double& x_right)
{
    // clumsy code, but crazy fast!
    // returning early speeds things up.

    double x1 = tr.p1.x;    double y1 = tr.p1.y;
    double x2 = tr.p2.x;    double y2 = tr.p2.y;
    double x3 = tr.p3.x;    double y3 = tr.p3.y;

    if(y == y1 && y == y2) {x_left = min(x1, x2); x_right = max(x1, x2); return;}
    if(y == y1 && y == y3) {x_left = min(x1, x3); x_right = max(x1, x3); return;}
    if(y == y2 && y == y3) {x_left = min(x2, x3); x_right = max(x2, x3); return;}

    if( (y2 - y) * (y3 - y) >= 0) // p2 and p3 on the same side of scanline y
    {
        double temp_x1 = (x1-x2)/(y1-y2)*(y-y1) + x1; // intersection of p1 and p2
        double temp_x2 = (x1-x3)/(y1-y3)*(y-y1) + x1; // intersection of p1 and p3
        x_left = min(temp_x1, temp_x2);     x_right = max(temp_x1, temp_x2);
        return;
    }
    if( (y1 - y) * (y3 - y) >= 0) // p1 and p3 on the same side of scanline y
    {
        double temp_x1 = (x1-x2)/(y1-y2)*(y-y1) + x1; // intersection of p2 and p1
        double temp_x2 = (x2-x3)/(y2-y3)*(y-y2) + x2; // intersection of p2 and p3
        x_left = min(temp_x1, temp_x2);     x_right = max(temp_x1, temp_x2);
        return;
    }
    if( (y1 - y) * (y2 - y) >= 0) // p1 and p2 on the same side of scanline y
    {
        double temp_x1 = (x1-x3)/(y1-y3)*(y-y1) + x1; // intersection of p3 and p1
        double temp_x2 = (x2-x3)/(y2-y3)*(y-y2) + x2; // intersection of p3 and p2
        x_left = min(temp_x1, temp_x2);     x_right = max(temp_x1, temp_x2);
        return;
    }
}

int main(){


//////////////////////////////////////////////////////////////////////////////////
 //                           
 //                             READ DATA
 //
//////////////////////////////////////////////////////////////////////////////////
    cout << "Reading data...\t";

    ifstream config("config.txt");
    config >> screen_width >> screen_height;
    config >> x_left_limit;     x_right_limit = - x_left_limit;
    config >> y_bottom_limit;   y_top_limit = - y_bottom_limit;
    config >> z_front_limit >> z_rear_limit;
    config.close();


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

    cout << "complete" << endl;
//////////////////////////////////////////////////////////////////////////////////
 //                           
 //                      initialize_z_buffer_and_frame_buffer()
 //
//////////////////////////////////////////////////////////////////////////////////
    cout << "Creating z_buffer...\t";
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
    cout << "complete" << endl;
//////////////////////////////////////////////////////////////////////////////////
//
//                                  MAIN PROCEDURE
//
////////////////////////////////////////////////////////////////////////////////////
    cout << "Main procedure - iterating...\t";

    dx = (x_right_limit - x_left_limit) / screen_width;
    dy = (y_top_limit - y_bottom_limit) / screen_height;

    for (int t = 0; t < triangles.size(); t++)
    {
        Triangle tr = triangles[t];

        double tr_max_y = max(max(tr.p1.y, tr.p2.y), tr.p3.y); // max_y of triangle
        double max_y = min(tr_max_y, y_top_limit);
        int upper_scanline_row = get_row_index_from_scanline_y(max_y);
        double upper_scanline = get_scanline_y_from_row_index(upper_scanline_row);

        double tr_min_y = min(min(tr.p1.y, tr.p2.y), tr.p3.y); // min_y of triangle
        double min_y = max(tr_min_y, y_bottom_limit);
        int lower_scanline_row = get_row_index_from_scanline_y(min_y);
        double lower_scanline = get_scanline_y_from_row_index(lower_scanline_row);

        int c, r;
        double x, y;
        for(r = lower_scanline_row, y = lower_scanline; r <= upper_scanline_row; r++, y += dy)  
        {
            double x_left, x_right;
            get_intersetion_x(tr, y, x_left, x_right); // call by reference, values returned in x_left and x_right

            int left_scanline_col = get_column_index_from_scanline_x(max(x_left, x_left_limit));
            double left_scanline = get_scanline_x_from_column_index(left_scanline_col);

            int right_scanline_col = get_column_index_from_scanline_x(min(x_right, x_right_limit));
            double right_scanline = get_scanline_x_from_column_index(right_scanline_col);

            for(c = left_scanline_col, x = left_scanline; c <= right_scanline_col; c++, x += dx)
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
    cout << "complete" << endl;

/////////////////////////////////////////////////////////////////////////////////////
//
//                                    IMAGE
//
///////////////////////////////////////////////////////////////////////////////////////
    cout << "Creating image...\t";
    int sw = int(screen_width);
    int sh = int(screen_height);
    bitmap_image image(sw, sh);

    for(int i=0; i<sw; i++){
        for(int j=0; j<sh; j++)
        {
            image.set_pixel(i, sh-j-1, z_buffer[i][j].color.r, z_buffer[i][j].color.g, z_buffer[i][j].color.b); // the bitmap library assumes the origin at the top-left corner, not the bottom-left; hence it is necesary to "flip the image"; that's why "j" is mapped to "sh-j-1" 
        }
    }
    image.save_image("1.bmp");
    cout << "complete" << endl;
///////////////////////////////////////////////////////////////////////////////
//
//                  PRINTING Z BUFFER
//
///////////////////////////////////////////////////////////////////////////////
    cout << "Printing z buffer...\t";
    ofstream zbfr("z_buffer.txt", ofstream::out);
    for(int i=0; i<sh; i++){
        for(int j=0; j<sw; j++)
        {
            int a = sw-i-1; // i don't know why it works. but it does
            int b = j;
            if (z_buffer[b][a].z < z_rear_limit)
                zbfr << fixed << setprecision(6) << z_buffer[b][a].z << "\t";
        }
        zbfr << endl;
    }
    zbfr.close();
    cout << "complete" << endl;
//////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       MEMORY RELEASE
//
//////////////////////////////////////////////////////////////////////////////////////////////

    cout << "Releasing memory...\t";
    vector<Triangle>().swap(triangles);
    triangles.clear();
    triangles.shrink_to_fit();

    for(int i = 0; i < screen_height; i++)
    {
        delete[] z_buffer[i];
        delete[] z_buffer;
    }
    cout << "complete" << endl;
//////////////////////////////////////////////////////////////////////////////////////////////

    cout << "Successfully done" << endl;

    return 0;
}