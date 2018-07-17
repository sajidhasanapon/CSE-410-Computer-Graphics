#ifndef BASE_HPP_INCLUDED
#define BASE_HPP_INCLUDED

#define EPSILON 0.000001


class Point3
{
public:
    double x, y, z;

    Point3(){}

    Point3(double x, double y, double z)
    {this->x = x; this->y = y; this->z = z;}

    Point3 operator + (Point3 p)
    {return Point3(this->x + p.x, this->y + p.y, this->z + p.z);}

    Point3 operator - (Point3 p)
    {return Point3(this->x - p.x, this->y - p.y, this->z - p.z);}

    Point3 operator * (double scale)
    {return Point3(this->x*scale, this->y*scale, this->z*scale);}

    Point3 operator / (double scale)
    {return Point3(this->x/scale, this->y/scale, this->z/scale);}

    Point3 normalize()
    {return *this / sqrt(x*x + y*y + z*z);}

    friend double dot(Point3 p1, Point3 p2)
    {return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;}

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

        n.x = ref.y*vec.z - ref.z*vec.y;
        n.y = ref.z*vec.x - ref.x*vec.z;
        n.z = ref.x*vec.y - ref.y*vec.x;

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








class Ray {

public:

    Point3 start;
    Point3 dir;

    Ray(Point3 start, Point3 dir) {
        this->start = start;
        this->dir = dir.normalize();
    }
};

enum {AMBIENT, DIFFUSE, SPECULAR, REFLECTION};

class Object {

public:

    Point3 reference_point;

    double height, width, length, source_factor = 1.0, refIdx = 1.5;

    int shine;

    double color[3];

    double co_efficients[4];

    Object() {}

    virtual void draw() = 0;

    virtual double getIntersectionT(Ray *r, bool debug) = 0;

    virtual Point3 getNormal(Point3 intersection) = 0;

    virtual double intersect(Ray* r, double current_color[3], int level) = 0;

    void setColor(double r, double g, double b) {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }

    void setShine(int shine) {
        this->shine = shine;
    }

    void setCoEfficients(double a, double d, double s, double r) {
        co_efficients[AMBIENT] = a;
        co_efficients[DIFFUSE] = d;
        co_efficients[SPECULAR] = s;
        co_efficients[REFLECTION] = r;
    }

    Point3 getReflection(Ray* ray, Point3 normal) {
        Point3 reflection = ray->dir - normal * 2.0 * dot(ray->dir, normal);
        return reflection.normalize();
    }

    Point3 getRefraction(Ray* ray, Point3 normal) {

        Point3 refraction(0, 0, 0);

        double d = dot(normal, ray->dir);
        double k = 1.0 - refIdx * refIdx * (1.0 - d * d);

        if (k >= 0) {
            refraction = ray->dir * refIdx - normal * (refIdx * d + sqrt(k));
            refraction = refraction.normalize();
        }

        return refraction;
    }

};

vector<Object*> objects;
vector<Point3> lights;

class Sphere: public Object {

public:

    Sphere(Point3 center, double radius) {
        reference_point = center;
        length = radius;
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glPushMatrix();
        glTranslatef (reference_point.x, reference_point.y, reference_point.z);
        glutSolidSphere(length, 100, 100);
        glPopMatrix();
    }

    double getIntersectionT(Ray* ray, bool debug) {

        Point3 c = reference_point;
        Point3 o = ray->start;
        Point3 l = ray->dir;

        Point3 d = o - c;
        double discriminant = dot(l, d)*dot(l, d) - dot(d, d) + length*length;

        if(discriminant < 0) return -1;

        double sqrt_disc = sqrt(discriminant);
        double t1 = -dot(l, d) + sqrt_disc;
        double t2 = -dot(l, d) - sqrt_disc;

        return min(t1, t2);
    }

    Point3 getNormal(Point3 intersection) {
        Point3 normal = intersection - reference_point;
        return normal.normalize();
    }

    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray, false);

        if(t <= 0) return -1;
        if(level == 0) return t;

        ////////////////////////////////////////////////////////////////////////////////
        if(level == 1)
        {
            current_color[0] = color[0];
            current_color[1] = color[1];
            current_color[2] = color[2];

            return 0;
        }


        for (int i=0; i<3; i++) {
            current_color[i] = color[i] * co_efficients[AMBIENT];
        }


        Point3 intersectionPoint = ray->start + ray->dir * t;

        Point3 normal = getNormal(intersectionPoint);

