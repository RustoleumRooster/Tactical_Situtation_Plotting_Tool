#ifndef _NOISEMAP_H_
#define _NOISEMAP_H_
#include <iostream>



class XVector
{
    public:
    float X;
    float Y;
    XVector()
    {}
    XVector(float x,float y)
    {
        X=x;
        Y=y;
    }
    float length();
    XVector normalize()
    {
        float l = this->length();
        X = X/l;
        Y = Y/l;
        return *this;
    }
    XVector scale(float f)
    {
        X = X*f;
        Y = Y*f;
        return *this;
    }
};

static double lerp(double t, double a, double b) { return a + t * (b - a); }
static double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }

/*
const int MapPixels = 512;
const int GridResolution = 128;
const int GridIntegrals = 4;
*/

class NoiseMap
{
public:

    const int MapPixels;
    const int GridResolution;
    const int GridIntegrals;

//    char m_map[MapPixels][MapPixels];
  //  XVector m_Vectors[GridIntegrals+1][GridIntegrals+1];

   char* m_map = NULL;
   float* m_fmap = NULL;
   XVector* m_Vectors = NULL;

    NoiseMap():
        MapPixels(512),
        GridResolution(32),
        GridIntegrals(16)
    {
    }
    NoiseMap(int pixels,int grid_integrals):
        MapPixels(pixels),
        GridResolution(pixels/grid_integrals),
        GridIntegrals(grid_integrals)
    {
    }
    ~NoiseMap()
    {
        if( m_map != NULL)
            delete[] m_map;
        if( m_fmap != NULL)
            delete[] m_fmap;
        if(m_Vectors != NULL)
            delete[] m_Vectors;
    }
    void initGrid();
    void init(float seed);
    void addNoise(NoiseMap &Map2,float scale,float offset = -0.5);
    void rescale();
    void postprocess();
};
/*
struct line_seg
{
  int x0;
  int y0;
  //int slope;
};*/

struct line_seg
{
  int x0;
  int y0;
  double slope;
  bool key=false;
};

class cubic_bezier
{
public:
    double ax;
    double ay;
    double bx;
    double by;
    double cx;
    double cy;
    double dx;
    double dy;

    double test_x(double t)
    {
        double t2=t*t;
        double t3 = t*t*t;
        return ax*(1-3*t+3*t2-t3)  +
            bx*3*(1-2*t+t2)*t   +
            cx*3*(1-t)*t2       +
            dx*t3;
    }
    double test_y(double t)
    {
        double t2=t*t;
        double t3 = t*t*t;
        return ay*(1-3*t+3*t2-t3)  +
            by*3*(1-2*t+t2)*t   +
            cy*3*(1-t)*t2       +
            dy*t3;
    }
    poly_form_x(double* term,double offset)
    {
        double a = (ax+offset)*(-1) + (bx+offset)*(3) + (cx+offset)*(-3) + (dx+offset)*(1);
        double b = (ax+offset)*3+     (bx+offset)*(-6)+ (cx+offset)*3+0;
        double c = (ax+offset)*(-3) +( bx+offset)*3;
        double d = (ax+offset);

        b/=a;
        c/=a;
        d/=a;

        term[0]=b;
        term[1]=c;
        term[2]=d;
    }
     poly_form_y(double* term,double offset)
    {
        double a = (ay+offset)*(-1) + (by+offset)*(3) + (cy+offset)*(-3) + (dy+offset)*(1);
        double b = (ay+offset)*3+     (by+offset)*(-6)+ (cy+offset)*3+0;
        double c = (ay+offset)*(-3) +( by+offset)*3;
        double d = (ay+offset);

        b/=a;
        c/=a;
        d/=a;

        term[0]=b;
        term[1]=c;
        term[2]=d;
    }
    /*
    double calc_length()
    {
        double ret=0;
        for(int i=0;i<25;i++)
        {
            double t = (double)i/25;
            ret+=std::sqrt((test_x((double)i/25)-(double)(i+1)/25)*(test_x((double)i/25)-(double)(i+1)/25)
                           +(test_y((double)i/25)-(double)(i+1)/25)*(test_y((double)i/25)-(double)(i+1)/25));
        }
        return ret;
    }
    */
    /*
     double calc_Length()
    {

        double r=0.0;
        double len=0;

        for(int i=1;i<=25;i++)
        {
            double t=(double)i/25;
            len+=sqrt((test_x(t)-test_x(r))*(test_x(t)-test_x(r))+((test_y(t)-test_y(r))*(test_y(t)-test_y(r))));
            r=t;
        }
        return len;

    }
    */
};

class bezier_object
{
public:
    cubic_bezier m_bez[400];
    int n_bez;
};

class Outline
{
    public:
    const int MapPixels;
    const int PathResolution;
    char* m_map=NULL;
    int* m_pathmap=NULL;

    line_seg m_trace[3000];
    int max_trace=3000;
    int n_trace;
    int steps;

    int focus_x;
    int focus_y;
    Outline(int pixels):
        MapPixels(pixels),
        PathResolution(8),
        focus_x(-1),
        focus_y(-1),
        n_trace(0)
    {

    }
    ~Outline()
    {
        if(m_map)
            delete[] m_map;
        if(m_pathmap)
            delete[] m_pathmap;
    }
    void set_focus(int x,int y)
    {
        focus_x=x;
        focus_y=y;
    }

bezier_object MakeBezierObject();
void fill_polygon(int x0,int y0);
void fill_line_up(int x0,int x1,int y0);
void fill_line_down(int x0,int x1,int y0);
void trace(int x0,int y0);
void simplify();
void simplify2();
void open0(int x0,int y0);
void open1(int x0,int y0);
void open3(int x0,int y0);
void open2(int x0,int y0);
void init(NoiseMap& hMap,unsigned char height);

};





#endif
