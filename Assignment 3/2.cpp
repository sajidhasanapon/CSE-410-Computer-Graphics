#include <bits/stdc++.h>
#include "bitmap_image.hpp"

using namespace std;


double screen_width,    screen_height;
double x_left_limit,    x_right_limit;
double y_bottom_limit,  y_top_limit;
double z_rear_limit,    z_front_limit;

double dx, dy;

class edge_table_tuple
{
public:
    double x_of_y_min, y_min, y_max, slope_inverse;
    int triangle_id;

    edge_table_tuple(double x, double y_min, double y_max, double m_inv, int id)
    {
        this->x_of_y_min    = x;
        this->y_min         = y_min;
        this->y_max         = y_max;
        this->slope_inverse = m_inv;
        this->triangle_id   = id;
    }

    void add_dx(double dx)
    {
        this->x_of_y_min += dx;
    }
};

//  The ET is typically built by using a bucket sort with as many buckets as there are scan lines.
typedef vector<edge_table_tuple> bucket;
bucket* edge_table;
bucket active_edge_table;
vector<bool> active_polygon_table;

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

    Color(){r = g = b = 0.0;}

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

void swap(double &a, double &b)
{
    double x;
    x = a;
    a = b;
    b = x;
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

bool compare_edge_on_x(edge_table_tuple &a, edge_table_tuple &b)
{return a.x_of_y_min < b.x_of_y_min;}

bool compare_edge_on_y_min(edge_table_tuple &a, edge_table_tuple &b)
{return a.y_min < b.y_min;}

void add_edges(Triangle tr, int id, double y, int row)
{
    double x1, x2, y1, y2;

    // p1 and p2
    x1 = tr.p1.x; y1 = tr.p1.y;
    x2 = tr.p2.x; y2 = tr.p2.y;
    if(y1 > y2) {swap(x1, x2), swap(y1, y2);}
    if(y1 <= y && y < y2 && y1 != y2)
    {
            double m_inv = (x1 - x2) / (y1 - y2);
            double x = m_inv * (y - y1) + x1;
            edge_table_tuple t = edge_table_tuple(x, y1, y2, m_inv, id);
            edge_table[row].push_back(t);
    }

    // p1 and p3
    x1 = tr.p1.x; y1 = tr.p1.y;
    x2 = tr.p3.x; y2 = tr.p3.y;
    if(y1 > y2) {swap(x1, x2), swap(y1, y2);}
    if(y1 <= y && y < y2 && y1 != y2)
    {
            double m_inv = (x1 - x2) / (y1 - y2);
            double x = m_inv * (y - y1) + x1;
            edge_table_tuple t = edge_table_tuple(x, y1, y2, m_inv, id);
            edge_table[row].push_back(t);
    }

    // p2 and p3
    x1 = tr.p2.x; y1 = tr.p2.y;
    x2 = tr.p3.x; y2 = tr.p3.y;
    if(y1 > y2) {swap(x1, x2), swap(y1, y2);}
    if(y1 <= y && y < y2 && y1 != y2)
    {
            double m_inv = (x1 - x2) / (y1 - y2);
            double x = m_inv * (y - y1) + x1;
            edge_table_tuple t = edge_table_tuple(x, y1, y2, m_inv, id);
            edge_table[row].push_back(t);
    }

}

void update_active_edge_table(int row)
{
    active_edge_table = edge_table[row];
    sort(active_edge_table.begin(), active_edge_table.end(), compare_edge_on_x);
}



int main()
{
    freopen("stderr.txt", "w", stderr);

    ifstream config("config.txt");
    config >> screen_width >> screen_height;
    config >> x_left_limit;     x_right_limit = - x_left_limit;
    config >> y_bottom_limit;   y_top_limit = - y_bottom_limit;
    config >> z_front_limit >> z_rear_limit;
    config.close();

    edge_table = new bucket[int(screen_height)];

    dx = (x_right_limit - x_left_limit) / screen_width;
    dy = (y_top_limit - y_bottom_limit) / screen_height;

    ifstream input("stage3.txt");
    vector<Triangle> triangles;
    vector<Triangle>emptyVector;

    Color** grid = new Color*[int(screen_width)];
    for(int i = 0; i < screen_height; i++)
    {
        grid[i] = new Color[int(screen_height)];
        //for (int j = 0; j < screen_height; j++)
        //{}
    }

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
        active_polygon_table.push_back(false);

        //triangle.print();
    }
    input.close();



    // creating edge table
    cout << "Creating edge table..." << endl;
    for (int row = 0; row < int(screen_height); row++)
    {
        //double y = double(row) * dy + dy / 2.0 + y_bottom_limit;
        double y = map_row_to_y(row);
        for(int tr_id = 0; tr_id < triangles.size(); tr_id++)
        {
            Triangle tr = triangles[tr_id];
            add_edges(tr, tr_id, y, row);
        }
        sort(edge_table[row].begin(), edge_table[row].end(), compare_edge_on_x);
    }
    cout << "Complete" << endl;

    // for (int row = 0; row < int(screen_height); row++){
    //     cout << "row: " << row << "         id:  ";
    //     for(int n = 0; n < edge_table[row].size(); n++){
    //         cout << edge_table[row][n].triangle_id << " ";
    //     }
    //     cout << endl;
    // }


    cout << "Scanning..." << endl;
    for (int row = 0; row < int(screen_height); row++)
    {
        double y = row * dy + dy/2.0 + y_bottom_limit;
        active_edge_table = edge_table[row];
        if(active_edge_table.size() < 2) continue;

        for(int t = 0; t < active_edge_table.size()-1; t++)
        {
            double x_left = active_edge_table[t].x_of_y_min;
            int c_low = map_x_to_grid_col(x_left);
            double x_right = active_edge_table[t+1].x_of_y_min;
            int c_high = map_x_to_grid_col(x_right);

            active_polygon_table[active_edge_table[t].triangle_id] = !active_polygon_table[active_edge_table[t].triangle_id];
            // active_polygon_table[active_edge_table[t+1].triangle_id] = !active_polygon_table[active_edge_table[t+1].triangle_id]; 

            int c = c_low;
            //double x = map_col_to_x(c)-dx/2.0;
            double x = x_left;
            for(; c < c_high ; c++, x+=dx)
            {
                double z = z_rear_limit;
                Color col = Color(0.0, 0.0, 0.0);
                for(int tr = 0; tr < active_polygon_table.size(); tr++)
                {
                    if(active_polygon_table[tr] == true)
                    {
                       double z_t = (triangles[tr].d - triangles[tr].a*x - triangles[tr].b*y ) / triangles[tr].c;
                       if (z_t < z)
                       {
                           z = z_t;
                           col = triangles[tr].color;
                       }
                    }
                }
                if(0 <= c && c < screen_width)
                    grid[c][row] = col;
            }
        }
    }
    cout << "Complete" << endl;

    int sw = int(screen_width);
    int sh = int(screen_height);
    bitmap_image image(sw, sh);

    for(int i=0; i<sw; i++){
        for(int j=0; j<sh; j++)
        {
            image.set_pixel(i, sh-j-1, grid[i][j].r, grid[i][j].g, grid[i][j].b);   
        }
    }

    image.save_image("2.bmp");
    
    return 0;
}