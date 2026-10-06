#ifndef _PLOT_OBJECTS_H_
#define _PLOT_OBJECTS_H_

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "PolyTypes.h"
#include "NoiseMap.h"
#include "agg_array.h"


class LinePoly;
class ArbPoly;
class ArcPoly;
class ArbPoly_solid;
class ArcPoly_solid;
class Marker_dot_poly;
class Marker_small_arrow_poly;
class Marker_small_circle_poly;

struct Scene;


enum
{
MARKER_DOT,
MARKER_SMALL_ARROW,
MARKER_SMALL_CIRCLE,
MARKER_CROSS,
MARKER_DASH,
MARKER_SQUARE,
MARKER_HIDE
};

enum
{
GRAPH_LINES,
GRAPH_MARKERS,
GRAPH_LABELS,
GRAPH_CIRCLES
};

enum {
 GRAPH_MARKERS_DIV4,
 GRAPH_MARKERS_RADIUS4,
 GRAPH_MARKERS_RADIUS1,
 GRAPH_MARKERS_WAYPOINT2,
 GRAPH_MARKERS_MARK1,
 GRAPH_MARKERS_HIGHLITE,
 GRAPH_MARKERS_GRID4,
 GRAPH_MARKERS_COMPASS,
 GRAPH_MARKERS_PROTRACTOR,
 GRAPH_MARKERS_SAMEDISTANCE,
 GRAPH_MARKERS_SD_INV,
 GRAPH_MARKERS_ARROW
};

enum {
 GRAPH_LABEL_INDEX,
 GRAPH_LABEL_LENGTH,
 GRAPH_LABEL_ALPHABET,
 GRAPH_LABEL_X4,
 GRAPH_LABEL_Y4,
 GRAPH_LABEL_COMPASS,
 GRAPH_LABEL_PROTRACTOR,
 GRAPH_LABEL_SAMEDISTANCE,
 GRAPH_LABEL_SD_INV
};

enum {
    GRAPH_CIRCLE_RADIUS,
    GRAPH_CIRCLE_COMPASS
};

void set_screen_corners_2(Scene scene,int b);

class PlotObject
{
public:
    int layer=0;
    int group=0;
    struct BoundingBox
    {
        double x0,x1,y0,y1;
    };
    BoundingBox m_BoundingBox;

    PlotObject()
    {
    }
    ~PlotObject()
    {
    }
};

class PointCloudObject
{
public:
    double* points;
    int n_points;
    PointCloudObject* next;

    PointCloudObject(int n)
    {
        n_points=n;
        points = new double [n_points*2];
    }
    ~PointCloudObject()
    {
        if (points)
            delete[] points;
    }
};

class LineObject : public PlotObject
{
public:
    double x0,y0,x1,y1;
    double m_length;
    LineObject* next;
    LineObject(double x,double y,double xx,double yy) : x0(x),y0(y),x1(xx),y1(yy),m_length(0),next(NULL)
    {
        m_BoundingBox.x0 = std::min(x0,x1);
        m_BoundingBox.x1 = std::max(x0,x1);
        m_BoundingBox.y0 = std::min(y0,y1);
        m_BoundingBox.y1 = std::max(y0,y1);

        m_length = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
/*
        if(x0>x1 || (x1==x0 && y1<y0))
        {
         double temp=x0;
         x0=x1;
         x1=temp;
         temp=y0;
         y0=y1;
         y1=temp;
        }
        */

    }
    void SetNewEndpoint(double xx, double yy)
    {
        x1=xx;
        y1=yy;

        m_BoundingBox.x0 = std::min(x0,x1);
        m_BoundingBox.x1 = std::max(x0,x1);
        m_BoundingBox.y0 = std::min(y0,y1);
        m_BoundingBox.y1 = std::max(y0,y1);

        m_length = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
    }

    void SetPosition(double x, double y, double xx, double yy)
    {
        x0=x;
        y0=y;
        x1=xx;
        y1=yy;

        m_BoundingBox.x0 = std::min(x0,x1);
        m_BoundingBox.x1 = std::max(x0,x1);
        m_BoundingBox.y0 = std::min(y0,y1);
        m_BoundingBox.y1 = std::max(y0,y1);

        m_length = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
    }

    ~LineObject()
    {
     if(next != NULL) delete next;
    }
    bool touchLine(double x,double y, double t)
    {
     double a = atan2(y-y0,x-x0) - angle();
     double d = sqrt((x-x0)*(x-x0)+(y-y0)*((y-y0)));
     return abs(d*sin(a))<t;
    }