        Point3 reflection = getReflection(ray, normal);

        Point3 refraction = getRefraction(ray, normal);

        for (int i=0; i<lights.size(); i++) {

            Point3 dir = lights[i] - intersectionPoint;
            double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
            dir = dir.normalize();

            Point3 start = intersectionPoint + dir*1.0;
            Ray L(start, dir);
            //cout<<intersectionPoint<<L.start<<L.dir;

            bool flag = false;

            for (int j=0; j < objects.size(); j++) {

                double tObj = objects[j]->getIntersectionT(&L, true);

                if(tObj > 0 || abs(tObj) > len) {
                    continue;
                }

                flag = true;
                break;
            }

            if (flag){

                double lambert = dot(L.dir, normal);
                double phong = pow(dot(reflection, ray->dir), shine);

                lambert = lambert > 0 ? lambert : 0;
                phong = phong > 0 ? phong : 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[DIFFUSE] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[SPECULAR] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                int nearest=-1;
                double minT = 9999999;
                double reflected_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&reflectionRay, true);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                    }
                }

                start = intersectionPoint + refraction * 1.0;

                Ray refractionRay(start, refraction);

                nearest=-1;
                minT = 9999999;
                double refracted_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&refractionRay, true);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&refractionRay, refracted_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += refracted_color[k] * refIdx;
                    }
                }
            }

            for (int k=0; k<3; k++) {
                if (current_color[k] > 1) {
                    current_color[k] = 1;
                } else if (current_color[k] < 0) {
                    current_color[k] = 0;
                }
            }


        }

        return t;
    }


};

class Floor: public Object {

public:

    double floorWidth;
    int n_tiles;

    Floor(double floorWidth, double tileWidth) {
        reference_point = Point3(-floorWidth/2.0, -floorWidth/2.0, 0.0);
        length = tileWidth;

        this -> floorWidth = floorWidth;
        n_tiles = floorWidth / length;
    }

    void draw(){
        glBegin(GL_QUADS);
        {
            for(int i = 0; i < n_tiles; i++)
                for(int j = 0; j < n_tiles; j++)
                {
                    bool c = (i + j) % 2;
                    glColor3f(c, c, c);

                    glVertex3f(reference_point.x+length*i, reference_point.y+length*j, reference_point.z);
                    glVertex3f(reference_point.x+length*(i+1), reference_point.y+length*j, reference_point.z);
                    glVertex3f(reference_point.x+length*(i+1), reference_point.y+length*(j+1), reference_point.z);
                    glVertex3f(reference_point.x+length*i, reference_point.y+length*(j+1), reference_point.z);
                }
        }
        glEnd();
    }

    Point3 getNormal(Point3 intersection) {
        Point3 normal(0,0,1);
        return normal.normalize();
    }

    double getIntersectionT(Ray* ray, bool debug) {

        if(ray->dir.z == 0) return -1;

        double t = -(ray->start.z / ray->dir.z);
        return t;
    }


    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray, false);
        if(t<=0) return -1;
        if(level==0) return t;

        Point3 intersectionPoint = ray->start + ray->dir * t;
        int pixel_x = (intersectionPoint.x - reference_point.x) / length;
        int pixel_y = (intersectionPoint.y - reference_point.y) / length;

        if(pixel_x < 0 || pixel_y < 0 || pixel_x >= n_tiles || pixel_y >= n_tiles) return -1;

        int c = (pixel_x + pixel_y)%2;
        color[0] = color[1] = color[2] = c;
        ////////////////////////////////////////////////////////////////////////////////



        
        if(level == 1)
        {
            current_color[0] = color[0];
            current_color[1] = color[1];
            current_color[2] = color[2];

            return 0;
        }

        for (int i=0; i<3; i++) current_color[i] = color[i] * co_efficients[AMBIENT];

        Point3 normal = getNormal(intersectionPoint);

        Point3 reflection = getReflection(ray, normal);
        Point3 refraction = getRefraction(ray, normal);

        for (int i=0; i<lights.size(); i++) {

            Point3 dir = lights[i] - intersectionPoint;
            double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
            dir = dir.normalize();

            Point3 start = intersectionPoint + dir*1.0;
            Ray L(start, dir);
            //cout<<intersectionPoint<<L.start<<L.dir;

            bool flag = false;

            for (int j=0; j < objects.size(); j++) {

                double tObj = objects[j]->getIntersectionT(&L, true);

                if(tObj > 0 || abs(tObj) > len) {
                    continue;
                }

                flag = true;
                break;
            }

            if (flag){

                double lambert = dot(L.dir, normal);
                double phong = pow(dot(reflection, ray->dir), shine);

                lambert = lambert > 0 ? lambert : 0;
                phong = phong > 0 ? phong : 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[DIFFUSE] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[SPECULAR] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                int nearest=-1;
                double minT = 9999999;
                double reflected_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&reflectionRay, true);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                    }
                }

                start = intersectionPoint + refraction * 1.0;

                Ray refractionRay(start, refraction);

                nearest=-1;
                minT = 9999999;
                double refracted_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&refractionRay, true);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&refractionRay, refracted_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += refracted_color[k] * refIdx;
                    }
                }
            }

            for (int k=0; k<3; k++) {
                if (current_color[k] > 1) {
                    current_color[k] = 1;
                } else if (current_color[k] < 0) {
                    current_color[k] = 0;
                }
            }


        }

        return t;
    }


};

