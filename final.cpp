#include <windows.h>
#include <gl/Gl.h>
#include <gl/glu.h>
#include "glut.h"
#include <iostream>
#include <math.h>
using namespace std;

/*

g++ -o final.exe -Wall final.cpp glut32.lib -lopengl32 -lglu32

*/


class Point3
{
    public:
        float x, y, z;

    Point3() {};
    Point3(float x, float y, float z) : x(x), y(y), z(z) {}

    void set(Point3 p) { x = p.x; y = p.y; z = p.z; }
};

double SX = 1, SY = 1, SZ = 1;

class Vector3 {

    public:

        // -------------------- Attributes -------------------- //

        // Components of the vector
        float x, y, z;

        // -------------------- Methods -------------------- //

        // Constructor
        Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

        // Constructor
        Vector3(const Vector3& vector) : x(vector.x), y(vector.y), z(vector.z) {}
        //return the s vector from two points, shape, sun/eye
        Vector3(Point3 a, Point3 b) : x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) {}
        // Constructor
        ~Vector3() {}


        //set vector with floats
        void set(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }

        //set vector with a point3
        void set(Point3 p) { x = p.x; y = p.y; z = p.z; }

        //set vector with a vector
        void set(Vector3 v) { x = v.x; y = v.y; z = v.z; }

        //return the opposite vector of the given vector
        Vector3 negative() { return Vector3(-1*x, -1*y, -1*z); }

        // = operator
        Vector3& operator=(const Vector3& vector) {
            if (&vector != this) {
                x = vector.x;
                y = vector.y;
                z = vector.z;
            }
            return *this;
        }

        // + operator
        Vector3 operator+(const Vector3 &v) const {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        // += operator
        Vector3& operator+=(const Vector3 &v) {
            x += v.x; y += v.y; z += v.z;
            return *this;
        }

        // - operator
        Vector3 operator-(const Vector3 &v) const {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        // -= operator
        Vector3& operator-=(const Vector3 &v) {
            x -= v.x; y -= v.y; z -= v.z;
            return *this;
        }

        // == operator
        bool operator==(const Vector3 &v) const {
            return x == v.x && y == v.y && z == v.z;
        }

        // != operator
        bool operator!=(const Vector3 &v) const {
          return !( *this == v );
        }

        // * operator
        Vector3 operator*(float f) const {
            return Vector3(f*x, f*y, f*z);
        }

        // *= operator
        Vector3 &operator*=(float f) {
            x *= f; y *= f; z *= f;
            return *this;
        }

        // / operator
        Vector3 operator/(float f) const {
            float inv = 1.f / f;
            return Vector3(x * inv, y * inv, z * inv);
        }

        // /= operator
        Vector3 &operator/=(float f) {
            float inv = 1.f / f;
            x *= inv; y *= inv; z *= inv;
            return *this;
        }

        // - operator
        Vector3 operator-() const {
            return Vector3(-x, -y, -z);
        }


        // Cross product operator
        Vector3 cross(const Vector3 &v) const{
            return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
        }

        // Dot product operator
        float dot(const Vector3 &v) const{
            return x * v.x + y * v.y + z * v.z;
        }

        // Normalize the vector and return it
        Vector3 normalize() {
            float l = magnitude();

            x /= l;
            y /= l;
            z /= l;
            return *this;
        }

        // Return the squared length of the vector
        float lengthSquared() const { return x*x + y*y + z*z; }

        // Return the length of the vector
        float magnitude() const { return sqrt(lengthSquared()); }

};

class Dino
{
    public:
        Point3 center;
        Dino() {};
        Dino(Point3 p) {center.x = p.x; center.y = p.y; center.z = p.z; }
        void drawLegs();
        void drawBody();
        void drawHead();
        void drawDino();
        //void turn();
        void move(Point3 p) { center = p; }

        //see if I can just use these functions to move dino
        void slide(float delU, float delV, float delN);

        //---------
        //Point3 eye, look;
        Vector3 u, v, n, up;
        double viewAngle, aspect, nearDist, farDist; // view volume shape
        bool turn = false;
        double turnAngle = 0;

        void set(Point3 center, Point3 look, Vector3 up);
        void set(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
        //-------------

};

Dino dino;

void Dino::set(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{

    Point3  Dino  =  Point3(x1,y1,z1);
    Point3  Look =  Point3(x2,y2,z2);
    Vector3 Up   =  Vector3(x3,y3,z3);

    //create a modelview matrix and send it to opengl
    center.set(Dino); // store the given center position
    
    n.set(center.x - Look.x, center.y - Look.y, center.z - Look.z); //make n

    u.set(Up.cross(n)); //make u = up cross n (cross product)
    n.normalize(); 
    u.normalize(); //make them unit length
    v.set(n.cross(u));

}
    
void Dino::slide(float delU, float delV, float delN)
{
    center.x  += delU * u.x + delV * v.x + delN * n.x;
    center.y  += delU * u.y + delV * v.y + delN * n.y;
    center.z  += delU * u.z + delV * v.z + delN * n.z;
}

void drawHalfSphere(int scaley, int scalex, GLfloat r) 
{
    glColor3f(.7,.7,.9);
    int i, j;
    GLfloat v[scalex*scaley][3];

    for(i = 0; i < scalex; ++i) 
        for(j = 0; j < scaley; ++j) 
        {

            v[i*scaley+j][0]=r*cos(j*2*M_PI/scaley)*cos(i*M_PI/(2*scalex));
            v[i*scaley+j][1]=r*sin(i*M_PI/(2*scalex));
            v[i*scaley+j][2]=r*sin(j*2*M_PI/scaley)*cos(i*M_PI/(2*scalex));

        }


    glBegin(GL_QUADS);
    for(i = 0; i < scalex-1; ++i) 
        for(j = 0; j < scaley; ++j) 
        {
            glVertex3fv(v[i*scaley+j]);
            glVertex3fv(v[i*scaley+(j+1)%scaley]);
            glVertex3fv(v[(i+1)*scaley+(j+1)%scaley]);
            glVertex3fv(v[(i+1)*scaley+j]);
        }

    glEnd();

}

void Dino::drawHead()
{
    //head
    glPushMatrix();
        glColor3f(.4,.6,1);
        glTranslated(center.x+3,center.y+1.5,center.z);
        glScaled(3,3,3);
        glutSolidSphere(.6,10,10);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0,.075,0);
        glTranslated(center.x+3,center.y+3.25,center.z+.2);
        glRotated(-78.5, 1, 0, 0);
        glutSolidCone(.5,.9,7,7);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0,.4,0);
        glTranslated(center.x+3,center.y+4.10,center.z+.35);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //right eye
    glPushMatrix();
        glColor3f(0,0,0);
        glTranslated(center.x+4.45,center.y+2,center.z-.5);
        glScaled(.5,.5,.5);
        glutSolidSphere(.45,10,10);
    glPopMatrix();

    //left eye
    glPushMatrix();
        glColor3f(0,0,0);
        glTranslated(center.x+4.45,center.y+2,center.z+.5);
        glScaled(.5,.5,.5);
        glutSolidSphere(.45,10,10);
    glPopMatrix();

    //mouth
    glPushMatrix();
        glColor3f(0,0,0);
        glTranslated(center.x+4.55,center.y+1.50,center.z);
        glScaled(.5,.5,.5);
        glutSolidSphere(.75,10,10);
    glPopMatrix();
}


void Dino::drawBody()
{
    //plate by head
    glColor3f(0,.2,1);
    glPushMatrix();
        glTranslated(center.x+2.00,center.y+4.30,center.z);
        glutSolidSphere(1,8,4);
    glPopMatrix();

    //middle plate
    glPushMatrix();
        glTranslated(center.x-.5,center.y+5.5,center.z);
        glutSolidSphere(1,8,4);
    glPopMatrix();

    //lower back plate
    glPushMatrix();
        glTranslated(center.x-3,center.y+4.95,center.z);
        glutSolidSphere(.95,8,4);
    glPopMatrix();

    //1st tail plate
    glPushMatrix();
        glTranslated(center.x-4.75,center.y+3,center.z);
        glutSolidSphere(.85,8,4);
    glPopMatrix();

    //draw tail
    glColor3f(.4,.6,1);
    glPushMatrix();
        glTranslated(center.x-3,center.y+1.5,center.z+.25);
        glRotatef(270,0.0f,2.0f,0.0f); 
        glutSolidCone(2,4,6,6);
    glPopMatrix();

    //draw tail whacky thingy
    glPushMatrix();
        glColor3f(0,.2,1);
        glTranslated(center.x-6.75,center.y+1.8,center.z+.25);
        glutSolidSphere(.8,6,6);
    glPopMatrix();

    //draw body
    glColor3f(.5,.6,.9);
    glPushMatrix();
        glTranslated(center.x-1,center.y+1,center.z);
        drawHalfSphere(7,7,4);
    glPopMatrix();

}



void Dino::drawLegs()
{

    //front right foot
    glColor3f(.7,.7,0);
    glPushMatrix();
        glTranslated(center.x+2,center.y,center.z+2);
        glutSolidSphere(1,10,10);
    glPopMatrix();

    //front left leg
    glPushMatrix();
        glTranslated(center.x+2,center.y,center.z-2);
        glutSolidSphere(1,10,10);
    glPopMatrix();

    //back right foot
    glPushMatrix();
        glTranslated(center.x-2,center.y,center.z+3);
        glutSolidSphere(1,10,10);
    glPopMatrix();

    //back left foot
    glPushMatrix();
        glTranslated(center.x-2,center.y,center.z-3);
        glutSolidSphere(1,10,10);
    glPopMatrix();


    //rollerskate1.1 hehe
    glColor3f(0,0,0);
    glPushMatrix();
        glTranslated(center.x+2.25,center.y-1.17,center.z+2.3);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //rollerskate1.2 hehe
    glPushMatrix();
        glTranslated(center.x+2.25,center.y-1.17,center.z+1.7);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //rollerskate1.3 hehe
    glPushMatrix();
        glTranslated(center.x+1.75,center.y-1.17,center.z+2.3);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //rollerskate1.4 hehe
    glPushMatrix();
        glTranslated(center.x+1.75,center.y-1.17,center.z+1.7);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //rollarskatestrap1.1 :p
    glPushMatrix();
        glTranslated(center.x+2.75,center.y+.6,center.z+2);
        glScaled(.25,.25,.75);
        glutSolidCube(1);
    glPopMatrix();

    //rollarskatestrap1.2 :p
    glPushMatrix();
        glTranslated(center.x+2.9,center.y+.2,center.z+2);
        glScaled(.25,.25,.75);
        glutSolidCube(1);
    glPopMatrix();

   
    //rollerskate2.1 hehe
    glColor3f(0,0,0);
    glPushMatrix();
        glTranslated(center.x+2.25,center.y-1.17,center.z-2.3);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //rollerskate2.2 hehe
    glPushMatrix();
        glTranslated(center.x+2.25,center.y-1.17,center.z-1.7);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //rollerskate2.3 hehe
    glPushMatrix();
        glTranslated(center.x+1.75,center.y-1.17,center.z-2.3);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //rollerskate2.4 hehe
    glPushMatrix();
        glTranslated(center.x+1.75,center.y-1.17,center.z-1.7);
        glutSolidSphere(.2,10,10);
    glPopMatrix();

    //rollarskatestrap2.1 :p
    glPushMatrix();
        glTranslated(center.x+2.75,center.y+.6,center.z-2);
        glScaled(.25,.25,.75);
        glutSolidCube(1);
    glPopMatrix();

    //rollarskatestrap2.2 :p
    glColor3f(0,0,0);
    glPushMatrix();
        glTranslated(center.x+2.9,center.y+.2,center.z-2);
        glScaled(.25,.25,.75);
        glutSolidCube(1);
    glPopMatrix();



    

}

void Dino::drawDino()
{
    
    glTranslated(dino.center.x,dino.center.y,dino.center.z);
    glRotated(dino.turnAngle, 0, 1, 0);
    glTranslated(-dino.center.x,-dino.center.y,-dino.center.z);
    

    //draw the dino
    this->drawLegs();
    this->drawBody();
    this->drawHead();
}

class Camera {
    public:
        Point3 eye, look;
        Vector3 u, v, n, up;
        double viewAngle, aspect, nearDist, farDist; // view volume shape
        void setModelviewMatrix(); //tell openGL where the camera is

    
        Camera() {};
        void set(Point3 eye, Point3 look, Vector3 up);
        void set(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
        void roll(float angle);
        void pitch(float angle);
        void yaw(float angle);
        void slide(float delU, float delV, float delN);
        void setShape(float vAng, float asp, float nearD, float farD);
        void getShapes(float &vAng, float &asp, float &nearD, float &farD);
};

Camera cam;

void Camera::setModelviewMatrix(void)
{
    //load modelview matrix with existing camera values
    float m[16];
    Vector3 eVec(eye.x, eye.y, eye.z); //constructor of a vector version of eye

    m[0] = u.x; m[4] = u.y; m[8]  = u.z; m[12] = -eVec.dot(u);
    m[1] = v.x; m[5] = v.y; m[9]  = v.z; m[13] = -eVec.dot(v);
    m[2] = n.x; m[6] = n.y; m[10] = n.z; m[14] = -eVec.dot(n);
    m[3] = 0;   m[7] = 0;   m[11] = 0;   m[15] = 1.0;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m); //load openGL modelview matrix
}

void Camera::set(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{

    Point3  Eye  =  Point3(x1,y1,z1);
    Point3  Look =  Point3(x2,y2,z2);
    Vector3 Up   =  Vector3(x3,y3,z3);

    //create a modelview matrix and send it to opengl
    eye.set(Eye); // store the given eye position
    
    n.set(eye.x - Look.x, eye.y - Look.y, eye.z - Look.z); //make n

    u.set(Up.cross(n)); //make u = up cross n (cross product)
    n.normalize(); 
    u.normalize();      //make them unit length
    v.set(n.cross(u));


    setModelviewMatrix(); //tell openGL

}

void Camera::setShape(float vAng, float asp, float nearD, float farD) 
{

    viewAngle = vAng;
    aspect = asp;
    nearDist = nearD;
    farDist = farD; 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(vAng, asp, nearD, farD);

}
    

void Camera::slide(float delU, float delV, float delN)
{
    eye.x  += delU * u.x + delV * v.x + delN * n.x;
    eye.y  += delU * u.y + delV * v.y + delN * n.y;
    eye.z  += delU * u.z + delV * v.z + delN * n.z;

    look.x  += delU * u.x + delV * v.x + delN * n.x;
    look.y  += delU * u.y + delV * v.y + delN * n.y;
    look.z  += delU * u.z + delV * v.z + delN * n.z;

    setModelviewMatrix();
}

void Camera::roll(float angle)
{
    //roll the camera through angle degrees
    float pinum = 3.14159265/180 * angle;

    float cs = cos(pinum);
    float sn = sin(pinum);

    Vector3 t( u );
    u.set(cs*t.x - sn*v.x, cs*t.y - sn*v.y, cs*t.z - sn*v.z);
    v.set(sn*t.x + cs*v.x, sn*t.y + cs*v.y, sn*t.z + cs*v.z);

    setModelviewMatrix();
}

void Camera::yaw(float angle)
{
    //roll the camera through angle degrees
    float pinum = 3.14159265/180 * angle;

    float cs = cos(pinum);
    float sn = sin(pinum);

    Vector3 t( n );
    n.set(cs*t.x - sn*u.x, cs*t.y - sn*u.y, cs*t.z - sn*u.z);
    u.set(sn*t.x + cs*u.x, sn*t.y + cs*u.y, sn*t.z + cs*u.z);

    setModelviewMatrix();
}

void Camera::pitch(float angle)
{
    //roll the camera through angle degrees
    float pinum = 3.14159265/180 * angle;

    float cs = cos(pinum);
    float sn = sin(pinum);

    Vector3 t(v);
    v.set(cs*t.x - sn*n.x, cs*t.y - sn*n.y, cs*t.z - sn*n.z);
    n.set(sn*t.x + cs*n.x, sn*t.y + cs*n.y, sn*t.z + cs*n.z);

    setModelviewMatrix();
}


//Is is the intensity of the light source
//Pd is the diffuse reflection coefficent
//Returns Id
double lambert(Vector3 s, Vector3 m) {
    float top = s.dot(m);
    float bottom = s.magnitude() * m.magnitude();

    float n = top/bottom;
    float lambert = max((float)0, n);

    //Is and Pd are global
    //return Is * Pd * lambert;;
    return lambert;;
}

//Is is the intensity of the light source
//Ps is the specular reflection coefficent
//Adjusted Specular term
//returns Isp
double phong(Vector3 v, Vector3 s, Vector3 m, int f) {
        
    Vector3 h = s + v;

    //Is this correctly translated? pg 387
    Vector3 left = (h/h.magnitude());
    Vector3 right = (m/m.magnitude());
    float frac = left.dot(right);

    //pick an f value 1-200
    float n = pow(frac,f);
    float phong = max((float)0, n);

    //Is and Ps are global
    //return Is * Ps * phong;
    return phong;
}

//-s + 2[(2dotm)/|m|^2]*m finding vector r... mirrior reflection direction
Vector3 getR(Vector3 s, Vector3 m)
{
    double top = s.dot(m);
    double bottom = pow(m.magnitude(),2);
    double fraction = top/bottom;
 
    return s.negative() + (m*(2*fraction));

}

//vector from shape --> sun/eye
Vector3 getSV(Point3 shape, Point3 sun)
{
    return Vector3(shape, sun);
}



double light(Vector3 s, Vector3 m, Vector3 v, double Ia, double Pa, double Id, double Pd, double Is, double Ps, int f) {
    
    return (Ia * Pa) + (Id * Pd * lambert(s,m)) + ( Is * Ps * phong(v, s, m, f));

}

void axis(double length)
{
    glPushMatrix();
        glBegin(GL_LINES);
            //x is red
            glColor3d(1,0,0);
            glVertex3d(0, 0, 0);
            glVertex3d(length,0,0);

            //y is green
            glColor3d(.5,1,.5);
            glVertex3d(0, 0, 0);
            glVertex3d(0, length, 0);

            //z is blue
            glColor3d(0,0,1);
            glVertex3d(0, 0, 0);
            glVertex3d(0, 0, length);
        glEnd();

    glPopMatrix();

    
}
  
Point3 sun(5,5,5);
double sunNum = 0;
void display(void)
{
    GLfloat pos[]  = {sun.x,sun.y,sun.z,1.0};
    GLfloat amb[]  = {.8,.9,.5,0};
    GLfloat dif[]  = {1.0,.8,1.0,0};
    GLfloat spec[] = {1.0,.8,1.0,0};
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glPopMatrix();

    //Ground Plane
    glPushMatrix();
        glColor4d(0,.4,0,0);
        glTranslated(-3,-3,-3);
        glScaled(100,1,100);
        glutSolidCube(2);       
    glPopMatrix();
    
    //draw axis lines, x = red, y = green, z = blue
    glPushMatrix();
        axis(1);
    glPopMatrix();

    //draw dino function
    glPushMatrix();
        glScaled(.2,.2,.2);       
        dino.drawDino();
    glPopMatrix();

    double x[] = { 1, 2,-3, 4, -4, 3, 3, -5, -2, -1};
    double z[] = { 3,-4, 3,-3, -1,-5, 2,  1,  4, -2};
    for(int i = 0; i < 10; i++)
    {
        //base log
        glPushMatrix();
            glColor3f(.2,.1,.1);
            glTranslated(x[i],0,z[i]);
            glScaled(.3,3,.3);
            glutSolidCube(.5);
        glPopMatrix();
        double n = 0;
        for(int j = 0; j < 3; j++)
        {
            //leaves
            glPushMatrix();
                glColor3f((double)i/20.0,n+.1,(double)i/20.0);
                glTranslated(x[i],n,z[i]);
                glRotated(-90, 1, 0, 0);
                glutSolidCone(.5,.5,5,5);
            glPopMatrix();
            n+=.4;
        }
    }

    glFlush();

}
int follow = 0;
int lightToggle = 1;
void keyboardDrawPrompt(unsigned char key, int xmouse, int ymouse) {
	switch(key)
	{
        //camera controls
        case 'a' :  cam.yaw(-2.0);          break; 
        case 's' :  cam.yaw(2.0);           break; 
        case 'e' :  cam.pitch(-2.0);        break; 
        case 'd' :  cam.pitch(2.0);         break; 
        case 'q' :  cam.roll(-2.0);         break; 
        case 'w' :  cam.roll(2.0);          break; 
        case 'D' :  cam.slide(0.2, 0, 0);   break;
        case 'S' :  cam.slide(0, 0, 0.2);   break; 
        case 'A' :  cam.slide(-0.2, 0, 0);  break;
        case 'W' :  cam.slide(0, 0, -0.2);  break; 
        case 'E' :  cam.slide(0.0, .2, 0);  break;
        case 'Q' :  cam.slide(0.0, -.2, 0); break;
        case 'f' :  follow ? follow = false : follow = true; if(follow) { cout << "Following Dino: ON\n"; } else{ cout << "Following Dino: OFF\n"; } break;

        //dino controls on its own axis
        case 'j':  dino.turnAngle += 10; break; 
        case 'n':  dino.turnAngle -= 10; break;
        case 'k':  dino.slide(0.0, .2, 0);  break;
        case 'm' : dino.slide(0.0, -.2, 0); break;

        //light controls L
        case 'u': sun.x = 13*cos(sunNum); sun.y = 13*sin(sunNum);  sunNum+=.2; break; 
        case 'p' :  lightToggle ? lightToggle = false : lightToggle = true; if(lightToggle){cout << "Light Source: ON\n";}   else{ cout << "Light Source: OFF\n";   }   break;


        case 27: exit(1);

    }

    if(follow)
        cam.set(cam.eye.x,cam.eye.y,cam.eye.z,dino.center.x*.2,dino.center.y*.2,dino.center.z*.2,0,1,0);

    if(lightToggle)
        glEnable(GL_LIGHT0);
    else 
        glDisable(GL_LIGHT0);

    //call the redraw function
    glutPostRedisplay();
}

void SpecialKeys(int key, int x, int y)
{
    switch (key)
    {
        //dino controls
        case GLUT_KEY_LEFT:   dino.slide(0.2, 0, 0); break;
        case GLUT_KEY_UP:     dino.slide(0, 0, 0.2); break; 
        case GLUT_KEY_RIGHT:  dino.slide(-0.2, 0, 0);  break;
        case GLUT_KEY_DOWN:   dino.slide(0, 0, -0.2);  break;  
    }

    if(follow)
        cam.set(cam.eye.x,cam.eye.y,cam.eye.z,dino.center.x*.2,dino.center.y*.2,dino.center.z*.2,0,1,0);

    glutPostRedisplay();

}

void myInit(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);   // set the bg color to white
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    //eye, look, up
    cam.setShape(30.0, 64.0/48.0, .5, 100.0);
    cam.set(10,10,10,0,0,0,0,2,0);
    dino.set(3,0,0,0,0,0,0,1,0);

}


// Runs the code setting the GL functions to the appropriate from above
//<<<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char **argv) {
		
    cout << "\nDino Controls\n";
    cout << "Arrow keys move foward/backward/left/right\n";
    cout << "'k'/'m' up and down\n";
    cout << "'n'/'j' rotate\n\n";

    cout << "Camera Controls\n";
    cout << "'W'/'A'/'S'/'D' move the camera\n";
    cout << "'f' toggle lock on\n";
    cout << "'q'/'w' roll\n";
    cout << "'a'/'s' yaw\n";
    cout << "'e'/'d' pitch\n\n";

    cout << "Light Controls\n";
    cout << "'u' translate\n";
    cout << "'p' toggle On/Off\n";
 

	glutInit(&argc, argv);                       // initialize the toolkit
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB); // set the display mode
	//glutInitWindowSize(900,500);               // set the window size
	glutInitWindowSize(680,480);                 // set the window size
	//glutInitWindowPosition(1000, 0);           // set the window position on the screen
	glutInitWindowPosition(680, 0);              // set the window position on the screen
	glutCreateWindow("Light");                   // open the screen window(with its exciting title)
    glutDisplayFunc(display);                    // register the redraw function
    glutKeyboardFunc(keyboardDrawPrompt);        // register the keyboard action function
    glutSpecialFunc(SpecialKeys);
    myInit();
	glutMainLoop(); 		                     // go into a perpetual loop
	
}