    double length(){return m_length;}

    double angle()
    {
    if(x1-x0 != 0)
        return atan2(y1-y0,x1-x0);
    else return y0 < y1 ? PI/2 : -PI/2;
    }

    double iangle()
    {
    if(x1-x0 != 0)
        return atan2(y0-y1,x0-x1);
    else return y0 < y1 ? -PI/2 : PI/2;
    }

    bool getPoly(LinePoly& poly,Scene scene);

};


class quad_bezier
{
public:
     double ax;
     double ay;
     double bx;
     double by;
     double cx;
     double cy;
};

class BezObject : public PlotObject
{
public:
    //double x0;
    //double y0;
    double ax;
    double ay;
    double bx;
    double by;
    double cx;
    double cy;
    double dx;
    double dy;
    double len;

    BezObject* next;

    quad_bezier getDerivative()
    {
        quad_bezier bez;
        bez.ax = (bx-ax)*3;
        bez.bx = (cx-bx)*3;
        bez.cx = (dx-cx)*3;
        bez.ay = (by-ay)*3;
        bez.by = (cy-by)*3;
        bez.cy = (dy-cy)*3;
        return bez;
    }

    double angle()
    {
   // if(x1-x0 != 0)
  //      return atan2(y1-y0,x1-x0);
  //  else return y0 < y1 ? PI/2 : -PI/2;

     if(dx-cx != 0)
        return atan2(dy-cy,dx-cx);
    else return cy < dy ? PI/2 : -PI/2;
    }

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

    void calc_Length()
    {

        double r=0.0;
        len=0;

        for(int i=1;i<=25;i++)
        {
            double t=(double)i/25;
            len+=sqrt((test_x(t)-test_x(r))*(test_x(t)-test_x(r))+((test_y(t)-test_y(r))*(test_y(t)-test_y(r))));
            r=t;
        }
    }

    void calc_Bounding_Box()
    {
         quad_bezier qb = getDerivative();

        double c = qb.ax;
        double b = 2*qb.bx - 2*qb.ax;
        double a = qb.ax+qb.cx-2*qb.bx;
        double x0,y0,x1,y1;
        bool bNoRoots=false;

        if((b*b-a*c*4)<0 || a==0 )
        {
           // cout<<"no x roots\n";
            bNoRoots=true;
        }

        double root1 = (-b + sqrt(b*b-a*c*4))/(a*2);
        double root2 = (-b - sqrt(b*b-a*c*4))/(a*2);
       // cout<<ii<<"\n";
        //cout<<"X Roots: "<<root1<<"  "<<root2<<"\n";

        if(root2 < root1)
        {
           // cout<<"flip\n";
            double q = root1;
            root1=root2;
            root2=q;
        }

        if(bNoRoots)
        {
            x0=test_x(0.0);
            x1=test_x(1.0);
        }
        else
        {
            x0=std::min(test_x(0.0),test_x(1.0));
            x1=std::max(test_x(0.0),test_x(1.0));

            if(root1>0 && root1<1.0)
            {
                    x0= std::min(x0,test_x(root1));
                    x1= std::max(x1,test_x(root1));
            }
            if(root2>0 && root2<1.0)
            {
                    x0= std::min(x0,test_x(root2));
                    x1= std::max(x1,test_x(root2));
            }
           // cout<<max(root1,0.0)<<" to "<<min(root2,1.0)<<"\n";
           // x0=test_x(max(root1,0.0));
           // x1=test_x(min(root2,1.0));
           // x0=min(test_x(0),test_x(root1));
           // x1=min(test_x(1.0),test_x(root2));
            //x1=test_x(0.74);
        }

        bNoRoots=false;
        c = qb.ay;
        b = 2*qb.by - 2*qb.ay;
        a = qb.ay+qb.cy-2*qb.by;


        root1 = (-b + sqrt(b*b-a*c*4))/(a*2);
        root2 = (-b - sqrt(b*b-a*c*4))/(a*2);

        if(root2 < root1)
        {
            double q = root1;
            root1=root2;
            root2=q;
        }

        if((b*b-a*c*4)<0 || a==0 )
        {
            //cout<<"no y roots\n";
            bNoRoots=true;
        }
        //cout<<"Y Roots: "<<root1<<"  "<<root2<<"\n";

        if(bNoRoots)
        {
            y0=test_y(0.0);
            y1=test_y(1.0);
        }
        else
        {
            y0=std::min(test_y(0.0),test_y(1.0));
            y1=std::max(test_y(0.0),test_y(1.0));

            if(root1>0 && root1<1.0)
            {
                    y0= std::min(y0,test_y(root1));
                    y1= std::max(y1,test_y(root1));
            }
            if(root2>0 && root2<1.0)
            {
                    y0= std::min(y0,test_y(root2));
                    y1= std::max(y1,test_y(root2));
            }
        }
        //std::cout<<"bb: "<<x0<<","<<y0<<"  "<<x1<<","<<y1<<"\n";
        //drawbox ret (x0,x1,y0,y1);
        m_BoundingBox.x0=std::min(x0,x1);
        m_BoundingBox.y0=std::min(y0,y1);
        m_BoundingBox.x1=std::max(x0,x1);
        m_BoundingBox.y1=std::max(y0,y1);
        //return ret;

      //  std::cout<<"Bounding Box:\n";
      //  std::cout<<m_BoundingBox.x0<<","<<m_BoundingBox.y0<<"    "<<m_BoundingBox.x1<<","<<m_BoundingBox.y1<<"\n";

    }
    BezObject(cubic_bezier bez): ax(bez.ax),bx(bez.bx),cx(bez.cx),dx(bez.dx),
                                    ay(bez.ay),by(bez.by),cy(bez.cy),dy(bez.dy),next(NULL)
    {
        calc_Bounding_Box();
        calc_Length();
    }
    void init(cubic_bezier bez)
    {
        ax=bez.ax;
        bx=bez.bx;
        cx=bez.cx;
        dx=bez.dx;
        ay=bez.ay;
        by=bez.by;
        cy=bez.cy;
        dy=bez.dy;
        next=NULL;

        calc_Bounding_Box();
        calc_Length();
      //  std::cout<<"l="<<len<<"\n";
    }

