#ifndef _OUTLINE_H_
#define _OUTLINE_H_

#include <iostream>


#define NO_X_ROOT 1
#define NO_Y_ROOT 2
#define X_ALIGNED 4
#define Y_ALIGNED 8
#define IS_INSERTION 16
#define IS_EDGE 32

struct line_seg
{
  int x0;
  int y0;
  double x_offset=0;
  double y_offset=0;
  char flags=0;
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
    bool bEdge=false;

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

        //b/=a;
        //c/=a;
        //d/=a;

        term[0]=a;
        term[1]=b;
        term[2]=c;
        term[3]=d;
    }
     poly_form_y(double* term,double offset)
    {
        double a = (ay+offset)*(-1) + (by+offset)*(3) + (cy+offset)*(-3) + (dy+offset)*(1);
        double b = (ay+offset)*3+     (by+offset)*(-6)+ (cy+offset)*3+0;
        double c = (ay+offset)*(-3) +( by+offset)*3;
        double d = (ay+offset);

        //b/=a;
        //c/=a;
        //d/=a;

        term[0]=a;
        term[1]=b;
        term[2]=c;
        term[3]=d;
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

class NoiseMap;

class Outline
{
    public:
    const int MapPixels;
    const int PathResolution;
    char* m_map=NULL;
    int* m_pathmap=NULL;
    NoiseMap* m_heightmap=NULL;
    int m_height=0;
    int trace_area=0;

    line_seg m_trace[3000];
    int max_trace=3000;
    int n_trace;
    int steps;
    bool bHasEdges=false;

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
void shift_trace(int z);

bezier_object MakeBezierObject();

int cubic_intersect(double* res ,double* points,int nPoints);
void postprocess();

void fill_polygon(int x0,int y0,int h1, int h2);
void fill_polygon(int x0,int y0);
void fill_line_up(int x0,int x1,int y0);
void fill_line_down(int x0,int x1,int y0);
void trace(int x0,int y0);

void open0(int x0,int y0);
void open1(int x0,int y0);
void open3(int x0,int y0);
void open2(int x0,int y0);
void init(NoiseMap& hMap,unsigned char height,unsigned char base_height);

void finalize_map(NoiseMap* hMap, unsigned char height);
};


class polynomial3
{
public:
    int start_x=0;
    int end_x=100;

    double a;
    double b;
    double c;
    double d;

    int increment;

    polynomial3() {}
    polynomial3(double aa,double bb,double cc, double dd) : a(aa),b(bb),c(cc),d(dd) {}

    void rewind(unsigned) {/*cout<<"rewind: "<<start_x<<" to "<<end_x<<"\n";*/increment=start_x;}
    void clip(int x,int y)
    {
     start_x=x;
     end_x=y;
    }
    double virtual test(double x)
    {
        //ASSERT(x>=0 && x<512);
        return (d*(x)*(x)*(x)+c*(x)*(x)+b*(x)+a);
    }
    double virtual test1d(double x)
    {
       // ASSERT(x>=0 && x<512);
        return (d*(x)*(x)*3+c*(x)*2+b);
    }
/*
    unsigned virtual vertex(double* x,double* y)
    {
        if(increment >= end_x+1) return agg::path_cmd_stop;

        *x=increment;
        *y=(d*(*x)*(*x)*(*x)+c*(*x)*(*x)+b*(*x)+a);



        if(increment==start_x)
        {
           // cout<<"test "<<*x<<" "<<this->test(*x)<<"\n";
          //  cout<<increment<<"- move "<<*x<<" "<<*y<<"\n";
            ++increment;
            return agg::path_cmd_move_to;
        }
     // cout<<increment<<"- line "<<*x<<" "<<*y<<"\n";
        ++increment;
        return agg::path_cmd_line_to;
    }
    */
};

polynomial3 FitPoly(double* points, int nPoints, int k=4);


#endif
