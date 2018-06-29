#include <bits/stdc++.h>
#include "bitmap_image.hpp"

using namespace std;

class edge_table_tuple
{
public:
    double x_of_y_min, y_max, slope_inverse;
    int triangle_id;    
};

bool compare_edge_table_tuple(edge_table_tuple &a, edge_table_tuple &b)
{return a.x_of_y_min < b.x_of_y_min;}

//  The ET is typically built by using a bucket sort with as many buckets as there are scan lines.
typedef vector<edge_table_tuple> bucket;
vector<bucket> edge_table;
bucket active_edge_table;



int main()
{
    vector<int> *v = new vector<int>;
}