    BezObject() : next(NULL)
    {
    }

     BezObject(double x1,double x2,double x3,double x4,
              double y1,double y2,double y3,double y4) : next(NULL),ax(x1),bx(x2),cx(x3),dx(x4),
                                                                    ay(y1),by(y2),cy(y3),dy(y4)
    {
        calc_Bounding_Box();
        calc_Length();
    }



    bool getPoly(BezierPoly& poly,Scene scene);
    bool getSpecialPoly(BezierPoly& poly,Scene scene);
};

class CurveObject: public PlotObject
{
public:
BezObject* m_bez;
CurveObject* next;
int n_bez;
int terr_group;
int perimeter;
int bb_area;

CurveObject(): m_bez(NULL),n_bez(0),next(NULL),terr_group(0)
    {
    }
void calcBoundingBox()
    {
        m_BoundingBox.x0=m_bez[0].m_BoundingBox.x0;
        m_BoundingBox.x1=m_bez[0].m_BoundingBox.x1;
        m_BoundingBox.y0=m_bez[0].m_BoundingBox.y0;
        m_BoundingBox.y1=m_bez[0].m_BoundingBox.y1;
    for(int i=0;i<n_bez;i++)
        {
       // std::cout<<"bb: x:"<<m_bez[i].m_BoundingBox.x0<<","<<m_bez[i].m_BoundingBox.x1<<", y:"<<
      //      m_bez[i].m_BoundingBox.y0<<","<<m_bez[i].m_BoundingBox.y1<<"\n";
         if(m_bez[i].m_BoundingBox.x0<m_BoundingBox.x0) m_BoundingBox.x0=m_bez[i].m_BoundingBox.x0;
         if(m_bez[i].m_BoundingBox.x1>m_BoundingBox.x1) m_BoundingBox.x1=m_bez[i].m_BoundingBox.x1;
         if(m_bez[i].m_BoundingBox.y0<m_BoundingBox.y0) m_BoundingBox.y0=m_bez[i].m_BoundingBox.y0;
         if(m_bez[i].m_BoundingBox.y1>m_BoundingBox.y1) m_BoundingBox.y1=m_bez[i].m_BoundingBox.y1;
        }
   // std::cout<<"\nbb: x:"<<m_BoundingBox.x0<<","<<m_BoundingBox.x1<<", y:"<<
    //        m_BoundingBox.y0<<","<<m_BoundingBox.y1<<"\n";
    }