class Triangle: public Object
{

public:

    Point3 a, b, c;

    Triangle(Point3 a, Point3 b, Point3 c) {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void draw() {
        glColor3f(color[0],color[1],color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        }
        glEnd();
    }

    Point3 getNormal(Point3 intersection) {

        Point3 u = b - a;
        Point3 v = c - a;

        Point3 normal = cross(u, v);

        return normal.normalize();
    }

    double getIntersectionT(Ray* ray, bool debug) {
        
        Point3 normal = getNormal(Point3(0,0,0));

        double denom = dot(normal, ray->dir);
        if (abs(denom) > 0.0001f) // your favorite epsilon
        {
            double t = dot(a - ray->start, normal) / denom;
            if (t >= 0) return true; // you might want to allow an epsilon here too
        }
        return false;

        Point3 e1 = b - a;
        Point3 e2 = c - a;

        Point3 p = cross(ray->dir, e2);
        double det = dot(e1, p);

        if(det > -EPSILON && det < EPSILON) {
            return -1;
        }

        double inv_det = 1.0 / det;

        Point3 T = ray->start - a;

        double u = dot(T, p) * inv_det;

        if(u < 0 || u > 1) {
            return -1;
        }

        Point3 q = cross(T, e1);

        double v = dot(ray->dir, q) * inv_det;

        if(v < 0 || u + v  > 1) {
            return -1;
        }

        double t = dot(e2, q) * inv_det;

        if(t > EPSILON) { //ray intersection
            return t;
        }

        return -1;
    }


    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray, false);

        if (t <= 0) {
            return -1;
        }

        if (level == 0 ) {
            return t;
        }


        for (int i=0; i<3; i++) {
            current_color[i] = color[i] * co_efficients[AMBIENT];
        }


        Point3 intersectionPoint = ray->start + ray->dir * t;

        for (int i=0; i<lights.size(); i++) {

            Point3 normal = getNormal(intersectionPoint);

            Point3 dir = lights[i] - intersectionPoint;
            double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
            dir = dir.normalize();

            if (dot(dir, normal) > 0) {
                normal = normal * (-1);
            }

            Point3 reflection = getReflection(ray, normal);
            Point3 refraction = getRefraction(ray, normal);

            Point3 start = intersectionPoint + dir*1.0;
            Ray L(start, dir);
            //cout<<intersectionPoint<<L.start<<L.dir;

            bool flag = false;

            for (int j=0; j < objects.size(); j++) {

                double tObj = objects[j]->getIntersectionT(&L, true);

                if(tObj > 0 || abs(tObj) > len) {
                    continue;
                }

                flag = true;
                break;
            }

            if (flag){

                double lambert = dot(L.dir, normal);
                double phong = pow(dot(reflection, ray->dir), shine);

                lambert = lambert > 0 ? lambert : 0;
                phong = phong > 0 ? phong : 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[DIFFUSE] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[SPECULAR] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                int nearest=-1;
                double minT = 9999999;
                double reflected_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&reflectionRay, true);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                    }
                }

                start = intersectionPoint + refraction * 1.0;

                Ray refractionRay(start, refraction);

                nearest=-1;
                minT = 9999999;
                double refracted_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&refractionRay, true);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&refractionRay, refracted_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += refracted_color[k] * refIdx;
                    }
                }
            }

            for (int k=0; k<3; k++) {
                if (current_color[k] > 1) {
                    current_color[k] = 1;
                } else if (current_color[k] < 0) {
                    current_color[k] = 0;
                }
            }


        }

        return t;
    }



};


