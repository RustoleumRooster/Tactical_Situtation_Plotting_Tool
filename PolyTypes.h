#ifndef _POLY_TYPES_H_
#define _POLY_TYPES_H_

#define PI 3.1415926539

#include <iostream>
#include "agg_basics.h"
#include "Outline.h"

#define CLIP_BORDER 32

struct Scene
{
    double x0;
    double y0;
    double x1;
    double y1;
    unsigned width;
    unsigned height;
};

extern int n_vertexes;
extern bool paused;

extern int screen_corners[18];
/* ={  64,64,
                        64,512+64,
                        512+64,512+64,
                        64+512,64,
                        64,64,
                        64,512+64,
                        512+64,512+64,
                        64+512,64,
                        64,64
                        };
*/

extern unsigned N_STEPS;

class BezierPoly {
public:
    unsigned increment;
    unsigned nPoints;
    double ax,ay,bx,by,cx,cy,dx,dy,t0,t1;
    BezierPoly(){}
    BezierPoly(double x1,double x2,double x3, double x4,
             double y1,double y2,double y3, double y4,
             double t,double tt,unsigned n) : ax(x1),bx(x2),cx(x3),dx(x4),ay(y1),by(y2),cy(y3),dy(y4),t0(t),t1(tt),nPoints(n) {}
    ~BezierPoly(){}
    Set(double x1,double x2,double x3, double x4,
             double y1,double y2,double y3, double y4,
             double t,double tt,unsigned n)
             {
                 ax=x1;
                 bx=x2;
                 cx=x3;
                 dx=x4;
                 ay=y1;
                 by=y2;
                 cy=y3;
                 dy=y4;
                 t0=t;
                 t1=tt;
                 nPoints=N_STEPS;
             }

    void rewind(unsigned) {increment=0;}

    unsigned vertex(double* x,double* y);

};
/*
class BezierPoly {
public:
    unsigned increment;
    unsigned nPoints;
    double ax,ay,bx,by,cx,cy,dx,dy,t0,t1;
    BezierPoly(){}
    BezierPoly(double x1,double x2,double x3, double x4,
             double y1,double y2,double y3, double y4,
             double t,double tt,unsigned n) : ax(x1),bx(x2),cx(x3),dx(x4),ay(y1),by(y2),cy(y3),dy(y4),t0(t),t1(tt),nPoints(n) {}
    ~BezierPoly(){}
    Set(double x1,double x2,double x3, double x4,
             double y1,double y2,double y3, double y4,
             double t,double tt,unsigned n)
             {
                 ax=x1;
                 bx=x2;
                 cx=x3;
                 dx=x4;
                 ay=y1;
                 by=y2;
                 cy=y3;
                 dy=y4;
                 t0=t;
                 t1=tt;
                 nPoints=n;
             }

    void rewind(unsigned) {increment=0;}

    void set0(double x,double y)
    {
       // x0=x;
       // y0=y;
    }
    void set1(double xx, double yy)
    {
      //  x1=xx;
      //  y1=yy;
    }

    unsigned vertex(double* x,double* y);

};
*/



void set_screen_corners(Scene scene,int b);

class CurvePoly_solid {
public:
    unsigned increment;
    cubic_bezier m_bez[400];
    int n_bez;
    double roots[50];
    //int axis[25];
    int n_roots;

    int nIntervals;
    double interval[32];
    double z_pos[32];
    int range[32];
    int close_to[16];

    int step;
    int nSteps;
    double t0;
    double t1;
    int iInterval;

    int corners[16];

    void addCorner(int i,int z1,int z2)
    {
    corners[i*2]=z1;
    corners[(i*2)+1]=z2;
    }

    void addInterval(double x0, double x1, int a0, int a1, double z_start, double z_end)
    {
        interval[nIntervals*2]=x0;
        interval[(nIntervals*2)+1]=x1;
        range[nIntervals*2]=a0;
        range[(nIntervals*2)+1]=a1-a0;
        z_pos[nIntervals*2]=z_start;
        z_pos[(nIntervals*2)+1]=z_end;
       // std::cout<<"added: "<<nIntervals<<" "<<x0<<" "<<x1<<" "<<a0<<" "<<a1<<" "<<z_start<<" "<<z_end<<"\n";
        nIntervals++;
    }