    void init()
    {
    calcBoundingBox();
    perimeter=0;
    bb_area=0;
    for(int i=0;i<n_bez;i++)
        {
         perimeter+=m_bez[i].len;
         bb_area+=(m_bez[i].m_BoundingBox.x1-m_bez[i].m_BoundingBox.x0)*(m_bez[i].m_BoundingBox.y1-m_bez[i].m_BoundingBox.y0);
        }
    std::cout<<" n="<<n_bez<<"\n";
    std::cout<<" l="<<perimeter<<"\n";
    std::cout<<" a="<<bb_area<<"\n";
    }

    int getCubicRoots(double* ret, double pa, double pb, double pc, double pd);
    double cuberoot(double v);



    bool getPoly(CurvePoly& poly,Scene scene);
    //bool getSpecialPoly(CurvePoly& poly,Scene scene);
    bool getPoly(CurvePoly_solid& poly,Scene scene);
};

class ArbObject : public PlotObject
{
public:
    double *point;
    unsigned nPoints;
    ArbObject* next;

    ArbObject(int size=10) : nPoints(0),next(NULL)
    {
        point = new double[size*2];
    }
    ~ArbObject()
    {
     if(point) delete[] point;
     if(next != NULL) delete next;
    }

    const int dog[18] ={  64,64,
                        512+64,64,
                        512+64,512+64,
                        64,512+64,
                        64,64,
                        512+64,64,
                        512+64,512+64,
                        64,512+64,
                        64,64
                        };

    void AddPoint(double x, double y);
    bool getPoly(ArbPoly& poly,Scene scene);
    bool getPoly(ArbPoly_solid& poly,Scene scene);
    void addCorners(ArbPoly_solid& poly,int a,int b);
};


class CircleObject : public PlotObject
{
    public:
    double x0,y0,radius;
    bool bInvert=false;
    CircleObject* next;
    //ArcPoly m_poly;

    CircleObject(double x,double y,double r) : x0(x),y0(y),radius(r),next(NULL)
    {
        m_BoundingBox.x0=x0-radius;
        m_BoundingBox.x1=x0+radius;
        m_BoundingBox.y0=y0-radius;
        m_BoundingBox.y1=y0+radius;
    }
    //void ScreenTransform(Scene scene);

     ~CircleObject()
    {
     if(next != NULL) delete next;
    }
    bool withinRadius(double x,double y)
    {
     return (sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))<radius);
    }
    bool touchRadius(double x,double y,double t)
    {
     return (sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))<radius+t &&
             sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))>radius-t);
    }
    setRadius(double r)
    {
        radius=r;
        m_BoundingBox.x0=x0-radius;
        m_BoundingBox.x1=x0+radius;
        m_BoundingBox.y0=y0-radius;
        m_BoundingBox.y1=y0+radius;
    }
    setPosition(double x,double y)
    {
        x0=x;
        y0=y;
        m_BoundingBox.x0=x0-radius;
        m_BoundingBox.x1=x0+radius;
        m_BoundingBox.y0=y0-radius;
        m_BoundingBox.y1=y0+radius;
    }

    bool getPoly(ArcPoly_solid& poly,Scene scene);
    bool getPoly(ArcPoly& poly,Scene scene);
    bool getPoly_Screen(ArcPoly& poly,Scene scene);
};

class bMarkerObject
{
public:
    int type;
    int layer=0;
    double x0;
    double y0;
    double x_offset=0.0;
    double y_offset=0.0;
    double rot;
    double len=0;

    void setLength(double nlen)
    {
     len=nlen;
    }
    void setPosition(double x,double y)
    {
        x0=x;
        y0=y;
    }
    void setRotation(double r)
    {
       rot=r;
    }
    bool clip(Scene scene)
    {
        if(x0 < scene.x0 ||
            x0 > scene.x1 ||
            y0 < scene.y0 ||
            y0 > scene.y1 )
        {
            return false;
        }
        return true;
    }
};

class MarkerObject : public PlotObject
{
    public:
    int type;
    double x0;
    double y0;
    double x_offset=0.0;
    double y_offset=0.0;
    double rot;
    double length=0;
    MarkerObject* next=NULL;

    MarkerObject(double x,double y, int t, double dir = 0.0) : x0(x),y0(y),type(t),rot(dir)
    {

    }

