#include <bits/stdc++.h>
using namespace std;

#define PI acos(-1.0)


stack<pair<double**, bool>> the_stack;




// ############################################################
// UTILITY FUNCTIONS

// returns a 4X4 zero matrix
double** zero_matrix()
{
    double** matrix = new double*[4];
    for(int i = 0; i < 4; i++)
    {
        matrix[i] = new double[4];
    }

    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            matrix[i][j] = 0;
        }
    }
    return matrix;
}

// returns a 4X4 identity matrix
double** I()
{
    double** identity_matrix = zero_matrix();

    // principal diagonal  
    for(int i = 0; i < 4; i++)
    {
        identity_matrix[i][i] = 1.0;
    }

    return identity_matrix;
}

// name says it all
double** multiply(double** matrix_1, double** matrix_2)
{
    double** ret = zero_matrix();

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                ret[i][j] += matrix_1[i][k] * matrix_2[k][j];

    return ret;
}

// this name is a bit confusing (sorry)
// what it actually does is multiplies the given transformation matrix ...
// ... with the_stack.top() and pushes the product back into the_stack.
void transform(double** matrix)
{
    double** prev = the_stack.top().first;
    double** res = multiply(prev, matrix);

    the_stack.push(make_pair(res, false));
}

// pushes a copy of the_stack.top() with is_pushed set to true
void push()
{
    double** mat = the_stack.top().first;
    the_stack.push(make_pair(mat, true));
}

// when pop() is called, is_pushed is checked
// the function is self-expplanatory (LOL)
void pop()
{
    while(the_stack.top().second == false)
    {
        the_stack.pop();
    }
    the_stack.pop();
}

// pretty obvious
void translate(double tx, double ty, double tz)
{
    double** matrix = I();

    matrix[0][3] = tx;
    matrix[1][3] = ty;
    matrix[2][3] = tz;

    transform(matrix);
}

// this one too, is obvious
void rotate(double angle, double ax, double ay, double az)
{
    double cos_theta = cos(PI * angle / 180.0);
    double sin_theta = sin(PI * angle / 180.0);

    // normalize the axis of rotation
    double L = sqrt(ax * ax + ay * ay + az * az);
    double ux = ax / L;
    double uy = ay / L;
    double uz = az / L;

    double** matrix = zero_matrix();

    matrix[0][0] = cos_theta + ux * ux * (1 - cos_theta);
    matrix[0][1] = ux * uy * (1 - cos_theta) - uz * sin_theta;
    matrix[0][2] = ux * uz * (1 - cos_theta) + uy * sin_theta;

    matrix[1][0] = ux * uy * (1 - cos_theta) + uz * sin_theta;
    matrix[1][1] = cos_theta + uy * uy * (1 - cos_theta);
    matrix[1][2] = uy * uz * (1 - cos_theta) - ux * sin_theta;

    matrix[2][0] = ux * uz * (1 - cos_theta) - uy * sin_theta;
    matrix[2][1] = uy * uz * (1 - cos_theta) + ux * sin_theta;
    matrix[2][2] = cos_theta + uz * uz * (1 - cos_theta);

    matrix[3][3] = 1.0;

    transform(matrix);
}

//goddamn obvious
void scale(double sx, double sy, double sz)
{
    double** matrix = I();

    matrix[0][0] = sx;
    matrix[1][1] = sy;
    matrix[2][2] = sz;

    transform(matrix);
}

double** triangle(double x0, double y0, double z0, 
            double x1, double y1, double z1,
            double x2, double y2, double z2)
    {
        double** matrix = zero_matrix();

        matrix[0][0] = x0;
        matrix[1][0] = y0;
        matrix[2][0] = z0;
        matrix[3][0] = 1.0;

        matrix[0][1] = x1;
        matrix[1][1] = y1;
        matrix[2][1] = z1;
        matrix[3][1] = 1.0;

        matrix[0][2] = x2;
        matrix[1][2] = y2;
        matrix[2][2] = z2;
        matrix[3][2] = 1.0;

        // column 4 - for the sake of creating a square matrix.
        // this column can be anything
        matrix[0][3] = matrix[1][3] = matrix[2][3] = matrix[3][3] = 1.0;

        // DEBUGGING PURPOSE

        double** res = multiply(the_stack.top().first, matrix);
        return res;
    }


void print_triangle(ofstream &stream, double** triangle)
{
    // need to print the transpose
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                stream << fixed << setprecision(7) << triangle[j][i] << " ";
            }
            stream << endl;
        }
        stream << endl;
}






ofstream stage1_out;
ofstream stage2_out;
ofstream stage3_out;