    void swapIntervals(int i, int j)
    {
     double x0,x1,z0,z1;
     int a0,a1,close,c0,c1;
     x0=interval[i*2];
     x1=interval[(i*2)+1];
     a0=range[i*2];
     a1=range[(i*2)+1];
     z0=z_pos[i*2];
     z1=z_pos[(i*2)+1];
     close=close_to[i];
     c0=corners[i*2];
     c1=corners[(i*2)+1];

     interval[i*2]=interval[j*2];
     interval[(i*2)+1]=interval[(j*2)+1];
     range[i*2]=range[j*2];
     range[(i*2)+1]=range[(j*2)+1];
     z_pos[i*2]=z_pos[j*2];
     z_pos[(i*2)+1]=z_pos[(j*2)+1];
     close_to[i]=close_to[j];
     corners[i*2]=corners[j*2];
     corners[(i*2)+1]=corners[(j*2)+1];

     interval[j*2]=x0;
     interval[(j*2)+1]=x1;
     range[j*2]=a0;
     range[(j*2)+1]=a1;
     z_pos[j*2]=z0;
     z_pos[(j*2)+1]=z1;
     close_to[j]=close;
     corners[j*2]=c0;
     corners[(j*2)+1]=c1;
    }

    void rewind(unsigned) {increment=0;iInterval=0;step=0;nSteps=N_STEPS;}

    unsigned vertex(double* x,double* y);
};

class CurvePoly {
public:
    unsigned increment;
    cubic_bezier m_bez[400];
    int n_bez;
    double roots[50];
    char root_flag[50];
    int n_roots;
    int step;
    int nSteps;
    double t0;
    double t1;
    int iInterval;
    int nIntervals;
    double interval[32];
    int range[32];


    void addInterval(double x0, double x1, int a0, int a1)
    {
        interval[nIntervals*2]=x0;
        interval[(nIntervals*2)+1]=x1;
        range[nIntervals*2]=a0;
        range[(nIntervals*2)+1]=a1-a0;
        nIntervals++;
        //std::cout<<"addInterval: "<<x0<<","<<x1<<"  "<<a0<<","<<a1<<"\n";
    }

    void rewind(unsigned) {increment=0;iInterval=0;step=0;nSteps=N_STEPS;}
    unsigned vertex(double* x,double* y);
};

class LinePoly {
public:
    unsigned increment;
    double x0;
    double y0;
    double x1;
    double y1;
    LinePoly(){}
    LinePoly(double x,double y,double xx, double yy) : x0(x),y0(y),x1(xx),y1(yy) {}
    ~LinePoly(){}

    void rewind(unsigned) {increment=0;}

    void set0(double x,double y)
    {
        x0=x;
        y0=y;
    }
    void set1(double xx, double yy)
    {
        x1=xx;
        y1=yy;
    }

    unsigned vertex(double* x,double* y);

};

//arbitrary multiline poly
class ArbPoly {
public:
    unsigned increment;
    double point[1000];
    //double* point;
    unsigned nPoints;

    ArbPoly():nPoints(0){}//{point = new double[size*2];}
    ~ArbPoly(){}//{if(point!=NULL){}delete[] point;}

    void rewind(unsigned) {increment=0;}
    void AddPoint(double x,double y)
    {
        point[nPoints*2]=x;
        point[nPoints*2+1]=y;
        nPoints++;
    }
   /* void CheckAddPoint(double x,double y)
    {
        if(point[(nPoints-1)*2] == x &&
           point[(nPoints-1)*2+1] == y)
            return;

        point[nPoints*2]=x;
        point[nPoints*2+1]=y;
        nPoints++;
    }*/

    unsigned vertex(double* x,double* y);

};

class ArbPoly_solid {
public:
    unsigned increment;
    double point[1000];
    unsigned nPoints;
    ArbPoly_solid():nPoints(0){}
    //LinePoly(double x,double y,double xx, double yy) : x0(x),y0(y),x1(xx),y1(yy) {}
    ~ArbPoly_solid(){}

    void rewind(unsigned) {increment=0;}
    void AddPoint(double x,double y)
    {
        if(nPoints>0 && point[(nPoints-1)*2] == x && point[(nPoints-1)*2+1] == y)
            return;

        point[nPoints*2]=x;
        point[nPoints*2+1]=y;
        nPoints++;
    }
    unsigned vertex(double* x, double* y);
};


class Marker_dot_poly {
    public:
    unsigned increment;
    static const int nPoints= 4;
    static const int radius=4.5;
    double x0;
    double y0;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }

    unsigned vertex(double* x,double* y);

};

class Marker_dash_poly {
    public:
    unsigned increment;
    int length =5;
    static const int nPoints= 2;
    static const int radius=4.5;
    double x0;
    double y0;
    double rot;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }
    void setRotation(double r)
    {
        rot=r;
    }
    void setLength(double l)
    {
        length=l;
    }

    unsigned vertex(double* x,double* y);

};