     ~MarkerObject()
    {
     if(next != NULL) delete next;
    }
    void resetBoundingBox()
    {
        switch(type)
        {
        case MARKER_SQUARE:
        case MARKER_DOT:
        case MARKER_DASH:
            m_BoundingBox.x0=x0-2;
            m_BoundingBox.x1=x0+2;
            m_BoundingBox.y0=y0-2;
            m_BoundingBox.y1=y0+2;
            break;
        case MARKER_SMALL_CIRCLE:
            m_BoundingBox.x0=x0-Marker_small_circle_poly::radius;
            m_BoundingBox.x1=x0+Marker_small_circle_poly::radius;
            m_BoundingBox.y0=y0-Marker_small_circle_poly::radius;
            m_BoundingBox.y1=y0+Marker_small_circle_poly::radius;
            break;
        case MARKER_SMALL_ARROW:
            m_BoundingBox.x0=x0-Marker_small_arrow_poly::length;
            m_BoundingBox.x1=x0+Marker_small_arrow_poly::length;
            m_BoundingBox.y0=y0-Marker_small_arrow_poly::length;
            m_BoundingBox.y1=y0+Marker_small_arrow_poly::length;
            break;
        }
    }
    void setLength(double len)
    {
     length=len;
    }
    void setPosition(double x,double y)
    {
        x0=x;
        y0=y;
        resetBoundingBox();
    }
    void setRotation(double r)
    {
       rot=r;
    }
    bool clip(Scene scene)
    {
        if(x0 < scene.x0 ||
            x0 > scene.x1 ||
            y0 < scene.y0 ||
            y0 > scene.y1 )
        {
            return false;
        }
        return true;
    }
};

class bLabelObject
{
public:

    int type;
    double x0;
    double y0;
    double x_offset=0;
    double y_offset=0;
    char text[12]="";

    void setPosition(double x,double y)
    {
        x0=x;
        y0=y;
    }

    void setText(std::string str)
    {
        strcpy(text,str.c_str());
    }
};

class LabelObject : public PlotObject
{
    public:
    int type;
    double x0;
    double y0;
    double x_offset=0;
    double y_offset=0;
    char text[12]="";
    LabelObject* next=NULL;

    LabelObject(double x,double y) : x0(x),y0(y)
    {
    }

    void setPosition(double x,double y)
    {
        x0=x;
        y0=y;
    }

    void setText(std::string str)
    {
        strcpy(text,str.c_str());
    }
};


class AngleObject : public PlotObject
{
    public:
    double x0,y0,radius;
    bool bInvert=false;
    AngleObject* next;
    //ArcPoly m_poly;

    double theta0=0;
    double theta1=PI*2;

    AngleObject(double x,double y,double r,double t0,double t1) : x0(x),y0(y),radius(r),theta0(t0),theta1(t1),next(NULL)
    {
        RecalcBoundingBox();
    }
    void SetPosition(double x,double y)
    {
         x0=x;
         y0=y;
         RecalcBoundingBox();
    }
    void RecalcBoundingBox()
    {
        if(( theta0 < PI && theta1 > PI) || (theta0 < -PI && theta1 > -PI) || (theta0 < PI*3 && theta1 > PI*3))
            m_BoundingBox.x0=x0-radius;
        else
            m_BoundingBox.x0=std::min(x0,std::min(x0+cos(theta0)*radius,x0+cos(theta1)*radius));

        if(( theta0 < 0 && theta1 > 0) || (theta0 < PI*2 && theta1 > PI*2))
            m_BoundingBox.x1=x0+radius;
        else
            m_BoundingBox.x1=std::max(x0,std::max(x0+cos(theta0)*radius,x0+cos(theta1)*radius));

        if(( theta0 < -PI/2 && theta1 > -PI/2) || ( theta0 < PI*3/2 && theta1 > PI*3/2))
            m_BoundingBox.y0=y0-radius;
        else
            m_BoundingBox.y0=std::min(y0,std::min(y0+sin(theta0)*radius,y0+sin(theta1)*radius));

        if(( theta0 < PI/2 && theta1 > PI/2) || ( theta0 < PI*5/2 && theta1 > PI*5/2))
            m_BoundingBox.y1=y0+radius;
        else
            m_BoundingBox.y1=std::max(y0,std::max(y0+sin(theta0)*radius,y0+sin(theta1)*radius));
    }
    void SetNewAngle(double r,double t0,double t1)
    {
        radius=r;
        theta0=t0;
        theta1=t1;
        RecalcBoundingBox();
    }

     ~AngleObject()
    {
        if(next != NULL) delete next;
    }

   // bool AngleObject::getPoly(ArcPoly& poly,Scene scene);
    bool getPoly_solid(ArcPoly_solid& poly,Scene scene);
    void addCorners(ArcPoly_solid& poly,int a,int b,bool insert=false);