// given
double eyeX, eyeY, eyeZ;
double lookX, lookY, lookZ;
double upX, upY, upZ;
double fovY, aspectRatio, near, far;

// temporary variables for stage 1
string command;
double tx, ty, tz;
double sx, sy, sz;
double angle, ax, ay, az;
double p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z;

// temporary variables for stage 2
double lx, ly, lz;
double rx, ry, rz;
double ux, uy, uz;
double L;

double **V;
double **R;
double **T;

// temporary variables for stage 3
double fovX, t, r;
double **P;

void init()
{
    the_stack.push(make_pair(I(), false));

    stage1_out.open("stage1.txt");
    stage2_out.open("stage2.txt");
    stage3_out.open("stage3.txt");

    cin >> eyeX >> eyeY >> eyeZ;
    cin >> lookX >> lookY >> lookZ;
    cin >> upX >> upY >> upZ;
    cin >> fovY >> aspectRatio >> near >> far;

    lx = lookX - eyeX;
    ly = lookY - eyeY;
    lz = lookZ - eyeZ;
    L = sqrt(lx * lx + ly * ly + lz * lz);
    lx /= L;
    ly /= L;
    lz /= L;

    rx = ly * upZ - lz * upY;
    ry = lz * upX - lx * upZ;
    rz = lx * upY - ly * upX;
    L = sqrt(rx * rx + ry * ry + rz * rz);
    rx /= L;
    ry /= L;
    rz /= L;

    ux = ry * lz - rz * ly;
    uy = rz * lx - rx * lz;
    uz = rx * ly - ry * lx;
    L = sqrt(ux * ux + uy * uy + uz * uz);
    ux /= L;
    uy /= L;
    uz /= L;

    T = I();
    T[0][3] = -eyeX;
    T[1][3] = -eyeY;
    T[2][3] = -eyeZ;

    R = zero_matrix();
    R[0][0] = rx;
    R[0][1] = ry;
    R[0][2] = rz;
    R[1][0] = ux;
    R[1][1] = uy;
    R[1][2] = uz;
    R[2][0] = -lx;
    R[2][1] = -ly;
    R[2][2] = -lz;
    R[3][3] = 1.0;

    V = multiply(R, T);

    // stage 3
    fovX = fovY * aspectRatio;
    t = near * tan(fovY /2.0 * PI / 180.0);
    r = near * tan(fovX /2.0 * PI / 180.0);

    P = zero_matrix();

    P[0][0] = near / r;
    P[1][1] = near / t;
    P[2][2] = -(far + near) / (far - near);
    P[2][3] = -(2 * far * near) / (far - near);
    P[3][2] = -1.0;
}

int main()
{
    freopen("scene.txt", "r", stdin);
    init();

    while (1)
    {
        cin >> command;

        if (command == "triangle")
        {
            cin >> p1x >> p1y >> p1z;
            cin >> p2x >> p2y >> p2z;
            cin >> p3x >> p3y >> p3z;

            double **tr1 = triangle(p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if(j==2)
                    {
                        stage1_out << fixed << setprecision(7) << tr1[j][i];
                    }
                    else
                    {
                        stage1_out << fixed << setprecision(7) << tr1[j][i] << " ";
                    }
                }
                stage1_out << endl;
            }
            stage1_out << endl;

            double **tr2 = multiply(V, tr1);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if(j==2)
                    {
                        stage2_out << fixed << setprecision(7) << tr2[j][i];
                    }
                    else
                    {
                        stage2_out << fixed << setprecision(7) << tr2[j][i] << " ";
                    }
                }
                stage2_out << endl;
            }
            stage2_out << endl;

            double **tr3 = multiply(P, tr2);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if(j==2)
                    {
                        stage3_out << fixed << setprecision(7) << tr3[j][i]/tr3[3][i];
                    }
                    else
                    {
                        stage3_out << fixed << setprecision(7) << tr3[j][i]/tr3[3][i] << " ";
                    }
                    
                }
                stage3_out << endl;
            }
            stage3_out << endl;
        }
        else if (command == "translate")
        {
            cin >> tx >> ty >> tz;
            translate(tx, ty, tz);
        }
        else if (command == "scale")
        {
            cin >> sx >> sy >> sz;
            scale(sx, sy, sz);
        }
        else if (command == "rotate")
        {
            cin >> angle >> ax >> ay >> az;
            rotate(angle, ax, ay, az);
        }
        else if (command == "push")
        {
            push();
        }
        else if (command == "pop")
        {
            pop();
        }
        else if (command == "end")
        {
            break;
        }
        else
        {
            cout << "UNKNOWN COMMAND" << endl;
        }
    }

    return 0;
}