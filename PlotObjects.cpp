
#include "PlotObjects.h"
#include "PolyTypes.h"
#include "Poly34.h"

//extern int screen_corners[18];
using namespace std;

extern int screen_corners[18] ={  64,64,
                        64,512+64,
                        512+64,512+64,
                        64+512,64,
                        64,64,
                        64,512+64,
                        512+64,512+64,
                        64+512,64,
                        64,64
                        };


inline double SceneX(double x, Scene scene)
{
return CLIP_BORDER+(x-scene.x0)/(scene.x1-scene.x0)*scene.width;
}

inline double SceneY(double y, Scene scene)
{
return CLIP_BORDER+(y-scene.y0)/(scene.y1-scene.y0)*scene.height;
}

inline double SceneX2(double x, Scene scene)
{
return (x-scene.x0)/(scene.x1-scene.x0)*scene.width;
}

inline double SceneY2(double y, Scene scene)
{
return (y-scene.y0)/(scene.y1-scene.y0)*scene.height;
}

bool ClipLine(double& x0, double& y0, double& x1, double& y1, Scene scene)
    {
        bool flipx = x0<x1;

        double xx0 = flipx ? x0 : x1;
        double yy0 = flipx ? y0 : y1;
        double xx1 = flipx ? x1 : x0;
        double yy1 = flipx ? y1 : y0;

        if(xx1 < scene.x0 ||
           xx0 > scene.x1 ||
           std::max(yy1,yy0) < scene.y0 ||
           std::min(yy1,yy0) > scene.y1 )
           {
              // std::cout<<"bb fail ";
            return false;
           }

         if(xx0>scene.x0 && xx0<scene.x1 && yy0>scene.y0 && yy0<scene.y1 &&
           xx1>scene.x0 && xx1<scene.x1 && yy1>scene.y0 && yy1<scene.y1)
        {
            x0=x0;
            y0=y0;
            x1=x1;
            y1=y1;
            return true;
        }

        if(x1-x0 == 0)
        {
            x0=x0;
            y0=std::max(y0,scene.y0);
            x1=x0;
            y1=std::min(y1,scene.y1);
            return true;
        }

        double slope = (yy1-yy0)/(xx1-xx0);

        double ix;
        double iy;
        double ix2;
        double iy2;
        double x;
        double y;

        iy= slope*(scene.x0-xx0)+yy0;
        ix= xx0+(scene.y0-yy0)/slope;
        ix2= xx0+(scene.y1-yy0)/slope;
        iy2= slope*(scene.x1-xx0)+yy0;

        if(xx0>=scene.x0 && xx0<=scene.x1 && yy0>=scene.y0 && yy0<=scene.y1 )
        {
            //poly.set0(x0,y0);
            x=xx0;
            y=yy0;
        }
        else if(yy0<scene.y0 && ix > scene.x0 && ix <scene.x1)
        {
            //poly.set0(ix,scene.y0);
            x=ix;
            y=scene.y0;
        }
        else if(yy0>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
        {
            //poly.set0(ix2,scene.y1);
            x=ix2;
            y=scene.y1;
        }
        else if(xx0<scene.x0 && iy >= scene.y0 && iy <= scene.y1)
        {
            //poly.set0(scene.x0,iy);
            x=scene.x0;
            y=iy;
        }
        else {return false;}

        //poly.set0(SceneX(x,scene),SceneY(y,scene));
        if(!flipx)
        {
            x1=x;
            y1=y;
        }
        else
        {
            x0=x;
            y0=y;
        }

        if(xx1>=scene.x0 && xx1<=scene.x1 && yy1>=scene.y0 && yy1<=scene.y1 )
        {
            //poly.set1(x1,y1);
            x=xx1;
            y=yy1;
        }
        else if(yy1<scene.y0 && ix > scene.x0 && ix <scene.x1)
        {
            //poly.set1(ix,scene.y0);
            x=ix;
            y=scene.y0;
        }
        else if(yy1>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
        {
            //poly.set1(ix,scene.y0);
            x=ix2;
            y=scene.y1;
        }
        else if(xx1>scene.x1 && iy2 >= scene.y0 && iy2 <= scene.y1)
        {
            //poly.set1(scene.x1,iy2);
            x=scene.x1;
            y=iy2;
        }
        else {return false;}

        //poly.set1(SceneX(x,scene),SceneY(y,scene));
        if(!flipx)
        {
            x0=x;
            y0=y;
        }
        else
        {
            x1=x;
            y1=y;
        }
    }

    // A real-cuberoots-only function:
double CurveObject::cuberoot(double v) {
  if(v<0) return -pow(-v,1/3);
  return pow(v,1/3);
}
/*
// Now then: given cubic coordinates {pa, pb, pc, pd} find all roots.
int CurveObject::getCubicRoots(double* ret, double pa, double pb, double pc, double pd) {
  double   a = (3*pa - 6*pb + 3*pc),
        b = (-3*pa + 3*pb),
        c = pa,
        d = (-pa + 3*pb - 3*pc + pd);

  // do a check to see whether we even need cubic solving:
  if (d<0.0001) {
    // this is not a cubic curve.
    if (a<0.0001) {
      // in fact, this is not a quadratic curve either.
      if (b<0.0001) {
        // in fact in fact, there are no solutions.
        return 0;
      }
      // linear solution
     // return [-c / b].filter(accept);
     ret[0] = -c / b;
     return 1;
    }
    // quadratic solution
    double q = sqrt(b*b - 4*a*c), a2 = 2*a;
   // return [(q-b)/2a, (-b-q)/a2].filter(accept)
   ret[0] = (q-b)/a2;
   ret[1] = (-b-q)/a2;
   return 2;
  }

  // at this point, we know we need a cubic solution.

  a /= d;
  b /= d;
  c /= d;

  double p = (3*b - a*a)/3,
      p3 = p/3,
      q = (2*a*a*a - 9*a*b + 27*c)/27,
      q2 = q/2,
      discriminant = q2*q2 + p3*p3*p3;

  // and some variables we're going to use later on:
  double u1, v1, root1, root2, root3;

  // three possible real roots:
  if (discriminant < 0) {
    double mp3  = -p/3,
    mp33 = mp3*mp3*mp3,
    r    = sqrt( mp33 ),
    t    = -q / (2*r),
    cosphi = t<-1 ? -1 : t>1 ? 1 : t,
    phi  = acos(cosphi),
    crtr = cuberoot(r),
    t1   = 2*crtr;
    root1 = t1 * cos(phi/3) - a/3;
    root2 = t1 * cos((phi+2*PI)/3) - a/3;
    root3 = t1 * cos((phi+4*PI)/3) - a/3;
    //return [root1, root2, root3].filter(accept);
    ret[0] = root1;
    ret[1] = root2;
    ret[2] = root3;
    return 3;
  }

  // three real roots, but two of them are equal:
  if(discriminant == 0) {
    u1 = q2 < 0 ? cuberoot(-q2) : -cuberoot(q2);
    root1 = 2*u1 - a/3;
    root2 = -u1 - a/3;
   // return [root1, root2].filter(accept);
   ret[0] = root1;
   ret[1] = root2;
   return 2;
  }

  // one real root, two complex roots
  double sd = sqrt(discriminant);
  u1 = cuberoot(sd - q2);
  v1 = cuberoot(sd + q2);
  root1 = u1 - v1 - a/3;
  //return [root1].filter(accept);
  ret[0] = root1;
  return 1;
}*/


bool LineObject::getPoly(LinePoly& poly,Scene scene)
    {
        if(m_BoundingBox.x1 < scene.x0 ||
           m_BoundingBox.x0 > scene.x1 ||
           m_BoundingBox.y1 < scene.y0 ||
           m_BoundingBox.y0 > scene.y1 )
            return false;

        double xx0 = x0<x1 ? x0 : x1;
        double yy0 = x0<x1 ? y0 : y1;
        double xx1 = x0<x1 ? x1 : x0;
        double yy1 = x0<x1 ? y1 : y0;

         if(xx0>scene.x0 && xx0<scene.x1 && yy0>scene.y0 && yy0<scene.y1 &&
           xx1>scene.x0 && xx1<scene.x1 && yy1>scene.y0 && yy1<scene.y1)
        {
            poly.set0(SceneX(xx0,scene),SceneY(yy0,scene));
            poly.set1(SceneX(xx1,scene),SceneY(yy1,scene));
            return true;
        }

        if(x1-x0 == 0)
        {
            poly.set0(SceneX(x0,scene),SceneY(std::max(y0,scene.y0),scene));
            poly.set1(SceneX(x0,scene),SceneY(std::min(y1,scene.y1),scene));
            return true;
        }

        double slope = (yy1-yy0)/(xx1-xx0);

        double ix;
        double iy;
        double ix2;
        double iy2;
        double x;
        double y;

        iy= slope*(scene.x0-xx0)+yy0;
        ix= xx0+(scene.y0-yy0)/slope;
        ix2= xx0+(scene.y1-yy0)/slope;
        iy2= slope*(scene.x1-xx0)+yy0;

        if(xx0>=scene.x0 && xx0<=scene.x1 && yy0>=scene.y0 && yy0<=scene.y1 )
        {
            //poly.set0(x0,y0);
            x=xx0;
            y=yy0;
        }
        else if(yy0<scene.y0 && ix > scene.x0 && ix <scene.x1)
        {
            //poly.set0(ix,scene.y0);
            x=ix;
            y=scene.y0;
        }
        else if(yy0>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
        {
            //poly.set0(ix2,scene.y1);
            x=ix2;
            y=scene.y1;
        }
        else if(xx0<scene.x0 && iy >= scene.y0 && iy <= scene.y1)
        {
            //poly.set0(scene.x0,iy);
            x=scene.x0;
            y=iy;
        }
        else return false;

        poly.set0(SceneX(x,scene),SceneY(y,scene));

        if(xx1>=scene.x0 && xx1<=scene.x1 && yy1>=scene.y0 && yy1<=scene.y1 )
        {
            //poly.set1(x1,y1);
            x=xx1;
            y=yy1;
        }
        else if(yy1<scene.y0 && ix > scene.x0 && ix <scene.x1)
        {
            //poly.set1(ix,scene.y0);
            x=ix;
            y=scene.y0;
        }
        else if(yy1>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
        {
            //poly.set1(ix,scene.y0);
            x=ix2;
            y=scene.y1;
        }
        else if(xx1>scene.x1 && iy2 >= scene.y0 && iy2 <= scene.y1)
        {
            //poly.set1(scene.x1,iy2);
            x=scene.x1;
            y=iy2;
        }
        else return false;

        poly.set1(SceneX(x,scene),SceneY(y,scene));
        return true;
    }

void ArbObject::AddPoint(double x, double y)
    {
        if(nPoints==0)
        {
            m_BoundingBox.x0 = x;
            m_BoundingBox.x1 = x;
            m_BoundingBox.y0 = y;
            m_BoundingBox.y1 = y;
        }
        else
        {
            m_BoundingBox.x0 = std::min(m_BoundingBox.x0,x);
            m_BoundingBox.x1 = std::max(m_BoundingBox.x1,x);
            m_BoundingBox.y0 = std::min(m_BoundingBox.y0,y);
            m_BoundingBox.y1 = std::max(m_BoundingBox.y1,y);
        }
        point[nPoints*2]=x;
        point[nPoints*2+1]=y;
        nPoints++;
    }


bool ArbObject::getPoly(ArbPoly& poly,Scene scene)
    {

        poly.nPoints=0;

        if(m_BoundingBox.x1 < scene.x0 ||
           m_BoundingBox.x0 > scene.x1 ||
           m_BoundingBox.y1 < scene.y0 ||
           m_BoundingBox.y0 > scene.y1 )
           {
            return false;
           }

        point[nPoints*2]=point[0];
        point[nPoints*2+1]=point[1];

        for(int i=0;i<nPoints;i++)
        {
            //this will be easier if we do everything left to right
            bool switchlr=point[i*2]<point[(i+1)*2] || (point[i*2]==point[(i+1)*2] && point[i*2+1]<point[(i+1)*2]+1);

            double xx0 = switchlr ? point[i*2] : point[(i+1)*2];
            double yy0 = switchlr ? point[i*2+1] : point[(i+1)*2+1];
            double xx1 = switchlr ? point[(i+1)*2] : point[i*2];
            double yy1 = switchlr ? point[(i+1)*2+1] : point[i*2+1];

            if(xx0>scene.x0 && xx0<scene.x1 && yy0>scene.y0 && yy0<scene.y1 &&
               xx1>scene.x0 && xx1<scene.x1 && yy1>scene.y0 && yy1<scene.y1)
            {
                poly.AddPoint(SceneX(xx0,scene),
                              SceneY(yy0,scene));
                poly.AddPoint(SceneX(xx1,scene),
                              SceneY(yy1,scene));

                continue;
            }

            //local bounding box check
            if(  std::min(xx0,xx1)>scene.x1 ||  std::max(xx0,xx1)<scene.x0 ||
                std::min(yy0,yy1)>scene.y1 ||  std::max(yy0,yy1)<scene.y0 )
                continue;

            if(xx1-xx0 == 0 && xx0>=scene.x0 && xx0<=scene.x1)
            {
                poly.AddPoint(SceneX(xx0,scene),
                              SceneY( std::max(yy0,scene.y0),scene));
                poly.AddPoint(SceneX(xx0,scene),
                              SceneY( std::min(yy1,scene.y1),scene));
                continue;
            }
            else if(xx1-xx0 == 0)
            {
                continue;}

            double slope = (yy1-yy0)/(xx1-xx0);

            double ix;
            double iy;
            double ix2;
            double iy2;
            double x;
            double y;

            iy= slope*(scene.x0-xx0)+yy0;
            ix= xx0+(scene.y0-yy0)/slope;
            ix2= xx0+(scene.y1-yy0)/slope;
            iy2= slope*(scene.x1-xx0)+yy0;

            if(xx0>=scene.x0 && xx0<=scene.x1 && yy0>=scene.y0 && yy0<=scene.y1 )
            {
                x=xx0;
                y=yy0;
            }
            else if(yy0<scene.y0 && ix > scene.x0 && ix <scene.x1)
            {
                x=ix;
                y=scene.y0;
            }
            else if(yy0>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
            {
                x=ix2;
                y=scene.y1;
                //cout<<"("<<xx0<<","<<yy0<<" ";
            }
            else if(xx0<scene.x0 && iy >= scene.y0 && iy <= scene.y1)
            {
                x=scene.x0;
                y=iy;
            }
            else{continue; }

            poly.AddPoint(SceneX(x,scene),
                          SceneY(y,scene));

            if(xx1>=scene.x0 && xx1<=scene.x1 && yy1>=scene.y0 && yy1<=scene.y1 )
            {
                x=xx1;
                y=yy1;
            }
            else if(yy1<scene.y0 && ix > scene.x0 && ix <scene.x1)
            {
                x=ix;
                y=scene.y0;
            }
            else if(yy1>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
            {
                x=ix2;
                y=scene.y1;
                //cout<<xx1<<","<<yy1<<") ";
            }
            else if(xx1>scene.x1 && iy2 >= scene.y0 && iy2 <= scene.y1)
            {
                x=scene.x1;
                y=iy2;
            }
            else{continue; }

            poly.AddPoint(SceneX(x,scene),
                          SceneY(y,scene));
        }
        //cout<<"\n";
       // cout<<poly.nPoints<<"\n";
       return true;
    }

bool ArbObject::getPoly(ArbPoly_solid& poly,Scene scene)
    {
        poly.nPoints=0;

        if(m_BoundingBox.x1 < scene.x0 ||
           m_BoundingBox.x0 > scene.x1 ||
           m_BoundingBox.y1 < scene.y0 ||
           m_BoundingBox.y0 > scene.y1 )
           {
            return false;
           }
        bool inside[4]={0,0,0,0};

        point[nPoints*2]=point[0];
        point[nPoints*2+1]=point[1];

        //for corner drawing:
        int status=0;   //status is the last scene edge we "exited" via
        int entry=-1;   //entry stores the scene edge we first "entered" via

        for(int i=0;i<nPoints;i++)
        {
            //this will be easier if we do everything left to right
            bool switchlr=point[i*2]>point[(i+1)*2] || (point[i*2]==point[(i+1)*2] && point[i*2+1]<point[(i+1)*2]+1);

            double xx0 = !switchlr ? point[i*2] : point[(i+1)*2];
            double yy0 = !switchlr ? point[i*2+1] : point[(i+1)*2+1];
            double xx1 = !switchlr ? point[(i+1)*2] : point[i*2];
            double yy1 = !switchlr ? point[(i+1)*2+1] : point[i*2+1];
/*
            if(poly.nPoints ==0 && xx0 != xx1 && xx1!=scene.x0)
            {
                double test = (yy1-scene.y0)/(xx1-scene.x0) - (yy1-yy0)/(xx1-xx0);
                //cout<<test<<"\n";
                if(xx0<scene.x0 && scene.x0<xx1 &&  test >0)  //we are above
                    inside[0]=true;
                else if(xx0<scene.x0 && scene.x0<xx1 &&  test <0) //below
                    inside[1]=true;

                if(yy0<scene.y0 && scene.y0<yy1 &&  test >0)   //to the left
                    inside[2]=true;
                else if(yy0<scene.y0 && scene.y0<yy1 &&  test <0) //to the right
                    inside[3]=true;
            }*/

            if(xx0>scene.x0 && xx0<scene.x1 && yy0>scene.y0 && yy0<scene.y1 &&
               xx1>scene.x0 && xx1<scene.x1 && yy1>scene.y0 && yy1<scene.y1)
            {
                poly.AddPoint(SceneX(point[i*2],scene),
                              SceneY(point[i*2+1],scene));

                poly.AddPoint(SceneX(point[(i+1)*2],scene),
                              SceneY(point[(i+1)*2+1],scene));
                continue;
            }

            //local bounding box check
            if( std::min(xx0,xx1)>scene.x1 || std::max(xx0,xx1)<scene.x0 ||
               std::min(yy0,yy1)>scene.y1 || std::max(yy0,yy1)<scene.y0 )
                continue;

            if(xx1-xx0 == 0 && xx0>=scene.x0 && xx0<=scene.x1)
            {
                int v_status=0;
                if(point[i*2+1]<scene.y0) v_status=1;
                if(point[i*2+1]>scene.y1) v_status=3;

                if(entry != -1 && v_status !=0 && v_status !=status)
                {
                    addCorners(poly,status,v_status);
                    //cout<<"corner v "<<status<<" "<<v_status<<"\n";
                }
                else if(entry==-1)
                    entry = v_status;

                if(switchlr)
                {
                    poly.AddPoint(SceneX(point[i*2],scene),
                                  SceneY(std::max(point[i*2+1],scene.y0),scene));
                    poly.AddPoint(SceneX(point[i*2],scene),
                                  SceneY(std::min(point[(i+1)*2+1],scene.y1),scene));
                }
                else
                {
                    poly.AddPoint(SceneX(point[i*2],scene),
                                  SceneY(std::min(point[i*2+1],scene.y1),scene));
                    poly.AddPoint(SceneX(point[i*2],scene),
                                  SceneY(std::max(point[(i+1)*2+1],scene.y0),scene));
                }

                if(point[(i+1)*2+1]<scene.y0) status=1;
                if(point[(i+1)*2+1]>scene.y1) status=3;

                continue;
            }
            else if(xx1-xx0 == 0)
            {
                continue;}

            double slope = (yy1-yy0)/(xx1-xx0);

            double ix;
            double iy;
            double ix2;
            double iy2;
            double x;
            double y;
            double xx;
            double yy;
            int l_status=0;
            int r_status=0;

            iy= slope*(scene.x0-xx0)+yy0;
            ix= xx0+(scene.y0-yy0)/slope;
            ix2= xx0+(scene.y1-yy0)/slope;
            iy2= slope*(scene.x1-xx0)+yy0;

            if(xx0>=scene.x0 && xx0<=scene.x1 && yy0>=scene.y0 && yy0<=scene.y1 )
            {
                x=xx0;
                y=yy0;
            }
            else if(yy0<scene.y0 && ix > scene.x0 && ix <scene.x1)
            {
                x=ix;
                y=scene.y0;
                l_status=1;
            }
            else if(yy0>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
            {
                x=ix2;
                y=scene.y1;
                l_status=3;
            }
            else if(xx0<scene.x0 && iy >= scene.y0 && iy <= scene.y1)
            {
                x=scene.x0;
                y=iy;
                l_status=4;
            }
            else{continue; }

            if(xx1>=scene.x0 && xx1<=scene.x1 && yy1>=scene.y0 && yy1<=scene.y1 )
            {
                xx=xx1;
                yy=yy1;
            }
            else if(yy1<scene.y0 && ix > scene.x0 && ix <scene.x1)
            {
                xx=ix;
                yy=scene.y0;
                r_status=1;
            }
            else if(yy1>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
            {
                xx=ix2;
                yy=scene.y1;
                r_status=3;
            }
            else if(xx1>scene.x1 && iy2 >= scene.y0 && iy2 <= scene.y1)
            {
                xx=scene.x1;
                yy=iy2;
                r_status=2;
            }
            else{continue; }

            if(!switchlr)
            {
                if(entry != -1 && l_status>0)
                {
                    //cout<<i<<" entering "<<l_status<<"\n";
                    if(l_status!=status)
                    {
                        //cout<<"corner a "<<status<<" "<<l_status<<"\n";
                        addCorners(poly,status,l_status);
                    }
                    //else status=0;
                }
                else if(entry==-1)
                    entry=l_status;

                poly.AddPoint(SceneX(x,scene),
                              SceneY(y,scene));

                poly.AddPoint(SceneX(xx,scene),
                              SceneY(yy,scene));
                if(r_status>0)
                {
                    //cout<<i<<" exiting "<<r_status<<"\n";
                    status=r_status;
                }
                //status=r_status;
            }
            else
            {
                if( entry !=-1 && r_status>0)
                {
                    //cout<<i<<" entering "<<r_status<<"\n";
                     if(r_status!=status)
                    {
                        //cout<<"corner b "<<status<<" "<<r_status<<"\n";
                        addCorners(poly,status,r_status);
                    }
                    else status=0;
                }
                else if(entry==-1)
                    entry=r_status;

                poly.AddPoint(SceneX(xx,scene),
                              SceneY(yy,scene));

                poly.AddPoint(SceneX(x,scene),
                              SceneY(y,scene));

                if(l_status>0)
                {
                    //cout<<i<<" exiting "<<l_status<<"\n";
                    status=l_status;
                }
            }
        }
        if(entry==-1)
            entry=0;
        if(status !=entry && entry !=0)
        {
            //cout<<"corner f "<<status<<" "<<entry<<"\n";
           addCorners(poly,status,entry);
        }
        //cout<<"\n";
        //cout<<poly.nPoints<<"\n";
        if(poly.nPoints==0 && inside[0]==true && inside[1]==true/* && inside[2]==true && inside[3]==true*/)
        {
            //cout<<"inside\n";
            addCorners(poly,0,4);
            return true;
        }
        else if(poly.nPoints==0)
            return false;
        //cout<<"ok!\n";
        return true;
    }

void ArbObject::addCorners(ArbPoly_solid& poly,int a,int b)
    {
        int c =b;
        if(a>=b) c+=4;
        //cout<<a<<" "<<b<<"\n";
        for(int i=a;i<c;i++)
        {
            poly.AddPoint(dog[i*2],dog[i*2+1]);
        }
    }

bool CircleObject::getPoly(ArcPoly_solid& poly,Scene scene)
    {
        poly.nArcs=0;
        poly.nCorners=0;

        poly.setPosition(SceneX(x0,scene),SceneY(y0,scene)).setRadius(radius/((scene.x1-scene.x0)/scene.width)).setNPoints(14+radius/(3*(scene.x1-scene.x0)/scene.width));

        if((y0+radius < scene.y0) ||
           (y0-radius > scene.y1) ||
           (x0+radius < scene.x0) ||
           (x0-radius > scene.x1))
            {
            poly.addArc(0,PI*2);
            return bInvert;
            }

        if((y0+radius < scene.y1) &&
           (y0-radius > scene.y0) &&
           (x0+radius < scene.x1) &&
           (x0-radius > scene.x0))
            {
            poly.addArc(0,PI*2);
            return !bInvert;
            }
        double t,t2,x,x2,y,y2;
        double theta[9];    //last point = first point
        int i=0;
        if((y0+radius)>scene.y0 && (y0-radius)<scene.y0)
        {
            t= asin((scene.y0-y0)/radius);
            t2= PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(x0+x>scene.x0 && x0+x<scene.x1)
            {
                theta[i]=t;
                i++;
            }
            if(x0+x2>scene.x0 && x0+x2<scene.x1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if((y0+radius)>scene.y1 && (y0-radius)<scene.y1)
        {
            t= asin((scene.y1-y0)/radius);
            t2=PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(x0+x>scene.x0 && x0+x<scene.x1)
            {
                theta[i]=t;
                i++;
            }
            if(x0+x2>scene.x0 && x0+x2<scene.x1)
            {
                theta[i]=t2;
                i++;
            }
        }
         if((x0+radius)>scene.x0 && (x0-radius)<scene.x0)
        {
            t= acos((scene.x0-x0)/radius);
            t2=PI*2-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(y0+y>scene.y0 && y0+y<scene.y1)
            {
                theta[i]=t;
                i++;
            }
            if(y0+y2>scene.y0 && y0+y2<scene.y1)
            {
                theta[i]=t2;
                i++;
            }
        }
         if((x0+radius)>scene.x1 && (x0-radius)<scene.x1)
        {
            t= -acos((scene.x1-x0)/radius);
            t2=-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(y0+y>scene.y0 && y0+y<scene.y1)
            {
                theta[i]=t;
                i++;
            }
            if(y0+y2>scene.y0 && y0+y2<scene.y1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if(i>0)
        {
            double temp;
            int best=-1;
            int c=0;
            //order clockwise
            for(int j=0;j<i-1;j++)
            {
                best=j;
                for(int jj=j+1;jj<i;jj++)
                {
                    if(theta[jj]<theta[best])
                        best=jj;
                }
                if(best>j)
                {
                    temp=theta[j];
                    theta[j]=theta[best];
                    theta[best]=temp;
                    c++;
                }
            }

            theta[i] = PI*2+theta[0]; //set last angle = first angle

            bool skip[] = {0,0,0,0};
            int cc=0;
            //test each screen corner. Does it fall within our circle?
            if(sqrt((scene.x0-x0)*(scene.x0-x0)+(scene.y0-y0)*(scene.y0-y0))<radius)
                {
                poly.corner[cc]=CLIP_BORDER;//scene.x0;
                poly.corner[cc+1]=CLIP_BORDER;//scene.y0;
                cc++;
                } else skip[0]=1;
            if(sqrt((scene.x1-x0)*(scene.x1-x0)+(scene.y0-y0)*(scene.y0-y0))<radius)
                {
                poly.corner[cc*2]=CLIP_BORDER+512;//scene.x1;
                poly.corner[cc*2+1]=CLIP_BORDER;//scene.y0;
                cc++;

                } else skip[1]=1;
            if(sqrt((scene.x1-x0)*(scene.x1-x0)+(scene.y1-y0)*(scene.y1-y0))<radius)
                {
                poly.corner[cc*2]=CLIP_BORDER+512;//scene.x1;
                poly.corner[cc*2+1]=CLIP_BORDER+512;//scene.y1;
                cc++;
                } else skip[2]=1;
            if(sqrt((scene.x0-x0)*(scene.x0-x0)+(scene.y1-y0)*(scene.y1-y0))<radius)
                {
                poly.corner[cc*2]=CLIP_BORDER;//scene.x0;
                poly.corner[cc*2+1]=CLIP_BORDER+512;//scene.y1;
                cc++;
                } else skip[3]=1;

            //clockwise
            if(cc==2 && skip[1] && skip[2])
            {
                poly.SwapCorners(0,1);
            }
            else if(cc==3 && skip[1])
            {
                poly.SwapCorners(0,1);
                poly.SwapCorners(1,2);
            }
            else if(cc==3 && skip[2])
            {
                poly.SwapCorners(0,2);
                poly.SwapCorners(1,2);
            }
            poly.nCorners=cc;
            double thetac;
            if(cc>0)
            {   //we need to know the angle after which to go to the corners
                thetac=atan2(poly.corner[1]-poly.center.y,poly.corner[0]-poly.center.x);
                if(theta[i]-(PI*2)>thetac)
                    thetac+=PI*2;
            }

            bool ok=bInvert;;
            double theta_test = theta[0]+(theta[1]-theta[0])/2;
            if(x0+radius*cos(theta_test) > scene.x0 &&
               x0+radius*cos(theta_test) < scene.x1 &&
               y0+radius*sin(theta_test) > scene.y0 &&
               y0+radius*sin(theta_test) < scene.y1 )
                ok=!bInvert;

            for(int j=0;j<i;j++)
            {
                if(ok)
                {
                    poly.addArc(theta[j],theta[j+1]);
                }
                else if(cc>0)
                {
                    if(thetac>theta[j] && thetac <theta[j+1])
                    {
                        //put the corners after the appropriate arc segment
                        if(j==0)
                            poly.cornerArc=(i/2)-1;
                        else
                            poly.cornerArc=(j-1)/2;
                    }
                }
                ok=!ok;
            }
        return true;
        }
        return false;
    }

bool CircleObject::getPoly(ArcPoly& poly,Scene scene)
    {
        poly.nArcs=0;

        poly.setPosition(SceneX(x0,scene),SceneY(y0,scene)).setRadius(radius/((scene.x1-scene.x0)/scene.width)).setNPoints(14+radius/(3*(scene.x1-scene.x0)/scene.width));

        if((y0+radius < scene.y0) ||
           (y0-radius > scene.y1) ||
           (x0+radius < scene.x0) ||
           (x0-radius > scene.x1))
            {
            poly.addArc(0,PI*2);
            return bInvert;
            }

        if((y0+radius < scene.y1) &&
           (y0-radius > scene.y0) &&
           (x0+radius < scene.x1) &&
           (x0-radius > scene.x0))
            {
            poly.addArc(0,PI*2);
            return !bInvert;
            }
        double t,t2,x,x2,y,y2;
        double theta[9];    //8 possible points+ last point = first point
        int i=0;
        if((y0+radius)>scene.y0 && (y0-radius)<scene.y0)
        {
            t= asin((scene.y0-y0)/radius);
            t2= PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(x0+x>scene.x0 && x0+x<scene.x1)
            {
                theta[i]=t;
                i++;
            }
            if(x0+x2>scene.x0 && x0+x2<scene.x1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if((y0+radius)>scene.y1 && (y0-radius)<scene.y1)
        {
            t= asin((scene.y1-y0)/radius);
            t2=PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(x0+x>scene.x0 && x0+x<scene.x1)
            {
                theta[i]=t;
                i++;
            }
            if(x0+x2>scene.x0 && x0+x2<scene.x1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if((x0+radius)>scene.x0 && (x0-radius)<scene.x0)
        {
            t= acos((scene.x0-x0)/radius);
            t2=PI*2-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(y0+y>scene.y0 && y0+y<scene.y1)
            {
                theta[i]=t;
                i++;
            }
            if(y0+y2>scene.y0 && y0+y2<scene.y1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if((x0+radius)>scene.x1 && (x0-radius)<scene.x1)
        {
            t= -acos((scene.x1-x0)/radius);
            t2=-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(y0+y>scene.y0 && y0+y<scene.y1)
            {
                theta[i]=t;
                i++;
            }
            if(y0+y2>scene.y0 && y0+y2<scene.y1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if(i>0) //need at least one intersection point
        {
            double temp;
            int best=-1;
            int c=0;

            //order the angles clockwise
            for(int j=0;j<i-1;j++)
            {
                best=j;
                for(int jj=j+1;jj<i;jj++)
                {
                    if(theta[jj]<theta[best])
                        best=jj;
                }
                if(best>j)
                {
                    temp=theta[j];
                    theta[j]=theta[best];
                    theta[best]=temp;
                    c++;
                }
            }

            theta[i] = PI*2+theta[0]; //set last angle = first angle

            bool ok=bInvert;;
            //test our first arc segment- is it visible?
            double theta_test = theta[0]+(theta[1]-theta[0])/2;
            if(x0+radius*cos(theta_test) > scene.x0 &&
               x0+radius*cos(theta_test) < scene.x1 &&
               y0+radius*sin(theta_test) > scene.y0 &&
               y0+radius*sin(theta_test) < scene.y1 )
                ok=!bInvert;
            //draw alternating arc segments
            for(int j=0;j<i;j++)
            {
                if(ok)
                {
                    poly.addArc(theta[j],theta[j+1]);
                }
                ok=!ok;
            }
        return true;
        }
    return false;
    }


bool CircleObject::getPoly_Screen(ArcPoly& poly,Scene scene_)
    {
        poly.nArcs=0;


    static Scene scene = {CLIP_BORDER,CLIP_BORDER,scene_.width+CLIP_BORDER,scene_.height+CLIP_BORDER,scene_.width,scene_.height};

        poly.setPosition(SceneX(x0,scene_),SceneY(y0,scene_)).setRadius(radius).setNPoints(42);

        //cout<<poly.center.x<<" "<<poly.center.y<<"\n";
        //cout<<"  "<<scene.x0<<" "<<scene.x1<<" "<<scene.y0<<" "<<scene.y1<<"\n";

        if((poly.center.y+radius < scene.y0) ||
           (poly.center.y-radius > scene.y1) ||
           (poly.center.x+radius < scene.x0) ||
           (poly.center.x-radius > scene.x1))
            {
            poly.addArc(0,PI*2);
            return bInvert;
            }

        if((poly.center.y+radius < scene.y1) &&
           (poly.center.y-radius > scene.y0) &&
           (poly.center.x+radius < scene.x1) &&
           (poly.center.x-radius > scene.x0))
            {
            poly.addArc(0,PI*2);
            return !bInvert;
            }
        double t,t2,x,x2,y,y2;
        double theta[9];    //8 possible points+ last point = first point
        int i=0;
        if((poly.center.y+radius)>scene.y0 && (poly.center.y-radius)<scene.y0)
        {
            t= asin((scene.y0-poly.center.y)/radius);
            t2= PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(poly.center.x+x>scene.x0 && poly.center.x+x<scene.x1)
            {
                theta[i]=t;
                i++;
            }
            if(poly.center.x+x2>scene.x0 && poly.center.x+x2<scene.x1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if((poly.center.y+radius)>scene.y1 && (poly.center.y-radius)<scene.y1)
        {
            t= asin((scene.y1-poly.center.y)/radius);
            t2=PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(poly.center.x+x>scene.x0 && poly.center.x+x<scene.x1)
            {
                theta[i]=t;
                i++;
            }
            if(poly.center.x+x2>scene.x0 && poly.center.x+x2<scene.x1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if((poly.center.x+radius)>scene.x0 && (poly.center.x-radius)<scene.x0)
        {
            t= acos((scene.x0-poly.center.x)/radius);
            t2=PI*2-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(poly.center.y+y>scene.y0 && poly.center.y+y<scene.y1)
            {
                theta[i]=t;
                i++;
            }
            if(poly.center.y+y2>scene.y0 && poly.center.y+y2<scene.y1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if((poly.center.x+radius)>scene.x1 && (poly.center.x-radius)<scene.x1)
        {
            t= -acos((scene.x1-poly.center.x)/radius);
            t2=-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(poly.center.y+y>scene.y0 && poly.center.y+y<scene.y1)
            {
                theta[i]=t;
                i++;
            }
            if(poly.center.y+y2>scene.y0 && poly.center.y+y2<scene.y1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if(i>0) //need at least one intersection point
        {
            double temp;
            int best=-1;
            int c=0;

            //order the angles clockwise
            for(int j=0;j<i-1;j++)
            {
                best=j;
                for(int jj=j+1;jj<i;jj++)
                {
                    if(theta[jj]<theta[best])
                        best=jj;
                }
                if(best>j)
                {
                    temp=theta[j];
                    theta[j]=theta[best];
                    theta[best]=temp;
                    c++;
                }
            }

            theta[i] = PI*2+theta[0]; //set last angle = first angle

            bool ok=bInvert;;
            //test our first arc segment- is it visible?
            double theta_test = theta[0]+(theta[1]-theta[0])/2;
            if(poly.center.x+radius*cos(theta_test) > scene.x0 &&
               poly.center.x+radius*cos(theta_test) < scene.x1 &&
               poly.center.y+radius*sin(theta_test) > scene.y0 &&
               poly.center.y+radius*sin(theta_test) < scene.y1 )
                ok=!bInvert;
            //draw alternating arc segments
            for(int j=0;j<i;j++)
            {
                if(ok)
                {
                    poly.addArc(theta[j],theta[j+1]);
                }
                ok=!ok;
            }
        return true;
        }
    return false;
    }

    /*
    bool AngleObject::getPoly(ArcPoly& poly,Scene scene)
    {
        poly.nArcs=0;
        poly.setPosition(SceneX(x0,scene),CLIP_BORDER+(y0-scene.y0)/(scene.y1-scene.y0)*scene.width).setRadius(radius/((scene.x1-scene.x0)/scene.width)).setNPoints(14+radius/(3*(scene.x1-scene.x0)/scene.width));

        if((m_BoundingBox.y1 < scene.y0) ||
           (m_BoundingBox.y0  > scene.y1) ||
           (m_BoundingBox.x1  < scene.x0) ||
           (m_BoundingBox.x0  > scene.x1))
            {
            poly.addArc(theta0,theta1);
            return bInvert;
            }

        if((m_BoundingBox.y1 < scene.y1) &&
           (m_BoundingBox.y0 > scene.y0) &&
           (m_BoundingBox.x1 < scene.x1) &&
           (m_BoundingBox.x0 > scene.x0))
            {
            poly.addArc(theta0,theta1);
            return !bInvert;
            }

        double t,t2,x,x2,y,y2;
        double theta[9];    //last point = first point
        int i=0;
        if((y0+radius)>scene.y0 && (y0-radius)<scene.y0)
        {
            t= asin((scene.y0-y0)/radius);
            t2= PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(x0+x>scene.x0 && x0+x<scene.x1)
            {
                theta[i]=t;
                i++;
            }
            if(x0+x2>scene.x0 && x0+x2<scene.x1)
            {
                theta[i]=t2;
                i++;
            }
        }
        if((y0+radius)>scene.y1 && (y0-radius)<scene.y1)
        {
            t= asin((scene.y1-y0)/radius);
            t2=PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(x0+x>scene.x0 && x0+x<scene.x1)
            {
                theta[i]=t;
                i++;
            }
            if(x0+x2>scene.x0 && x0+x2<scene.x1)
            {
                theta[i]=t2;
                i++;
            }
        }
         if((x0+radius)>scene.x0 && (x0-radius)<scene.x0)
        {
            t= acos((scene.x0-x0)/radius);
            t2=PI*2-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(y0+y>scene.y0 && y0+y<scene.y1)
            {
                theta[i]=t;
                i++;
            }
            if(y0+y2>scene.y0 && y0+y2<scene.y1)
            {
                theta[i]=t2;
                i++;
            }
        }
         if((x0+radius)>scene.x1 && (x0-radius)<scene.x1)
        {
            t= -acos((scene.x1-x0)/radius);
            t2=-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(y0+y>scene.y0 && y0+y<scene.y1)
            {
                theta[i]=t;
                i++;
            }
            if(y0+y2>scene.y0 && y0+y2<scene.y1)
            {
                theta[i]=t2;
                i++;
            }
        }
        //cout<<"i: "<<i<<"\n";
        if(i>0)
        {
            double temp;
            int best=-1;
            int c=0;

             //if(theta0 < (-PI/2))
            {
                for(int j=0;j<i;j++)
                {
                    if(theta[j] > (theta0+(PI*2)) )
                        theta[j]-=(PI*2);
                    else if(theta[j] <theta0 && theta[j]<-PI/2)
                        theta[j]+=PI*2;
                }
            }

            theta[i]=theta0;
            theta[i+1]=theta1;
            i+=2;

            for(int j=0;j<i-1;j++)
            {
                best=j;
                for(int jj=j+1;jj<i;jj++)
                {
                    if(theta[jj]<theta[best])
                        best=jj;
                }
                if(best>j)
                {
                    temp=theta[j];
                    theta[j]=theta[best];
                    theta[best]=temp;
                    c++;
                }
            }

            //chop points outside our angle range
            while(theta[i-1]>theta1)
                --i;
            int chop = 0;
            while(theta[chop] < theta0)
                ++chop;
            if(chop>0)
                for(int j=0;j<i;j++)
                    theta[j]=theta[j+chop];
            i-=chop;
            if(i>2)
            {
                bool ok=bInvert;
                double theta_test = theta[0]+(theta[1]-theta[0])/2;
                if(x0+radius*cos(theta_test) > scene.x0 &&
                   x0+radius*cos(theta_test) < scene.x1 &&
                   y0+radius*sin(theta_test) > scene.y0 &&
                   y0+radius*sin(theta_test) < scene.y1 )
                    ok=!bInvert;
                //int cc=0;
                for(int j=0;j<i-1;j++)
                {
                    if(ok)
                    {
                      // cc++;
                        poly.addArc(theta[j],theta[j+1]);
                    }
                    ok=!ok;


                }
                return true;
            }
            return false;
                  //  for(int j=0;j<i;j++)
                  //      cout<<theta[j]*180/PI<<" ";
                  //  cout<<"\n";
             // if(cc==0)
              //{
              //    cout<<"i="<<i<<"\n";
             //    cout<<"Error!\n";
            //     return false;
            //  }
        return true;
        }
        return false;
    }*/
bool AngleObject::getPoly_solid(ArcPoly_solid& poly,Scene scene)
    {
        poly.nArcs=0;
        //cout<<"x::";
        poly.setPosition(SceneX(x0,scene),SceneY(y0,scene)).setRadius(radius/((scene.x1-scene.x0)/scene.width)).setNPoints(14+radius/(3*(scene.x1-scene.x0)/scene.width));

        //cout<<"bb: "<<m_BoundingBox.x0<<", "<<m_BoundingBox.y0<<"   "<<m_BoundingBox.x1<<", "<<m_BoundingBox.y1<<"\n";

        if((m_BoundingBox.y1 < scene.y0) ||
           (m_BoundingBox.y0  > scene.y1) ||
           (m_BoundingBox.x1  < scene.x0) ||
           (m_BoundingBox.x0  > scene.x1))
            {
            //poly.addArc(theta0,theta1);
            //cout<<"outside\n";
            return false;
            }

        if((m_BoundingBox.y1 < scene.y1) &&
           (m_BoundingBox.y0 > scene.y0) &&
           (m_BoundingBox.x1 < scene.x1) &&
           (m_BoundingBox.x0 > scene.x0))
            {
                poly.addArc(theta0,theta1);

                poly.nCorners=1;
                poly.corner[0]=poly.center.x;
                poly.corner[1]=poly.center.y;
                poly.cornerArc=0;
            //cout<<"inside\n";
            return !bInvert;
            }
        double t,t2,x,x2,y,y2;
        double theta[10];
        int status_points[10]={0,0,0,0,0,0,0,0,0,0}; //for drawing corners
        int i=0;
        if((y0+radius)>scene.y0 && (y0-radius)<scene.y0)
        {
          //  cout<<"y0 ";
            t= asin((scene.y0-y0)/radius);
            t2= PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(x0+x>scene.x0 && x0+x<scene.x1)
            {
                theta[i]=t;
                status_points[i]=1;
                i++;
            }
            if(x0+x2>scene.x0 && x0+x2<scene.x1)
            {
                theta[i]=t2;
                status_points[i]=1;
                i++;
            }
        }
        if((y0+radius)>scene.y1 && (y0-radius)<scene.y1)
        {
          //  cout<<"y1 ("<<y0<<" "<<scene.y1<<") ";
            t= asin((scene.y1-y0)/radius);
            t2=PI-t;
            x=cos(t)*radius;
            x2=cos(t2)*radius;
            if(x0+x>scene.x0 && x0+x<scene.x1)
            {
                theta[i]=t;
                status_points[i]=3;
                i++;
            }
            if(x0+x2>scene.x0 && x0+x2<scene.x1)
            {
                theta[i]=t2;
                status_points[i]=3;
                i++;
            }
        }
         if((x0+radius)>scene.x0 && (x0-radius)<scene.x0)
        {
           // cout<<"x0 ";
            t= acos((scene.x0-x0)/radius);
            t2=PI*2-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(y0+y>scene.y0 && y0+y<scene.y1)
            {
                theta[i]=t;
                status_points[i]=4;
                i++;
            }
            if(y0+y2>scene.y0 && y0+y2<scene.y1)
            {
                theta[i]=t2;
                status_points[i]=4;
                i++;
            }
        }
         if((x0+radius)>scene.x1 && (x0-radius)<scene.x1)
        {
           // cout<<"x1 ";
            t= -acos((scene.x1-x0)/radius);
            t2=-t;
            y=sin(t)*radius;
            y2=sin(t2)*radius;
            if(y0+y>scene.y0 && y0+y<scene.y1)
            {
                theta[i]=t;
                status_points[i]=2;
                i++;
            }
            if(y0+y2>scene.y0 && y0+y2<scene.y1)
            {
                theta[i]=t2;
                status_points[i]=2;
                i++;
            }
        }
        //cout<<"i: "<<i<<"\n";
        //if(i>0)
        {
            double temp;
            int itemp;
            int best=-1;
            int c=0;

             //if(theta0 < (-PI/2))
            {
                for(int j=0;j<i;j++)
                {
                    if(theta[j] > (theta0+(PI*2)) )
                        theta[j]-=(PI*2);
                    else if(theta[j]+PI*2 > theta0 && theta[j]+PI*2 < theta1)
                        theta[j]+=PI*2;
                }
            }
            /*{
                for(int j=0;j<i;j++)
                {
                    if(theta[j] > (theta0+(PI*2)) )
                        theta[j]-=(PI*2);
                    else if(theta[j] <theta0 && theta[j]<-PI/2)
                        theta[j]+=PI*2;
                }
            }*/

            theta[i]=theta0;
            theta[i+1]=theta1;
            i+=2;

            for(int j=0;j<i-1;j++)
            {
                best=j;
                for(int jj=j+1;jj<i;jj++)
                {
                    if(theta[jj]<theta[best])
                        best=jj;
                }
                if(best>j)
                {
                    temp=theta[j];
                    theta[j]=theta[best];
                    theta[best]=temp;

                    itemp=status_points[j];
                    status_points[j]=status_points[best];
                    status_points[best]=itemp;
                    c++;
                }
            }
            //for(int j=0;j<i;j++)
            //    cout<<theta[j]*180/PI<<" ";

            //cout<<"\n";

            //chop points outside our angle range
            while(theta[i-1]>theta1)
                --i;
            int chop = 0;
            while(theta[chop] < theta0)
                ++chop;
            if(chop>0)
                for(int j=0;j<i;j++)
                {
                    theta[j]=theta[j+chop];
                    status_points[j]=status_points[j+chop];
                }
            i-=chop;

            //for(int j=0;j<i;j++)
            //    cout<<theta[j]*180/PI<<" ";
            //cout<<"\n";

            //for(int j=0;j<i;j++)
            //    cout<<theta[j]*180/PI<<" ";
            //cout<<"\n";

            double corner[6];
            poly.nCorners=0;
            corner[2]=x0;
            corner[3]=y0;
            corner[4]=x0+cos(theta0)*radius;
            corner[5]=y0+sin(theta0)*radius;
            corner[0]=x0+cos(theta1)*radius;
            corner[1]=y0+sin(theta1)*radius;
            poly.cornerArc=-1;

            //////////////////////////////////
            int status=0;
            int entry=-1;
            for(int j=0;j<2;j++)
            {
                //this will be easier if we do everything left to right
                bool switchlr=corner[j*2]>corner[(j+1)*2] || (corner[j*2]==corner[(j+1)*2] && corner[j*2+1]<corner[(j+1)*2]+1);

                double xx0 = !switchlr ? corner[j*2] : corner[(j+1)*2];
                double yy0 = !switchlr ? corner[j*2+1] : corner[(j+1)*2+1];
                double xx1 = !switchlr ? corner[(j+1)*2] : corner[j*2];
                double yy1 = !switchlr ? corner[(j+1)*2+1] : corner[j*2+1];
    /*
                if(poly.nPoints ==0 && xx0 != xx1 && xx1!=scene.x0)
                {
                    double test = (yy1-scene.y0)/(xx1-scene.x0) - (yy1-yy0)/(xx1-xx0);
                    //cout<<test<<"\n";
                    if(xx0<scene.x0 && scene.x0<xx1 &&  test >0)  //we are above
                        inside[0]=true;
                    else if(xx0<scene.x0 && scene.x0<xx1 &&  test <0) //below
                        inside[1]=true;

                    if(yy0<scene.y0 && scene.y0<yy1 &&  test >0)   //to the left
                        inside[2]=true;
                    else if(yy0<scene.y0 && scene.y0<yy1 &&  test <0) //to the right
                        inside[3]=true;
                }*/
/*
                if(xx0>scene.x0 && xx0<scene.x1 && yy0>scene.y0 && yy0<scene.y1 &&
                   xx1>scene.x0 && xx1<scene.x1 && yy1>scene.y0 && yy1<scene.y1)
                {
                  //  poly.AddPoint(CLIP_BORDER+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              CLIP_BORDER+(corner[j*2+1]-scene.y0)/(scene.y1-scene.y0)*scene.width);

                  //  poly.AddPoint(CLIP_BORDER+(corner[(j+1)*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              CLIP_BORDER+(corner[(j+1)*2+1]-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    continue;
                }*/
                //cout<<"x "<<j<<"\n";

                //local bounding box check
                if( std::min(xx0,xx1)>scene.x1 || std::max(xx0,xx1)<scene.x0 ||
                   std::min(yy0,yy1)>scene.y1 || std::max(yy0,yy1)<scene.y0 )
                    continue;
               // cout<<"x "<<j<<"\n";
             /*   if(xx1-xx0 == 0 && xx0>=scene.x0 && xx0<=scene.x1)
                {
                    int v_status=0;
                    if(corner[j*2+1]<scene.y0) v_status=1;
                    if(corner[j*2+1]>scene.y1) v_status=3;

                    if(entry != -1 && v_status !=0 && v_status !=status)
                    {
                        //addCorners(poly,status,v_status);
                        //cout<<"corner v "<<status<<" "<<v_status<<"\n";
                    }
                    else if(entry==-1)
                        entry = v_status;

                    if(switchlr)
                    {
                    poly.AddPoint(CLIP_BORDER+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              CLIP_BORDER+(max(corner[j*2+1],scene.y0)-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    poly.AddPoint(CLIP_BORDER+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              CLIP_BORDER+(min(corner[(j+1)*2+1],scene.y1)-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    }
                    else
                    {
                    poly.AddPoint(CLIP_BORDER+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              CLIP_BORDER+(min(corner[j*2+1],scene.y1)-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    poly.AddPoint(CLIP_BORDER+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              CLIP_BORDER+(max(corner[(j+1)*2+1],scene.y0)-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    }

                    if(corner[(j+1)*2+1]<scene.y0) status=1;
                    if(corner[(j+1)*2+1]>scene.y1) status=3;

                    continue;
                }
                else if(xx1-xx0 == 0)
                {
                    continue;}*/

                double slope = (yy1-yy0)/(xx1-xx0);
                double ix;
                double iy;
                double ix2;
                double iy2;
                double x;
                double y;
                double xx;
                double yy;
                int l_status=0;
                int r_status=0;

                iy= slope*(scene.x0-xx0)+yy0;
                ix= xx0+(scene.y0-yy0)/slope;
                ix2= xx0+(scene.y1-yy0)/slope;
                iy2= slope*(scene.x1-xx0)+yy0;

                if(xx0>=scene.x0 && xx0<=scene.x1 && yy0>=scene.y0 && yy0<=scene.y1 )
                {
                    x=xx0;
                    y=yy0;
                }
                else if(yy0<scene.y0 && ix > scene.x0 && ix <scene.x1)
                {
                    x=ix;
                    y=scene.y0;
                    l_status=1;
                }
                else if(yy0>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
                {
                    x=ix2;
                    y=scene.y1;
                    l_status=3;
                }
                else if(xx0<scene.x0 && iy >= scene.y0 && iy <= scene.y1)
                {
                    x=scene.x0;
                    y=iy;
                    l_status=4;
                }
                else{continue; }

                if(xx1>=scene.x0 && xx1<=scene.x1 && yy1>=scene.y0 && yy1<=scene.y1 )
                {
                    xx=xx1;
                    yy=yy1;
                }
                else if(yy1<scene.y0 && ix > scene.x0 && ix <scene.x1)
                {
                    xx=ix;
                    yy=scene.y0;
                    r_status=1;
                }
                else if(yy1>scene.y1 && ix2 > scene.x0 && ix2 <scene.x1)
                {
                    xx=ix2;
                    yy=scene.y1;
                    r_status=3;
                }
                else if(xx1>scene.x1 && iy2 >= scene.y0 && iy2 <= scene.y1)
                {
                    xx=scene.x1;
                    yy=iy2;
                    r_status=2;
                }
                else{continue; }

                if(!switchlr)
                {
                    if(entry != -1 && l_status>0)
                    {
                        //cout<<i<<" entering "<<l_status<<"\n";
                        if(l_status!=status)
                        {
                            //cout<<"corner a "<<status<<" "<<l_status<<"\n";
                            addCorners(poly,status,l_status);
                        }
                        //else status=0;
                    }
                    else if(entry==-1)
                        entry=l_status;
                    //cout<<i<<" entering "<<l_status<<"\n";

                    if(j>0 || l_status>0)
                        poly.AddPoint(SceneX(x,scene),
                                      SceneY(y,scene));

                    if(j==0 || r_status>0)
                        poly.AddPoint(SceneX(xx,scene),
                                      SceneY(yy,scene));

                    if(r_status>0)
                    {
                       // cout<<i<<" exiting "<<r_status<<"\n";
                        status=r_status;
                    }
                    //status=r_status;
                }
                else
                {
                    if( entry !=-1 && r_status>0)
                    {
                        //cout<<i<<" entering "<<r_status<<"\n";
                         if(r_status!=status)
                        {
                            //cout<<"corner b "<<status<<" "<<r_status<<"\n";
                            addCorners(poly,status,r_status);
                        }
                        else status=0;
                    }
                    else if(entry==-1)
                        entry=r_status;

                    if(j>0 || r_status>0)
                        poly.AddPoint(SceneX(xx,scene),
                                      SceneY(yy,scene));

                    if(j==0 || l_status>0)
                        poly.AddPoint(SceneX(x,scene),
                                      SceneY(y,scene));

                    if(l_status>0)
                    {
                        //cout<<i<<" exiting "<<l_status<<"\n";
                        status=l_status;
                    }
                }
            }
            //////////////////////////////////
           // if(i==2)paused=true;
            if(i>=2)
            {
                int first=-1;
                int last=-1;
                bool ok=bInvert;
                double theta_test = theta[0]+(theta[1]-theta[0])/2;
                if(x0+radius*cos(theta_test) > scene.x0 &&
                   x0+radius*cos(theta_test) < scene.x1 &&
                   y0+radius*sin(theta_test) > scene.y0 &&
                   y0+radius*sin(theta_test) < scene.y1 )
                    ok=!bInvert;
               // int cc=0;
                //cout<<"::";
                for(int j=0;j<i-1;j++)
                {
                    if(ok)
                    {
                       //cc++;
                        poly.addArc(theta[j],theta[j+1]);
                        poly.cornerArc++;
                        if(first==-1)
                            first=status_points[j];
                        last = status_points[j+1];
                       // cout<<"x";

                    }
                    ok=!ok;
                }
                std::cout<<"first: "<<first<<"\n";
                std::cout<<"last: "<<last<<"\n";
                std::cout<<"status: "<<status<<"\n";
                std::cout<<"entry: "<<entry<<"\n";
                //cout<<poly.nPoints<<"\n";
                if(entry==-1)entry=0;

                if(last!=entry && entry >0 && last>0)
                    addCorners(poly,last,entry,true);
                if(status!=first && status>0 && first>0)
                    addCorners(poly,status,first);
                if(first !=last&& first>0 &&last>0 &&entry==0)
                    addCorners(poly,last,first);
                if(status !=entry&& status>0 &&entry>0 && first==-1 && last==-1)
                    addCorners(poly,status,entry);
                return true;
            }
            return false;/*
                  //  for(int j=0;j<i;j++)
                  //      cout<<theta[j]*180/PI<<" ";
                  //  cout<<"\n";
              if(cc==0)
              {
                  cout<<"i="<<i<<"\n";
                 cout<<"Error!\n";
                 return false;
              }*/
        return true;
        }
        return false;
    }

void AngleObject::addCorners(ArcPoly_solid& poly,int a,int b,bool insert)
    {
        int c =b;
        if(a>=b) c+=4;
        //cout<<a<<" "<<b<<"\n";
        if(insert)
            for(int i=c-1;i>=a;i--)
            {
                    poly.InsertPoint(dog[i*2],dog[i*2+1]);
            }
        else
            for(int i=a;i<c;i++)
            {
                    poly.AddPoint(dog[i*2],dog[i*2+1]);
            }
    }

bool ArrowObject::getPoly(ArbPoly_solid& poly,Scene scene)
{
    double x,y,xx,yy,rot;

    poly.nPoints=0;

    x=SceneX(this->x0,scene);
    y=SceneY(this->y0,scene);
    xx=SceneX(this->x1,scene);
    yy=SceneY(this->y1,scene);

    //std::cout<<" "<<" "<<x<<","<<y<<" "<<xx<<","<<yy<<"\n";
    rot=atan2((yy-y),(xx-x));
    double len=sqrt((xx-x)*(xx-x)+(yy-y)*(yy-y));

    if(len<2) return false;
    double s = len<35?(len-2)/33:1;
    this->setLengthRotationPosition(    s,
                                        std::max(0.0,len-35),
                                        rot,
                                        x+(len>35?10:len/3.5)*cos(rot),y+(len>35?10:len/3.5)*sin(rot));

    static Scene myScene = {CLIP_BORDER,CLIP_BORDER,scene.width+CLIP_BORDER,scene.height+CLIP_BORDER,scene.width,scene.height};
    for(int i=(len>35?0:3);i<(len>35?7:6);i++)
    {
        x=points[i*2];
        y=points[i*2+1];
        xx=points[(i+1)*2];
        yy=points[(i+1)*2+1];

        if(ClipLine(x,y,xx,yy,myScene))
            {
                //std::cout<<i<<" "<<x<<","<<y<<" "<<xx<<","<<yy<<"\n";
               // poly.AddPoint(SceneX(x,scene),SceneY(yy,scene));
               // poly.AddPoint(SceneX(xx,scene),SceneY(yy,scene));
                poly.AddPoint(x,y);
                poly.AddPoint(xx,yy);
            }
      //  else std::cout<<i<<" outside\n";
    }
    //poly.nPoints=7;
    return true;
}
void ArrowObject::setLengthRotationPosition(double s,double len, double rot,double x,double y)
    {
       // x0=x;
       // y0=y;

        memcpy(&points[0],&points_tail[0],sizeof(double)*4);
        memcpy(&points[4],&points_head[0],sizeof(double)*10);

        for(int i=0;i<5;i++)
        {
            points[4+(i*2)]*=s;
            points[4+(i*2)+1]*=s;
            points[4+(i*2)]+=len;
        }
        double r,theta;
        for(int i=0;i<7;i++)
        {
            r=sqrt(points[i*2]*points[i*2]+points[i*2+1]*points[i*2+1]);
            theta=atan2(points[i*2],points[i*2+1])-PI/2+rot;
            points[i*2]=r*cos(theta)+x;
            points[i*2+1]=r*sin(theta)+y;
           // std::cout<<i<<" "<<points[i*2]<<","<<points[i*2+1]<<"\n";
        }
        points[14]=points[0];
        points[15]=points[1];
    }