    const int dog[18] ={  64,64,
                        512+64,64,
                        512+64,512+64,
                        64,512+64,
                        64,64,
                        512+64,64,
                        512+64,512+64,
                        64,512+64,
                        64,64
                        };

};


class Path_Lines;
class Path_Draw;

class PathMetaStyle
{
    public:
    int line_style=0;
    int marker_style=0;
    int color_style=0;
    int index_style=0;
    int scale_style=0;
    int scale_interval=5;

};

class PathObject
{
    public:
    PathObject* next=NULL;


    double m_points[50*2];
    double m_stats[50*2];
    int n_points=0;
    Path_Draw* draw=NULL;
    Scene m_scene;
    PathMetaStyle m_style;

    void addPoint(double x,double y,int ii=-1);
    void removePoint(int ii);
    void popPoint();
    void changed();
    Path_Draw* addGraph(int t, int st, int n_ticks,char flags=0);
    void removeGraph(int t, int subtype=-1);
    void removeGraph(Path_Draw*);
    Path_Draw* findGraph(int t, int subtype=-1);

    PathObject();


    ~PathObject()
    {
     if (draw) delete draw;
    }
};

#define GRAPH_FLAGS_SCREENCOORDS 1
#define GRAPH_FLAGS_SINGLE 2
#define GRAPH_FLAGS_EXISTS 4
#define GRAPH_FLAGS_INTERVAL 8
#define GRAPH_FLAGS_INVERSE 16
class Path_Draw
{

   // protected:
        public:
        PathObject* data=NULL;

    public:
        char flags = 0;
        int graph_type=0;
        int type=0;
        int iPoint=0;

        Path_Draw* next=NULL;
        virtual void popPoint()=0;
        virtual void addPoint(double x,double y) =0;
//        virtual void vacate()=0;
        virtual void init()=0;
         Path_Draw(PathObject* d,int t);
        virtual void changed()=0;
        ~Path_Draw()
        {
         if(next) delete next;
        }
};

class Path_Labels: public Path_Draw
{
    double len=0;

public:
    agg::pod_array<bLabelObject> bLabels;
    LabelObject* labels=NULL;

    int n_ticks=1;
    int index;
    int interval=0;

    Path_Labels(PathObject*);

    void addPoint(double x,double y);
    void init();
    void popPoint();
    void changed();

   ~Path_Labels() {
       if(labels) delete labels;
   }

};


class Path_Circle: public Path_Draw
{
public:
    CircleObject* circles=NULL;
    //int type=0;
    int n_ticks=2;

    Path_Circle(PathObject*);

    void addPoint(double x,double y);
    void init();
    void popPoint();
    void changed();

    ~Path_Circle() {
       if(circles) delete circles;
   }

};


class Path_Markers: public Path_Draw
{
    double total_len=0;
public:
   // MarkerObject* markers=NULL;
    agg::pod_array<bMarkerObject> bMarkers;
   // int type=0;
    int n_ticks=2;
    int index=0;
    int interval=0;

    Path_Markers(PathObject*);

    void addPoint(double x,double y);
    void init();
    void popPoint();
    void changed();

    ~Path_Markers() {}

};

class Path_Lines: public Path_Draw
{
public:
    LineObject* lines=NULL;

    Path_Lines(PathObject*);

    void addPoint(double x,double y);
    void init();
    void popPoint();
    void changed();

    ~Path_Lines() {
       if(lines) delete lines;
   }
};

class ArrowObject : public PlotObject
{
    public:

    double x0;
    double y0;
    double x1;
    double y1;

    const double points_tail[4] = {0,-3.5,0,3.5};
    const double points_head[10] = {   0,3,
                                    0,8,
                                    15,0,
                                    0,-8,
                                    0,-3};
    double points[16];  //one extra point = starting point
    void setLengthRotationPosition(double s, double len, double rot,double x,double y);



    ArrowObject* next;
    ArrowObject():next(NULL){}
    ~ArrowObject(){if(!next) delete next;}
    void setPosition(double x,double y,double xx,double yy)
    {
        x0=x;
        y0=y;
        x1=xx;
        y1=yy;
    }
    void setOrigin(double x,double y)
    {
        x0=x;
        y0=y;
    }
    void setEndpoint(double xx,double yy)
    {
        x1=xx;
        y1=yy;
    }
    bool getPoly(ArbPoly_solid& poly,Scene scene);

};

#endif // _PLOT_OBJECTS_H_
