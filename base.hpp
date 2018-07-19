#ifndef BASE_HPP_INCLUDED
#define BASE_HPP_INCLUDED

#define EPSILON 0.000001

extern int recursion_level;

class Point3
{
  public:
    double x, y, z;

    Point3() {}

    Point3(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Point3 operator+(Point3 p)
    {
        return Point3(this->x + p.x, this->y + p.y, this->z + p.z);
    }

    Point3 operator-(Point3 p)
    {
        return Point3(this->x - p.x, this->y - p.y, this->z - p.z);
    }

    Point3 operator*(double scale)
    {
        return Point3(this->x * scale, this->y * scale, this->z * scale);
    }

    Point3 operator/(double scale)
    {
        return Point3(this->x / scale, this->y / scale, this->z / scale);
    }

    Point3 normalize()
    {
        return *this / sqrt(x * x + y * y + z * z);
    }

    double length()
    {
        return sqrt(x * x + y * y + z * z);
    }

    friend double dot(Point3 p1, Point3 p2)
    {
        return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
    }

    friend Point3 cross(Point3 p1, Point3 p2)
    {
        double a = p1.y * p2.z - p1.z * p2.y;
        double b = p1.z * p2.x - p1.x * p2.z;
        double c = p1.x * p2.y - p1.y * p2.x;

        return Point3(a, b, c);
    }

    friend Point3 rotate(Point3 vec, Point3 ref, double sign)
    {
        Point3 n, ret;
        double angle = 0.05 * sign;

        n.x = ref.y * vec.z - ref.z * vec.y;
        n.y = ref.z * vec.x - ref.x * vec.z;
        n.z = ref.x * vec.y - ref.y * vec.x;

        ret.x = vec.x * cos(angle) + n.x * sin(angle);
        ret.y = vec.y * cos(angle) + n.y * sin(angle);
        ret.z = vec.z * cos(angle) + n.z * sin(angle);

        return ret;
    }

    void print()
    {
        cout << x << "," << y << "," << z << endl;
    }
};

class Ray
{

  public:
    Point3 start;
    Point3 dir;

    Ray(Point3 start, Point3 dir)
    {
        this->start = start;
        this->dir = dir.normalize();
    }
};

enum
{
    AMBIENT,
    DIFFUSE,
    SPECULAR,
    REFLECTION
};

class Object
{

  public:
    char type = 'x';
    double source_factor = 1.0, refIdx = 0.0;
    double shine;
    double color[3];
    double co_efficients[4];

    Object() {}

    virtual void draw() = 0;
    virtual double getIntersectionT(Ray *r, bool debug) = 0;
    virtual Point3 getNormal(Point3 intersection) = 0;

    double intersect(Ray *ray, double current_color[3], int level);

    void setColor(double r, double g, double b)
    {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double a, double d, double s, double r)
    {
        co_efficients[AMBIENT] = a;
        co_efficients[DIFFUSE] = d;
        co_efficients[SPECULAR] = s;
        co_efficients[REFLECTION] = r;
    }

    Point3 get_reflected_ray_direction(Ray *ray, Point3 normal)
    {
        Point3 out_dir = ray->dir - normal * 2.0 * dot(ray->dir, normal);
        return out_dir.normalize();
    }