class Marker_small_circle_poly {
    public:
    unsigned increment;
    static const int nPoints= 10;
    static const int radius=5;
    double x0;
    double y0;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }

    unsigned vertex(double* x,double* y);
};


class Marker_small_arrow_poly {
    public:
    unsigned increment;

    static const int length=10;
    double x0;
    double y0;
    double rot;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }
     void setRotation(double r)
    {
        rot=r;
    }
    unsigned vertex(double* x, double* y);

};

class Marker_square_poly {
    public:
    unsigned increment;

    static const int length=5;
    double x0;
    double y0;
    double rot;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }

    unsigned vertex(double* x, double* y);

};

class Marker_cross_poly {
    public:
    unsigned increment;

    static const int length=5;
    double x0;
    double y0;
    double rot;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }

    unsigned vertex(double* x, double* y);

};

/*

class ArcPoly_solid {
public:

    agg::point_d center;
    unsigned increment;
    unsigned arcInc;
    unsigned int nPoints;
    double theta[8];
    unsigned int nArcPoints[4];
    int nArcs;
    double radius;

    int cornerArc;
    int nCorners;
    double corner[8];

    ArcPoly_solid():nArcs(0),nPoints(5),nCorners(0){}
    ArcPoly_solid(agg::point_d cent,double r,int n,double t0,double t1)
        : increment(0),
        center(cent),
        radius(r),
        nPoints(n),
        nArcs(1),
        nCorners(0)
    {
        theta[0]=t0;
        theta[1]=t1;
    }
    ~ArcPoly_solid()
    {
    }
    void SwapCorners(int a,int b)
    {
            double temp = corner[a*2];
            corner[a*2]=corner[b*2];
            corner[b*2]=temp;
            temp = corner[a*2+1];
            corner[a*2+1]=corner[b*2+1];
            corner[b*2+1]=temp;
    }
    ArcPoly_solid& setNPoints(int n)
    {
        nPoints=n;
        return *this;
    }
    ArcPoly_solid& setPosition(double x,double y)
    {
        center.x=x;
        center.y=y;
        return *this;
    }
    ArcPoly_solid& setRadius(double r)
    {
        radius=r;
        return *this;
    }
    ArcPoly_solid& addArc(double t0,double t1)
    {
    if(nArcs<4)
        {
        theta[nArcs*2]=t0;
        theta[nArcs*2+1]=t1;
        nArcPoints[nArcs]=max(4,2+(int)(nPoints*(t1-t0)/(PI*2)));
        //cout<<nArcPoints[nArcs]<<"\n";
        nArcs++;
        }
    return *this;
    }

    void rewind(unsigned) {
        increment = 0;
        arcInc=0;
    }


    unsigned vertex(double* x, double* y) {
    //cout<<increment<<"\n";

        if(increment>100)
        {cout<<"run away!!\n";
        cout<<"arc "<<arcInc<<"\n";
        cout<<"corners "<<nCorners<<"\n";
        cout<<"corner arc "<<cornerArc<<"\n";
        paused=true;return agg::path_cmd_stop;
        }

       if(nCorners>0 && arcInc==cornerArc && increment>=nArcPoints[arcInc] && increment<nArcPoints[arcInc]+nCorners)
       {
           *x = corner[(increment-nArcPoints[arcInc])*2];
           *y = corner[((increment-nArcPoints[arcInc])*2)+1];
           ++increment;
           //cout<<increment<<" "<<*x<<", "<<*y<<"\n";
           return agg::path_cmd_line_to;
       }

        if ( arcInc+1 == nArcs && increment >= nArcPoints[arcInc]+1+nCorners)
            return agg::path_cmd_stop;
        else if(theta[0]+2*PI==theta[1] && increment >= nArcPoints[arcInc])
            return agg::path_cmd_stop;
        else if(arcInc+1 == nArcs && increment >= nArcPoints[arcInc])
        {
        *x = center.x + radius * cos( theta[0]);
        *y = center.y + radius * sin( theta[0]);
        ++increment;
        return agg::path_cmd_line_to;
       // agg::path_cmd
        }

        if ( arcInc+1<nArcs && increment >= nArcPoints[arcInc] )
        {
            increment=0;
            ++arcInc;
        }

        n_vertexes++;

        *x = center.x + radius * cos( theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1)) );
        *y = center.y + radius * sin( theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1)) );

        double t = theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1));

        if(increment ==0 && arcInc == 0)
        {
            ++increment;
            //cout<<increment<<" ("<<*x<<", "<<*y<<")\n";
            return agg::path_cmd_move_to;
        }
        //if ( increment == 0 )
        //{
          //  cout<<increment<<" "<<arcInc<<" "<<"("<<theta[arcInc*2]<<" "<<theta[arcInc*2+1]<<" "<<nArcPoints[arcInc]<<")"<<" move to "<<t<<"\n";
          //  ++increment;
          //  return agg::path_cmd_line_to;

        //}
       // cout<<increment<<" "<<*x<<", "<<*y<<"\n";

       // cout<<increment<<" "<<arcInc<<"/"<<nArcs<<"->"<<nArcPoints[arcInc]<<" "<<" line to "<<t<<"\n";
        ++increment;

        return agg::path_cmd_line_to;
    }
};
*/