class GeneralQuadratic: public Object {

public:

    double A, B, C, D, E, F, G, H, I, J;

    GeneralQuadratic(double coeff[10], Point3 reff, double length, double width, double height) {
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

    Point3 getNormal(Point3 intersection) {

        double u = 2 * A * intersection.x + D * intersection.y + F * intersection.z  + G;
        double v = 2 * B * intersection.y + D * intersection.x + E * intersection.z  + H;
        double z = 2 * C * intersection.z + E * intersection.y + F * intersection.x  + I;

        Point3 normal(u, v, z);

        return normal.normalize();
    }

    double getIntersectionT(Ray* ray, bool debug) {

        double a = A * ray->dir.x * ray->dir.x + B * ray->dir.y * ray->dir.y + C * ray->dir.z * ray->dir.z;
        double b = 2 * (A * ray->start.x * ray->dir.x + B * ray->start.y * ray->dir.y + C * ray->start.z * ray->dir.z);
        double c = A * ray->start.x * ray->start.x + B * ray->start.y * ray->start.y + C * ray->start.z * ray->start.z;

        a += D * ray->dir.x * ray->dir.y + E * ray->dir.y * ray->dir.z + F * ray->dir.z * ray->dir.x;
        b += D * (ray->start.x * ray->dir.y + ray->dir.x * ray->start.y)
             + E * (ray->start.y * ray->dir.z + ray->dir.y * ray->start.z)
             + F * (ray->start.z * ray->dir.x + ray->dir.z * ray->start.x);
        c += D * ray->start.x * ray->start.y + E * ray->start.y * ray->start.z + F * ray->start.z * ray->start.x;

        b += G * ray->dir.x + H * ray->dir.y + I * ray->dir.z;
        c += G * ray->start.x + H * ray->start.y + I * ray->start.z + J;


        double d = b*b - 4*a*c;


        if (d < 0) {
            return -1;
        }

        double t1 = (- b + sqrt(d)) / (2.0*a);
        double t2 = (- b - sqrt(d)) / (2.0*a);


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

        if (flag1 && flag2) {
            return -1;
        } else if (flag1) {
            return t2;
        } else if (flag2) {
            return t1;
        } else {
            if (t2 > t1) {
                double temp = t1;
                t1 = t2;
                t2 = temp;
            }
            return t2;
        }
    }


    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray, false);

        if (t <= 0) {
            return -1;
        }

        if (level == 0 ) {
            return t;
        }


        for (int i=0; i<3; i++) {
            current_color[i] = color[i] * co_efficients[AMBIENT];
        }


        //cout<<t<<endl;
        Point3 intersectionPoint = ray->start + ray->dir * t;

        Point3 normal = getNormal(intersectionPoint);

        Point3 reflection = getReflection(ray, normal);

        Point3 refraction = getRefraction(ray, normal);

        for (int i=0; i<lights.size(); i++) {

            Point3 dir = lights[i] - intersectionPoint;
            double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
            dir = dir.normalize();

            Point3 start = intersectionPoint + dir*1.0;
            Ray L(start, dir);
            //cout<<intersectionPoint<<L.start<<L.dir;

            bool flag = false;

            for (int j=0; j < objects.size(); j++) {

                double tObj = objects[j]->getIntersectionT(&L, true);

                if(tObj > 0 || abs(tObj) > len) {
                    continue;
                }

                flag = true;
                break;
            }

            if (flag){

                double lambert = dot(L.dir, normal);
                double phong = pow(dot(reflection, ray->dir), shine);

                lambert = lambert > 0 ? lambert : 0;
                phong = phong > 0 ? phong : 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[DIFFUSE] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[SPECULAR] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                int nearest=-1;
                double minT = 9999999;
                double reflected_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&reflectionRay, true);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                    }
                }

                start = intersectionPoint + refraction * 1.0;

                Ray refractionRay(start, refraction);

                nearest=-1;
                minT = 9999999;
                double refracted_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&refractionRay, true);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&refractionRay, refracted_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += refracted_color[k] * refIdx;
                    }
                }
            }

            for (int k=0; k<3; k++) {
                if (current_color[k] > 1) {
                    current_color[k] = 1;
                } else if (current_color[k] < 0) {
                    current_color[k] = 0;
                }
            }


        }

        return t;
    }



};

#endif // BASE_HPP_INCLUDED