    Point3 getRefraction(Ray *ray, Point3 normal)
    {
        double n_dot_i = dot(normal, ray->dir);
        double k = 1.0 - refIdx * refIdx * (1.0 - n_dot_i * n_dot_i);

        if (k < 0.0)
            return Point3(0.0, 0.0, 0.0);

        Point3 out_dir = ray->dir * refIdx - normal * (refIdx * n_dot_i + sqrt(k));
        return out_dir.normalize();
    }
};

vector<Object *> objects;
vector<Point3> lights;

int get_nearest(Ray *ray)
{
    int nearest = -1;
    double t_min = 9999999;

    for (int k = 0; k < objects.size(); k++)
    {
        double tk = objects[k]->getIntersectionT(ray, false);

        if (tk <= 0)
        {
            continue;
        }
        else if (tk < t_min)
        {
            t_min = tk;
            nearest = k;
        }
    }

    return nearest;
}

double Object::intersect(Ray *ray, double current_color[3], int level)
{
    double t = getIntersectionT(ray, false);
    Point3 intersectionPoint = ray->start + ray->dir * t;
    Point3 normal = getNormal(intersectionPoint);
    Point3 reflection = get_reflected_ray_direction(ray, normal);
    Point3 refraction = getRefraction(ray, normal);

    double lambert, phong;

    for (int i = 0; i < lights.size(); i++)
    {
        // ambientt term
        for (int k = 0; k < 3; k++)
        {
            current_color[k] += color[k] * co_efficients[AMBIENT];
        }

        Point3 dir = (lights[i] - intersectionPoint);
        double len = dir.length();
        dir = dir.normalize();

        Point3 start = intersectionPoint + dir * EPSILON;
        Ray L(start, dir);

        //Point3 E = (intersectionPoint).normalize();
        //Point3 R = (get_reflected_ray_direction(&L, normal)*-1.0).normalize();


        Point3 R = (normal * (dot(L.dir, normal) * 2.0) - L.dir).normalize();
        Point3 V = (intersectionPoint*-1.0).normalize();

        bool flag = false;

        for (int j = 0; j < objects.size(); j++)
        {
            double tObj = objects[j]->getIntersectionT(&L, false);

            if (tObj > 0)
            {
                flag = true;
                break;
            }
        }

        if (!flag)
        {
            lambert = source_factor * co_efficients[DIFFUSE] * max(dot(L.dir, normal), 0.0);
            phong = co_efficients[SPECULAR] * max(pow(dot(R, V), shine), 0.0);

            lambert = min(lambert, 1.0);
            phong = min(phong, 1.0);


            for (int k = 0; k < 3; k++)
            {
                current_color[k] += (lambert * color[k]);
                current_color[k] += (phong * color[k]);
            }
        }

        if (level < recursion_level)
        {
            Point3 start = intersectionPoint + reflection * EPSILON;

            Ray reflectionRay(start, reflection);
            double reflected_color[3] = {0.0, 0.0, 0.0};

            int nearest = get_nearest(&reflectionRay);

            if (nearest != -1)
            {
                objects[nearest]->intersect(&reflectionRay, reflected_color, level + 1);

                for (int k = 0; k < 3; k++)
                {
                    current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                }
            }

            start = intersectionPoint + refraction * EPSILON;

            Ray refractionRay(start, refraction);
            double refracted_color[3] = {0.0, 0.0, 0.0};

            nearest = get_nearest(&refractionRay);

            if (nearest != -1)
            {
                objects[nearest]->intersect(&refractionRay, refracted_color, level + 1);

                for (int k = 0; k < 3; k++)
                {
                    current_color[k] += refracted_color[k] * refIdx;
                }
            }
        }

        for (int k = 0; k < 3; k++)
        {
            if (current_color[k] > 1)
            {
                current_color[k] = 1;
            }
            else if (current_color[k] < 0)
            {
                current_color[k] = 0;
            }
        }
    }

    return t;
}

class Sphere : public Object
{
  public:
    Point3 center;
    double radius;

    Sphere(Point3 center, double radius)
    {
        this->center = center;
        this->radius = radius;
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glPushMatrix();
        glTranslatef(center.x, center.y, center.z);
        glutSolidSphere(radius, 100, 100);
        glPopMatrix();
    }

    double getIntersectionT(Ray *ray, bool debug)
    {

        Point3 c = center;
        Point3 o = ray->start;
        Point3 l = ray->dir;

        Point3 d = o - c;
        double discriminant = dot(l, d) * dot(l, d) - dot(d, d) + radius * radius;

        if (discriminant < 0)
            return -1;

        double sqrt_disc = sqrt(discriminant);
        double t1 = -dot(l, d) + sqrt_disc;
        double t2 = -dot(l, d) - sqrt_disc;

        if (debug == false)
            return min(t1, t2);
        return max(t1, t2);
    }

    Point3 getNormal(Point3 intersection)
    {
        Point3 normal = intersection - center;
        return normal.normalize();
    }
};

class Floor : public Object
{