class ArcPoly_solid {
public:

    agg::point_d center;
    unsigned increment;
    unsigned arcInc;
    unsigned int nPoints;
    double theta[8];
    unsigned int nArcPoints[4];
    int nArcs;
    double radius;

    int cornerArc;
    int nCorners;
    double corner[8];

    ArcPoly_solid():nArcs(0),nPoints(5),nCorners(0){}
    ArcPoly_solid(agg::point_d cent,double r,int n,double t0,double t1)
        : increment(0),
        center(cent),
        radius(r),
        nPoints(n),
        nArcs(1),
        nCorners(0)
    {
        theta[0]=t0;
        theta[1]=t1;
    }
    ~ArcPoly_solid()
    {
    }
    void SwapCorners(int a,int b)
    {
            double temp = corner[a*2];
            corner[a*2]=corner[b*2];
            corner[b*2]=temp;
            temp = corner[a*2+1];
            corner[a*2+1]=corner[b*2+1];
            corner[b*2+1]=temp;
    }
    ArcPoly_solid& setNPoints(int n)
    {
        nPoints=n;
        return *this;
    }
    ArcPoly_solid& setPosition(double x,double y)
    {
        center.x=x;
        center.y=y;
        return *this;
    }
    ArcPoly_solid& setRadius(double r)
    {
        radius=r;
        return *this;
    }
    ArcPoly_solid& addArc(double t0,double t1)
    {
    if(nArcs<4)
        {
        theta[nArcs*2]=t0;
        theta[nArcs*2+1]=t1;
        nArcPoints[nArcs]=std::max(4,2+(int)(nPoints*(t1-t0)/(PI*2)));
        //cout<<nArcPoints[nArcs]<<"\n";
        nArcs++;
        }
    return *this;
    }
    void AddPoint(double x,double y)
    {
        if(x==corner[(nCorners-1)*2] && y==corner[(nCorners-1)*2+1])
            return;
        corner[nCorners*2]=x;
        corner[nCorners*2+1]=y;
        nCorners++;
    }
    void InsertPoint(double x,double y)
    {
        for(int i=nCorners;i>0;i--)
        {
            corner[i*2]=corner[(i-1)*2];
            corner[i*2+1]=corner[(i-1)*2+1];
        }
        corner[0]=x;
        corner[1]=y;
        nCorners++;
    }
    void rewind(unsigned) {
        increment = 0;
        arcInc=0;
    }
    unsigned vertex(double* x, double* y);

};


class ArcPoly {
public:

    agg::point_d center;
    unsigned increment;
    unsigned arcInc;
    unsigned int nPoints;
    double theta[8];
    unsigned int nArcPoints[4];
    int nArcs;
    double radius;

    ArcPoly():nArcs(0),nPoints(5){}
    ArcPoly(agg::point_d cent,double r,int n,double t0,double t1)
        : increment(0),
        center(cent),
        radius(r),
        nPoints(n),
        nArcs(1)
    {
        theta[0]=t0;
        theta[1]=t1;
    }
    ~ArcPoly()
    {
    }
    ArcPoly& setNPoints(int n)
    {
        nPoints=n;
        return *this;
    }
    ArcPoly& setPosition(double x,double y)
    {
        center.x=x;
        center.y=y;
        return *this;
    }
    ArcPoly& setRadius(double r)
    {
        radius=r;
        return *this;
    }
    ArcPoly& addArc(double t0,double t1)
    {
    if(nArcs<4)
        {
        theta[nArcs*2]=t0;
        theta[nArcs*2+1]=t1;
        nArcPoints[nArcs]=std::max(4,2+(int)(nPoints*(t1-t0)/(PI*2)));
        //cout<<nArcPoints[nArcs]<<"\n";
        nArcs++;
        }
    return *this;
    }

    void rewind(unsigned) {
        increment = 0;
        arcInc=0;
    }

    unsigned vertex(double* x, double* y);
};


#endif