  public:
    Point3 origin;
    double floorWidth, tile_size;
    int n_tiles;

    Floor(double floorWidth, double tile_size)
    {
        this->floorWidth = floorWidth;
        this->tile_size = tile_size;
        this->origin = Point3(-floorWidth / 2.0, -floorWidth / 2.0, 0.0);
        this->n_tiles = floorWidth / tile_size;
    }

    void draw()
    {
        glBegin(GL_QUADS);
        {
            for (int i = 0; i < n_tiles; i++)
                for (int j = 0; j < n_tiles; j++)
                {
                    bool c = (i + j) % 2;
                    glColor3f(c, c, c);

                    glVertex3f(origin.x + tile_size * i, origin.y + tile_size * j, origin.z);
                    glVertex3f(origin.x + tile_size * (i + 1), origin.y + tile_size * j, origin.z);
                    glVertex3f(origin.x + tile_size * (i + 1), origin.y + tile_size * (j + 1), origin.z);
                    glVertex3f(origin.x + tile_size * i, origin.y + tile_size * (j + 1), origin.z);
                }
        }
        glEnd();
    }

    Point3 getNormal(Point3 intersection) { return Point3(0, 0, 1); }

    double getIntersectionT(Ray *ray, bool debug)
    {

        if (ray->dir.z == 0)
            return -1;

        double t = -(ray->start.z / ray->dir.z);
        Point3 intersectionPoint = ray->start + ray->dir * t;

        int pixel_x = (intersectionPoint.x - origin.x) / tile_size;
        int pixel_y = (intersectionPoint.y - origin.y) / tile_size;

        if (pixel_x < 0 || pixel_y < 0 || pixel_x >= n_tiles || pixel_y >= n_tiles)
            return -1;

        int c = (pixel_x + pixel_y) % 2;
        color[0] = color[1] = color[2] = c;

        return t;
    }
};

class Triangle : public Object
{
  public:
    Point3 p1, p2, p3;
    // let the equation of the plane of the triangle be ax + by + cz = d
    double a, b, c, d;

    Triangle(Point3 p1, Point3 p2, Point3 p3)
    {
        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;

        // calculating a, b, c, d
        // using parametric form
        Point3 AB = p2 - p1;
        Point3 AC = p3 - p1;

        Point3 V = cross(AB, AC);

        this->a = V.x;
        this->b = V.y;
        this->c = V.z;
        this->d = this->a * p1.x + this->b * p1.y + this->c * p1.z;
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
            glVertex3f(p3.x, p3.y, p3.z);
        }
        glEnd();
    }

    Point3 getNormal(Point3 intersection)
    {
        Point3 normal = Point3(a, b, c);
        return normal.normalize();
    }

    double getIntersectionT(Ray *ray, bool debug)
    {
        Point3 normal = getNormal(Point3(0, 0, 0));

        double t;
        bool flag = false;
        double denom = dot(normal, ray->dir);

        if (denom < 0.0)
        {
            normal = normal * -1.0;
            denom = dot(normal, ray->dir);
        }

        // checking whether the ray intersects with the triangle's plane
        if (abs(denom) < EPSILON)
            return -1;

        t = (dot(normal, p1 - ray->start)) / denom;
        if (t >= 0)
            flag = true; // yes, intersects the plane

        if (!flag)
            return -1; // no

        // checking whether the point of intersection is inside the triangle
        bool b1, b2, b3;
        Point3 intersectionPoint = ray->start + ray->dir * t;

        Point3 N = cross(p2 - p1, p3 - p1);
        double area2 = N.length();

        Point3 C, edge1, edge2;

        // edge 0
        edge1 = p2 - p1;
        edge2 = intersectionPoint - p1;
        C = cross(edge1, edge2);
        if (dot(N, C) < 0)
            return -1;

        edge1 = p3 - p2;
        edge2 = intersectionPoint - p2;
        C = cross(edge1, edge2);
        if (dot(N, C) < 0)
            return -1;

        edge1 = p1 - p3;
        edge2 = intersectionPoint - p3;
        C = cross(edge1, edge2);
        if (dot(N, C) < 0)
            return -1;

        return t;
    }
};

class GeneralQuadratic : public Object
{

  public:
    Point3 reference_point;
    double height, width, length;
    double A, B, C, D, E, F, G, H, I, J;

    GeneralQuadratic(double coeff[10], Point3 reff, double length, double width, double height)
    {
        this->A = coeff[0];
        this->B = coeff[1];
        this->C = coeff[2];
        this->D = coeff[3];
        this->E = coeff[4];
        this->F = coeff[5];
        this->G = coeff[6];
        this->H = coeff[7];
        this->I = coeff[8];
        this->J = coeff[9];
        this->reference_point = reff;
        this->height = height;
        this->width = width;
        this->length = length;
    }

    void draw() {}

    Point3 getNormal(Point3 intersection)
    {

        double u = 2 * A * intersection.x + D * intersection.y + F * intersection.z + G;
        double v = 2 * B * intersection.y + D * intersection.x + E * intersection.z + H;
        double z = 2 * C * intersection.z + E * intersection.y + F * intersection.x + I;

        Point3 normal(u, v, z);

        return normal.normalize();
    }

    double getIntersectionT(Ray *ray, bool debug)
    {

        double a = A * ray->dir.x * ray->dir.x + B * ray->dir.y * ray->dir.y + C * ray->dir.z * ray->dir.z;
        double b = 2 * (A * ray->start.x * ray->dir.x + B * ray->start.y * ray->dir.y + C * ray->start.z * ray->dir.z);
        double c = A * ray->start.x * ray->start.x + B * ray->start.y * ray->start.y + C * ray->start.z * ray->start.z;

        a += D * ray->dir.x * ray->dir.y + E * ray->dir.y * ray->dir.z + F * ray->dir.z * ray->dir.x;
        b += D * (ray->start.x * ray->dir.y + ray->dir.x * ray->start.y) + E * (ray->start.y * ray->dir.z + ray->dir.y * ray->start.z) + F * (ray->start.z * ray->dir.x + ray->dir.z * ray->start.x);
        c += D * ray->start.x * ray->start.y + E * ray->start.y * ray->start.z + F * ray->start.z * ray->start.x;

        b += G * ray->dir.x + H * ray->dir.y + I * ray->dir.z;
        c += G * ray->start.x + H * ray->start.y + I * ray->start.z + J;

        double d = b * b - 4 * a * c;

        if (d < 0)
        {
            return -1;
        }

        double t1 = (-b + sqrt(d)) / (2.0 * a);
        double t2 = (-b - sqrt(d)) / (2.0 * a);

        Point3 intersectionPoint1 = ray->start + ray->dir * t1;
        Point3 intersectionPoint2 = ray->start + ray->dir * t2;

        double xMin = reference_point.x;
        double xMax = xMin + length;

        double yMin = reference_point.y;
        double yMax = yMin + width;

        double zMin = reference_point.z;
        double zMax = zMin + height;

        //cout<<xMin<<','<<xMax<<';'<<yMin<<','<<yMax<<';'<<zMin<<','<<zMax<<endl;
        //cout<<intersectionPoint1<<intersectionPoint2;

        bool flag1 = (length > 0 && (xMin > intersectionPoint1.x || intersectionPoint1.x > xMax) ||
                      width > 0 && (yMin > intersectionPoint1.y || intersectionPoint1.y > yMax) ||
                      height > 0 && (zMin > intersectionPoint1.z || intersectionPoint1.z > zMax));

        bool flag2 = (length > 0 && (xMin > intersectionPoint2.x || intersectionPoint2.x > xMax) ||
                      width > 0 && (yMin > intersectionPoint2.y || intersectionPoint2.y > yMax) ||
                      height > 0 && (zMin > intersectionPoint2.z || intersectionPoint2.z > zMax));

        //cout<<flag1<<','<<flag2<<endl;

        if (flag1 && flag2)
        {
            return -1;
        }
        else if (flag1)
        {
            return t2;
        }
        else if (flag2)
        {
            return t1;
        }
        else
        {
            if (t2 > t1)
            {
                double temp = t1;
                t1 = t2;
                t2 = temp;
            }
            return t2;
        }
    }
};

#endif // BASE_HPP_INCLUDED
