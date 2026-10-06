
#include <iostream>
#include <string.h>
#include <sstream>
#include "PlotObjects.h"
#include "PolyTypes.h"

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
double angle(double x0,double y0,double x1,double y1)
    {
    if(x1-x0 != 0)
        return atan2(y1-y0,x1-x0);
    else return y0 < y1 ? PI/2 : -PI/2;
    }

double iangle(double x0,double y0,double x1,double y1)
    {
    if(x1-x0 != 0)
        return atan2(y0-y1,x0-x1);
    else return y0 < y1 ? -PI/2 : PI/2;
    }


//
//================================================================================
//

void PathObject::addPoint(double x,double y,int ii)
    {

        int i=ii==-1?n_points:ii;

        std::cout<<"PathObject add point "<<i<<"\n";

        if(i<n_points)
            {
                for(int j=n_points;j>i;j--)
                {
                    m_points[2*j]=m_points[2*(j-1)];
                    m_points[2*j+1]=m_points[2*(j-1)+1];
                    m_stats[2*j]=m_stats[2*(j-1)];
                    m_stats[2*j+1]=m_stats[2*(j-1)+1];
                }
            }

        m_points[i*2]=x;
        m_points[i*2+1]=y;

        n_points++;

        double x0=m_points[(i-1)*2];
        double x1=m_points[(i-0)*2];
        double y0=m_points[(i-1)*2+1];
        double y1=m_points[(i-0)*2+1];

        if(i>0)
            {
            //length
            m_stats[i*2]=sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));

            //angle
            m_stats[i*2+1]= angle(x0,y0,x1,y1);
            }
        else
            {
                m_stats[0]=0;
                m_stats[1]=0;
            }

        for(Path_Draw* p = draw; p!=NULL; p=p->next)
            {
            p->addPoint(x,y);
            }
    }

void PathObject::popPoint()
    {
        n_points--;

        for(Path_Draw* p = draw; p!=NULL; p=p->next)
            {
            p->popPoint();
            }
    }

PathObject::PathObject()
    {
    // draw = new Path_Lines(this);
    /*
     draw = new Path_Markers(this);
     dynamic_cast<Path_Markers*>(draw)->n_ticks=4;
     dynamic_cast<Path_Markers*>(draw)->type=GRAPH_MARKERS_RADIUS4;

     draw->next = new Path_Labels(this);
    dynamic_cast<Path_Labels*>(draw->next)->n_ticks=2;
    dynamic_cast<Path_Labels*>(draw->next)->type=GRAPH_LABEL_LENGTH;

    draw->next->next = new Path_Lines(this);

    draw->next->next->next = new Path_Circle(this);
    */
    }
Path_Draw* PathObject::findGraph(int graph_type, int subtype)
{
    for(Path_Draw* p = draw; p!=NULL; p=p->next)
    {
        //std::cout<<p->graph_type<<" "<<p-> type<<"\n";
        if(p->graph_type == graph_type && (subtype== -1 || subtype == p-> type))
        {
            return p;
        }
    }
}

void PathObject::removeGraph(Path_Draw* p)
{
    if(p==draw)
    {
        draw=p->next;
        p->next=NULL;
        delete p;
    }
    else for(Path_Draw* pp = draw; pp!=NULL; pp=pp->next)
    {
        if(pp->next == p)
        {
         pp->next = p->next;
         p->next=NULL;
         delete p;
        }
    }
}

void PathObject::removeGraph(int graph_type, int subtype)
{
    Path_Draw* p;
    while(p=findGraph(graph_type,subtype))
          {
              removeGraph(p);
          }
}
/*
void PathObject::removeGraph(int graph_type, int subtype)
{
 for(Path_Draw* p = draw; p!=NULL; p=p->next)
    {
        //std::cout<<p->graph_type<<" "<<p-> type<<"\n";
        if(p->graph_type == graph_type && (subtype== -1 || subtype == p-> type))
        {
            //p->vacate();
         //   std::cout<<"removing...\n";
            if(p==draw)
            {
                draw=p->next;
                p->next=NULL;
                delete p;
                break;
            }
            else for(Path_Draw* pp = draw; pp!=NULL; pp=pp->next)
            {
                if(pp->next == p)
                {
                 //   cout<<"<----\n";
                 pp->next = p->next;
                 p->next=NULL;
                 delete p;
                 break;
                }
            }

        }
    }
}
*/

Path_Draw* PathObject::addGraph(int t,int st, int n,char flags)
{
    Path_Draw* p=NULL;

    switch(t)
    {
    case GRAPH_LINES:
        p = new Path_Lines(this);
    break;
    case GRAPH_CIRCLES:
        p = new Path_Circle(this);
        dynamic_cast<Path_Circle*>(p)->type=st;
    break;
    case GRAPH_LABELS:
        p = new Path_Labels(this);
        dynamic_cast<Path_Labels*>(p)->n_ticks=n;
        dynamic_cast<Path_Labels*>(p)->type=st;
    break;
    case GRAPH_MARKERS:
        p = new Path_Markers(this);
        dynamic_cast<Path_Markers*>(p)->n_ticks=n;
        dynamic_cast<Path_Markers*>(p)->type=st;
    break;
    }

    p->flags = flags;

    if(!draw) draw = p;
    else
    {
        Path_Draw* m = draw;
        while(m->next) {m=m->next;}
        m->next = p;
    }
    return p;
}



void PathObject::changed()
{
  //  std::cout<<"PathObject::changed "<<i<<"\n";
    for(int j=0;j<n_points;j++)
    {
        double x0=m_points[(j-1)*2];
        double x1=m_points[(j-0)*2];
        double y0=m_points[(j-1)*2+1];
        double y1=m_points[(j-0)*2+1];

        if(j>0)
            {
            //length
            m_stats[j*2]=sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
            //angle
            m_stats[j*2+1]= angle(x0,y0,x1,y1);
            }
        else
            {
                m_stats[0]=0;
                m_stats[1]=0;
            }
    }

    for(Path_Draw* p = draw; p!=NULL; p=p->next)
            {
            p->changed();
            }
}

void PathObject::removePoint(int n)
{
    for(int i=n;i<n_points-1;i++)
    {
         m_points[i*2]=m_points[(i+1)*2];
         m_points[i*2+1]=m_points[(i+1)*2+1];
         m_stats[i*2]=m_stats[(i+1)*2];
         m_stats[i*2+1]=m_stats[(i+1)*2+1];
    }

    n_points--;

    std::cout<<"removed point "<<n<<", "<<n_points<<" remaining\n";

    for(Path_Draw* p=draw;p!=NULL;p=p->next)
       // p->changed(1);
        p->init();

    changed();
}

//
//================================================================================
//

Path_Draw::Path_Draw(PathObject* d,int t)
    {
     data=d;
     graph_type=t;
    }

//
//================================================================================
//
Path_Circle::Path_Circle(PathObject* p) : Path_Draw(p,GRAPH_CIRCLES)
{
}
void Path_Circle::popPoint()
{
}
void Path_Circle::addPoint(double x,double y)
{
    if(data->n_points>1 || type==GRAPH_CIRCLE_COMPASS )
    {
    if(circles==NULL)
        {
            circles=new CircleObject(x,y,1);
        }
    }
}
void Path_Circle::changed()
{
     if(data->n_points>0)
    {
    if(circles)
        {
            switch(type)
            {
            case GRAPH_CIRCLE_RADIUS:
                if(data->n_points>1)
                {
                    circles->setPosition(data->m_points[0],data->m_points[1]);
                    circles->setRadius(data->m_stats[2]);
                }
                //circles->setRadius(50);
            break;
            case GRAPH_CIRCLE_COMPASS:
                circles->setPosition(data->m_points[(data->n_points-1)*2],data->m_points[(data->n_points-1)*2+1]);
                circles->setRadius(90);
            break;
            }
        }
    }
}
void Path_Circle::init()
{
}

//
//================================================================================
//

Path_Lines::Path_Lines(PathObject* d) : Path_Draw(d,GRAPH_LINES)
     {
     }


void Path_Lines::popPoint()
    {
        if(data->n_points>0 && lines)
        {
           // n_points-=1;
            LineObject* m = lines;
            while(m->next)
            {
                if(m->next->next==NULL)
                {
                 delete m->next;
                 m->next=NULL;
                 break;
                }
                m=m->next;
            }
            iPoint--;
        }

    }
#define LINE_GAP 5
void Path_Lines::addPoint(double x,double y)
{
      //  static int i=0;
       // std::cout<<"line- add point "<<iPoint<<"\n";
        if(iPoint>0)
        {
            if(!lines)
            {
                lines = new LineObject( data->m_points[iPoint*2], data->m_points[iPoint*2+1], data->m_points[(iPoint-1)*2], data->m_points[(iPoint-1)*2+1]);
                lines->layer=data->m_style.color_style;
            //std::cout<<"new line "<< data->m_points[0*2]<<","<< data->m_points[(0)*2+1]<<"  "<< data->m_points[(1)*2]<<","<< data->m_points[(1)*2+1]<<"\n";
            }
            else
            {
                LineObject* m = lines;
                while(m->next) m=m->next;
                LineObject* n = new LineObject( data->m_points[iPoint*2], data->m_points[iPoint*2+1], data->m_points[(iPoint-1)*2], data->m_points[(iPoint-1)*2+1]);
                n->layer=data->m_style.color_style;
              //  std::cout<<"new line "<<m->x1<<","<<m->y1<<"  "<<x<<","<<y<<"\n";
                m->next=n;
            }
        }
    iPoint++;
}

void Path_Lines::init()
{
    if(lines) delete lines;
    lines=NULL;
    iPoint=0;
    for(int i=0;i<data->n_points;i++)
    {
        addPoint(data->m_points[i*2],data->m_points[i*2+1]);
    }
}


void Path_Lines::changed()
    {
        int i=0;
        for(LineObject* n=lines;n!=NULL;n=n->next,i++)
        {
            n->SetPosition( data->m_points[i*2], data->m_points[i*2+1], data->m_points[(i+1)*2], data->m_points[(i+1)*2+1]);
        }
    }

//
//================================================================================
//

void PM_1_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a)
{
    if(j%2)
        {
        m.setPosition(x0+cos(a)*len*0.5,y0+sin(a)*len*0.5);
       // std::cout<<" mid\n";
        }
    else
        {
        //  std::cout<<" end\n";
        m.setPosition(x1,y1);
        }

    if(j%2==1)
        {
        m.type=MARKER_SMALL_ARROW;
        m.setRotation(a);
        }
    else
        {
        m.type=MARKER_DOT;
        // m.setRotation(iangle(data->m_points[(i)*2],data->m_points[(i)*2+1],data->m_points[(i-1)*2],data->m_points[(i-1)*2+1]));
        }
   // else
   //     m.type=MARKER_CROSS;

}


void PM_2_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a)
{
     if(j%4==0)
        {
            m.setPosition(x1,y1);
        }
    else
        {
        double r = (double)(j%4) / 4;
            m.setPosition(x0+cos(a)*len*r,y0+sin(a)*len*r);
        }

    if(i==0)
        {
        m.type=MARKER_DOT;
        }
    else if(i==n_points-1 && j%4==0)
        {
        m.type=MARKER_SMALL_ARROW;
        m.setRotation(a);
        }
    else if(j%4 != 0)
        {
        m.type=MARKER_DASH;
        m.setRotation(a+PI*0.5);
        if(j%4==2)
            m.setLength(6);
        else
            m.setLength(4);
        // m.setRotation(iangle(data->m_points[(i)*2],data->m_points[(i)*2+1],data->m_points[(i-1)*2],data->m_points[(i-1)*2+1]));
        }
    else
        m.type=MARKER_DOT;

}

void PM_RAD1_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a)
{/*
    if(i==0 && len > 5)
        {
        m.type=MARKER_SMALL_ARROW;
        m.setPosition(x1,y1);
        m.setRotation(PI+a);
        }
    else */if(i==n_points-1 && i>1)
        {
        m.type=MARKER_SMALL_ARROW;
        //m.setPosition(x0+cos(a)*len,y0+sin(a)*len);
        m.setPosition(x1,y1);
        m.setRotation(a);
        }
    else
    {
        m.setPosition(x1,y1);
        m.type=MARKER_DOT;
    }
}


void PM_ARROW_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a)
{
    if(i==n_points-1)
        {
        m.type=MARKER_SMALL_ARROW;
        //m.setPosition(x0+cos(a)*len,y0+sin(a)*len);
        m.setPosition(x1,y1);
        m.setRotation(a);
        }
    else
    {
        m.type=MARKER_HIDE;
    }
}


void PM_RULE3_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a)
{
    if(j%4==0)
        {
            m.setPosition(x1,y1);
        }
    else
        {
        double r = (double)(j%4) / 4;
            m.setPosition(x0+cos(a)*len*r,y0+sin(a)*len*r);
        }

   /* if(i==0 || (i==n_points-1 && j%4==0))
        {
        m.type=MARKER_DASH;
        m.setRotation(a+PI*0.5);
        m.setLength(6);
        }
    else*/
    if(j%4 != 0)
        {
        m.type=MARKER_DASH;
        m.setRotation(a+PI*0.5);
        if(j%4==2)
            m.setLength(6);
        else
            m.setLength(4);
        // m.setRotation(iangle(data->m_points[(i)*2],data->m_points[(i)*2+1],data->m_points[(i-1)*2],data->m_points[(i-1)*2+1]));
        }
    else
        m.type=MARKER_DOT;
}

void PM_SAMED_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a,double len_sum)
{

    if(j%2)
        m.setLength(3);
    else
        m.setLength(2);

    if(i==0)
        {
            m.setPosition(x1+cos(a)*len_sum,y1+sin(a)*len_sum);
        }
    else
        {
            m.setPosition(x0+cos(a)*len_sum,
                           y0+sin(a)*len_sum);
            m.x_offset=cos(a+PI*0.5)*(m.len+1);
            m.y_offset=sin(a+PI*0.5)*(m.len+1);
        }

    m.type=MARKER_DASH;
    m.setRotation(a+PI*0.5);

        // m.setRotation(iangle(data->m_points[(i)*2],data->m_points[(i)*2+1],data->m_points[(i-1)*2],data->m_points[(i-1)*2+1]));

}

void PM_4_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a)
{

    m.setPosition(x1,y1);
    m.type=MARKER_CROSS;
}

void PM_HIGHLITE_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a)
{
    //if(i==0)
    //    m.setPosition(x0,y0);
    //else
        m.setPosition(x1,y1);
    m.type=MARKER_SQUARE;
    m.layer=1;
}


void PM_GRID4_changed(bMarkerObject& m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a)
{
    double dx = x1-x0;
    double inc = dx/4;
    if(j%4==0)
        {
            m.setPosition(x1,y1);
        }
    else
        {
        double r = (double)(j%4) / 4;
            m.setPosition(x0+cos(a)*len*r,y0+sin(a)*len*r);
        }

    if(i==0 || (i==n_points-1 && j%4==0))
        {
        m.type=MARKER_DASH;
        m.setRotation(a+PI*0.5);
        m.setLength(6);
        }
    else if(j%4 != 0)
        {
        m.type=MARKER_DASH;
        m.setRotation(a+PI*0.5);
        if(j%4==2)
            m.setLength(6);
        else
            m.setLength(4);
        // m.setRotation(iangle(data->m_points[(i)*2],data->m_points[(i)*2+1],data->m_points[(i-1)*2],data->m_points[(i-1)*2+1]));
        }
    else
        m.type=MARKER_DOT;
}


void PM_COMP_changed(bMarkerObject& m,int j,
                  double x0,double y0,
                  double len,double a,double b)
{
    int n=36;
   // double b = 1;
    double inc = PI/n;
    double d = b-a;

    if(d<0)
        d=PI*2+d;

    if(d>PI*2)
        d=d-PI*2;
    d/=inc;

    int index = floor(b/inc);
    double offset = (b)-floor(b/inc)*inc;

    int i0 = index-floor(d);

    double t0 = inc*(i0-8)+offset;
    m.type=MARKER_DASH;

    i0-=1;
    while (i0-index<0)
   {
    i0+=n*2;
   }

    if(j>13)
    {
        m.setLength(8);
        m.setRotation(b);
        m.setPosition(cos(b)*40,
                   sin(b)*40);
        return;
    }
    else if((i0+j-index)%3==1)
        m.setLength(6);
    else
        m.setLength(2);

    m.setRotation(PI+t0+(PI/n)*j);
    m.setPosition(cos(PI+t0+(PI/n)*j)*(170-4),
                   sin(PI+t0+(PI/n)*j)*(170-4));
}


void Path_Markers::changed()
{
    //MarkerObject* m= NULL;

    int i,j,ii;
    double len_sum=0;
  //  std::cout<<type<<", current size = "<<bMarkers.size()<<" \n";
  //  std::cout<<bMarkers.capacity()<<"\n";
    total_len=0;
    for(int i=0;i<data->n_points;i++)
        {
            total_len+=data->m_stats[i*2];
        }

    if( flags & GRAPH_FLAGS_INTERVAL )
    {
        if(interval>0)
        {
            n_ticks=total_len/interval;
            bMarkers.resize(n_ticks);
        }
    }
    //std::cout<<n_ticks<<"\n";
    for(i=0,j=0;i<data->n_points && j<bMarkers.size();j++)
        {

        if(flags & GRAPH_FLAGS_INTERVAL && flags & GRAPH_FLAGS_INVERSE)
        {
            len_sum+=interval;
            ii= data->n_points-i-1;
            while(len_sum > data->m_stats[(ii)*2])
            {

                len_sum -= data->m_stats[(ii)*2];
                i++;
                ii = data->n_points-i-1;
            }

            if(ii<0)
            {
                bMarkers[j].type=MARKER_DASH;
                bMarkers[j].setLength(0);
                continue;
            }

        }
        else if(flags & GRAPH_FLAGS_INTERVAL)
        {
            len_sum+=interval;
              //  std::cout<<i<<" ";
                while(len_sum > data->m_stats[(i)*2])
                {
                    len_sum -= data->m_stats[(i)*2];
                    i++;
                }

                if(i>=data->n_points)
                {
                    bMarkers[j].type=MARKER_DASH;
                    bMarkers[j].setLength(0);
                    continue;
                }
        }
        bMarkers[j].layer=data->m_style.color_style;
       // std::cout<<"j= "<<j<<"\n";
        switch(type)
            {
            case GRAPH_MARKERS_SAMEDISTANCE:
               // std::cout<<i<<","<< j<<"\n";
                PM_SAMED_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i+1)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1],
                         len_sum);
            break;
            case GRAPH_MARKERS_SD_INV:
                PM_SAMED_changed(bMarkers[j],ii,j,data->n_points,
                         data->m_points[(ii)*2],data->m_points[(ii)*2+1],
                         data->m_points[(ii-1)*2],data->m_points[(ii-1)*2+1],
                         data->m_stats[(ii-1)*2],ii==0&&data->n_points>1?PI+data->m_stats[(1)*2+1]:PI+data->m_stats[(ii)*2+1],
                         len_sum);
            break;
            case GRAPH_MARKERS_DIV4:
                PM_RULE3_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1]);
            break;
            case GRAPH_MARKERS_RADIUS1:
                PM_RAD1_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         i==0&&data->n_points>1?data->m_stats[(1)*2]:data->m_stats[(i)*2],
                         i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1]);
            break;
            case GRAPH_MARKERS_ARROW:

                PM_ARROW_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         i==0&&data->n_points>1?data->m_stats[(1)*2]:data->m_stats[(i)*2],
                         i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1]);
            break;
            case GRAPH_MARKERS_RADIUS4:
                PM_2_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1]);
            break;
            case GRAPH_MARKERS_WAYPOINT2:
                //std::cout<<"m_changed: "<<data->m_points[(i-1)*2]<<","<<data->m_points[(i-1)*2+1]<<" --> "<<data->m_points[(i)*2]<<","<<data->m_points[(i)*2+1]<<" i="<<i<<", j="<<j<<" n="<<data->n_points<<"\n";
                PM_1_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1]);
            break;
            case GRAPH_MARKERS_MARK1:
               // std::cout<<"mark\n";
                PM_4_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1]);
              //  std::cout<<"mark2\n";
            break;
            case GRAPH_MARKERS_HIGHLITE:
                PM_HIGHLITE_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(index-1)*2],data->m_points[(index-1)*2+1],
                         data->m_points[(index)*2],data->m_points[(index)*2+1],
                         data->m_stats[(index)*2],index==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(index)*2+1]);
            break;
            case GRAPH_MARKERS_GRID4:
                PM_GRID4_changed(bMarkers[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1]);
            break;
            case GRAPH_MARKERS_COMPASS:
               // std::cout<<i<<" "<<j<<"\n";
                PM_COMP_changed(bMarkers[j],j,
                         data->m_points[(data->n_points-1)*2],data->m_points[(data->n_points-1)*2+1],
                         data->m_stats[(i)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1],0);
            break;
            case GRAPH_MARKERS_PROTRACTOR:
                PM_COMP_changed(bMarkers[j],j,
                         0,0,
                         data->m_stats[(index)*2],
                         PI+data->m_stats[(index+1)*2+1],data->m_stats[(index)*2+1]);
            break;
            }

            if( flags & GRAPH_FLAGS_INTERVAL)
                {
                    //len_sum+=interval;
                }
            else if( flags & GRAPH_FLAGS_SINGLE && j%n_ticks==0 && j>0 )
                {
                    i++;
                }
            else if (j%n_ticks==0)
                i++;
        }
}

void Path_Markers::popPoint()
{
    int c=0;

    if(!(flags & GRAPH_FLAGS_SINGLE))
    {
        bMarkers.resize(bMarkers.size()-n_ticks>0?bMarkers.size()-n_ticks:0);
        iPoint--;
    }
}

void Path_Markers::addPoint(double x,double y)
{

    if(flags & GRAPH_FLAGS_EXISTS  && flags & GRAPH_FLAGS_SINGLE)
        return;

    flags |= GRAPH_FLAGS_EXISTS;

    total_len=0;
    for(int i=0;i<data->n_points;i++)
        {
            total_len+=data->m_stats[i*2];
        }

    if( flags & GRAPH_FLAGS_INTERVAL )
    {
        if(interval>0)
        {
            n_ticks=total_len/interval;
        }
        else
        {
            return;
        }

        bMarkers.resize(n_ticks);
    }
    else if( flags & GRAPH_FLAGS_SINGLE)
    {
        bMarkers.resize(n_ticks);
    }
    else if( bMarkers.size() == 0)
    {
        bMarkers.resize(1);
    }
    else
    {
        bMarkers.resize(bMarkers.size()+n_ticks);
    }

     std::cout<<"Marker type "<<type<<" addPoint size="<<bMarkers.size()<<" n_points="<<data->n_points<<"\n";

    iPoint++;

    changed();
   // set_markers();
}

void Path_Markers::init()
{
    std::cout<<"init type="<<type<<" n_points="<<data->n_points<<"\n";
  //  if(markers) delete markers;
  //  markers=NULL;

    iPoint=0;
    total_len=0;
    /*
    if(data->n_points > bMarkers.size())
    {
        bMarkers.resize(bMarkers.size()+8);
        std::cout<<"pod_array new size "<<bMarkers.size()<<"\n";
    }
    */
    bMarkers.resize(0);
    for(int i=0;i<data->n_points;i++)
    {
        //std::cout<<i<<"\n";
        addPoint(data->m_points[i*2],data->m_points[i*2+1]);
    }
    changed();
}

Path_Markers::Path_Markers(PathObject* p) : Path_Draw(p,GRAPH_MARKERS)
{
    type=MARKER_SMALL_CIRCLE;
}


//
//================================================================================
//


Path_Labels::Path_Labels(PathObject* p)  : Path_Draw(p,GRAPH_LABELS)
{

}

void Path_Labels::addPoint(double x,double y)
{

    if(flags & GRAPH_FLAGS_EXISTS  && flags & GRAPH_FLAGS_SINGLE)
        return;

    flags |= GRAPH_FLAGS_EXISTS;

    int total_len=0;
    for(int i=0;i<data->n_points;i++)
        {
            total_len+=data->m_stats[i*2];
        }

    if( flags & GRAPH_FLAGS_INTERVAL )
    {
        if(interval>0)
        {
            n_ticks=total_len/interval;
        }
        else
        {
            return;
        }
        bLabels.resize(n_ticks);
    }
    else if( flags & GRAPH_FLAGS_SINGLE)
    {
        bLabels.resize(n_ticks);
    }
    else
    {
        bLabels.resize(bLabels.size()+n_ticks);
    }

    switch(type)
    {
        default:
            if(!labels)
            {
                labels = new LabelObject(x,y);
                //labels=m;
                if(flags & GRAPH_FLAGS_SINGLE)
                {
                LabelObject* m = labels;
                while(m->next) m=m->next;

                for(int i=0;i<n_ticks-1;i++)
                    {
                        LabelObject* n = new LabelObject(x,y);
                        m->next = n;
                        m=m->next;
                    }
                }
            }
            else
            {
                LabelObject* m = labels;
                while(m->next) m=m->next;

                for(int i=0;i<n_ticks;i++)
                {
                    LabelObject* n = new LabelObject(x,y);
                    m->next = n;
                    m=m->next;
                }
            }
        break;
    }
    changed();
   // if(next) next->addPoint(x,y);
   // set_labels();
}
void Path_Labels::init()
{
    if(labels) delete labels;
    labels=NULL;
    bLabels.resize(0);
    iPoint=0;
    for(int i=0;i<data->n_points;i++)
    {
        addPoint(data->m_points[i*2],data->m_points[i*2+1]);
    }
    changed();
}
void Path_Labels::popPoint()
{
    if(!(flags & GRAPH_FLAGS_SINGLE))
    {
         bLabels.resize(bLabels.size()-n_ticks>0?bLabels.size()-n_ticks:0);
        for(int i=0;i<n_ticks;i++)
         if(data->n_points>=0)
                {

                   // n_points-=1;
                    LabelObject* m = labels;
                    if(!m)return;

                    while(m->next)
                    {
                        if(m->next->next==NULL)
                        {
                         delete m->next;
                         m->next=NULL;
                         break;
                        }
                        m=m->next;
                    }
                }
    iPoint--;
    }
    changed();
   // if(next) next->popPoint();
}

void PL_2_changed(bLabelObject &m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a,double total_len)
{
        m.setPosition(x0+cos(a)*len*1.0,y0+sin(a)*len*1.0);
        m.x_offset=10;
        m.y_offset=0;
        if(i>0)
        {
           // len+=data->m_stats[i*2];
            m.setText(patch::to_string(total_len));
           // m.type=0;
        }
        else
        {
            m.setText("");
        }
}


void PL_3_changed(bLabelObject &m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a,double total_len)
{

    if(j%2)
        {
        m.setPosition(x0+cos(a)*len*0.5,y0+sin(a)*len*0.5);
        }
    else
        {
        m.setPosition(x1,y1);
        }
    m.x_offset=-20;
    m.y_offset=-10;
    m.setText(patch::to_string((char('A'+(char)j))));
}

void PL_1_changed(bLabelObject &m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a,double total_len)
{
   // if(j%2==0)
    {
        m.setPosition(x1,y1);
        m.x_offset=-20;
        m.y_offset=-10;
        m.setText(patch::to_string(i));
    }
}

void PL_X4(bLabelObject &m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a,double total_len)
{
    double x;
    double y;

    double r = (double)(j%2) / 2;

    if(j%2!=0)
    {
        x=x0+cos(a)*len*r;
        y= y0+sin(a)*len*r;
    }
    else
    {
        x=x1;
        y=y1;
    }

    m.setPosition(x,y);

    m.x_offset=0;
    m.y_offset=16;
    m.setText(patch::to_string(x));

}

void PL_Y4(bLabelObject &m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a,double total_len)
{
    double x;
    double y;

    double r = (double)(j%2) / 2;

    if(j%2!=0)
    {
        x=x0+cos(a)*len*r;
        y= y0+sin(a)*len*r;
    }
    else
    {
        x=x1;
        y=y1;
    }

    m.setPosition(x,y);

    m.x_offset=-40;
    m.y_offset=-2;
    m.setText(patch::to_string(y));

}

void PL_SAMED_changed(bLabelObject &m,int i,int j,int n_points,
                  double x0,double y0,
                  double x1,double y1,
                  double len,double a,double len_sum,double total_len)
{
    if(i==0)
        {
            m.setPosition(x1+cos(a)*len_sum,y1+sin(a)*len_sum);
        }
    else
        {
            m.setPosition(x0+cos(a)*len_sum,
                           y0+sin(a)*len_sum);
            m.x_offset=cos(a+PI*0.5)*3+3;
            m.y_offset=sin(a+PI*0.5)*3;
        }


    //m.type=MARKER_DASH;
    //m.setRotation(a+PI*0.5);
    //m.setLength(4);
    m.setText(patch::to_string(total_len));
        // m.setRotation(iangle(data->m_points[(i)*2],data->m_points[(i)*2+1],data->m_points[(i-1)*2],data->m_points[(i-1)*2+1]));

}

void PL_COMP_changed(bLabelObject &m,int j,
                  double x0,double y0,
                  double len,double a,double b)
{
    int n=24;
   // double b = 1;
    double inc = PI/n;
    double d = b-a;

    int index = floor(b/inc);
    double offset = (b)-floor(b/inc)*inc;

    while(d<0)
        d=PI*2+d;

    while(d>PI*2)
        d=d-PI*2;

    d/=inc;

    int i0=index-floor(d);
    if(abs(i0-index)%2==1)offset+=inc;
    double t0 = inc*(i0-4)+offset;


    i0-=1;
    while (i0-index<0)
   {
    i0+=n*2;
   }
    double aa = PI+t0+(PI/(n/2))*j;
    int i2 = (i0-index)/2+j-1;
    i2=i2>n-1?i2-n:i2;
    i2=i2<0?i2+n:i2;
    m.setText(patch::to_string(360*i2*inc/(PI))     );
    m.setPosition(x0,y0);

    if(j==4)
    {
        if(fabs(d)<0.0001)d=0;
        m.setText(patch::to_string("bitchez!"));
        m.x_offset=10;
        m.y_offset=-25;
    }
   // else if((i0+j)%4==0)
  //  {
  //      m.x_offset=cos(aa)*(180+32)-4;
  //      m.y_offset=sin(aa)*(180+32)+4;
  //  }
    else
    {
        m.x_offset=cos(aa)*(180+16)-4;
        m.y_offset=sin(aa)*(180+16)+4;
    }
}

void Path_Labels::changed()
{
    len=0;
    double total_len=0;
    double len_sum=0;
    for(int i=0;i<data->n_points;i++)
        {
            total_len+=data->m_stats[i*2];
        }

    if( flags & GRAPH_FLAGS_INTERVAL )
    {
        if(interval>0)
        {
            n_ticks=total_len/interval;
            bLabels.resize(n_ticks);
        }
    }

    int i,j,ii;
    for(i=0,j=0;i<data->n_points && j<bLabels.size();j++)
        {
        if(j%n_ticks==0)
            len+=data->m_stats[i*2];

        if(flags & GRAPH_FLAGS_INTERVAL && flags & GRAPH_FLAGS_INVERSE)
        {
            len_sum+=interval;
            ii= data->n_points-i-1;
            while(len_sum > data->m_stats[(ii)*2])
            {

                len_sum -= data->m_stats[(ii)*2];
                i++;
                ii = data->n_points-i-1;
            }

            if(ii<0)
            {
                bLabels[j].setText("");
                continue;
            }
        }
        else if(flags & GRAPH_FLAGS_INTERVAL)
        {
            len_sum+=interval;
            while(len_sum > data->m_stats[(i)*2])
            {
                len_sum -= data->m_stats[(i)*2];
                i++;
            }

            if(i>=data->n_points)
            {
                bLabels[j].setText("");
                continue;
            }
        }
        switch(type)
        {
            case GRAPH_LABEL_SAMEDISTANCE:

               bLabels[j].type=1;
                PL_SAMED_changed(bLabels[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i+1)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1],
                         len_sum,(j+1)*interval);
            break;
            case GRAPH_LABEL_SD_INV:
                bLabels[j].type=1;
                PL_SAMED_changed(bLabels[j],ii,j,data->n_points,
                         data->m_points[(ii)*2],data->m_points[(ii)*2+1],
                         data->m_points[(ii-1)*2],data->m_points[(ii-1)*2+1],
                         data->m_stats[(ii-1)*2],ii==0&&data->n_points>1?PI+data->m_stats[(1)*2+1]:PI+data->m_stats[(ii)*2+1],
                         len_sum,(j+1)*interval);
            break;
            case GRAPH_LABEL_INDEX:
                if(j%n_ticks==0)
                {
                bLabels[j].type=0;
                PL_1_changed(bLabels[j],i,j,data->n_points,
                                 data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                                 data->m_points[(i)*2],data->m_points[(i)*2+1],
                                 data->m_stats[(i)*2],data->m_stats[(i)*2+1],len);
                }
            break;
            case GRAPH_LABEL_LENGTH:
                {
                bLabels[j].type=1;
                PL_2_changed(bLabels[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],data->m_stats[(i)*2+1],len);
                }
            break;
            case GRAPH_LABEL_ALPHABET:
                bLabels[j].type=0;
                PL_3_changed(bLabels[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],data->m_stats[(i)*2+1],len);
            break;
            case GRAPH_LABEL_X4:
                bLabels[j].type=1;
                PL_X4(bLabels[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],data->m_stats[(i)*2+1],len);
            break;
            case GRAPH_LABEL_Y4:
                bLabels[j].type=1;
                PL_Y4(bLabels[j],i,j,data->n_points,
                         data->m_points[(i-1)*2],data->m_points[(i-1)*2+1],
                         data->m_points[(i)*2],data->m_points[(i)*2+1],
                         data->m_stats[(i)*2],data->m_stats[(i)*2+1],len);
            break;
            case GRAPH_LABEL_COMPASS:
                bLabels[j].type=1;
                PL_COMP_changed(bLabels[j],j,
                         data->m_points[(data->n_points-1)*2],data->m_points[(data->n_points-1)*2+1],
                         data->m_stats[(i)*2],i==0&&data->n_points>1?data->m_stats[(1)*2+1]:data->m_stats[(i)*2+1],0);
            break;
            case GRAPH_LABEL_PROTRACTOR:
                bLabels[j].type=1;
                //std::cout<<index<<"\n";
                PL_COMP_changed(bLabels[j],j,
                         data->m_points[(index)*2],data->m_points[(index)*2+1],
                         data->m_stats[(index)*2],
                         PI+data->m_stats[(index+1)*2+1],data->m_stats[(index)*2+1]);
            break;
        }

   //     switch(type)
   //         {
                if( flags & GRAPH_FLAGS_INTERVAL )
                    {
                    }
                else if( flags & GRAPH_FLAGS_SINGLE && j%n_ticks==0 && j>0 )
                    //if (j%n_ticks==0)
                    {
                        i++;
                    }
     //               break;
     //           default:
                else    if (j%n_ticks==0)
                        i++;
     //               break;
     //       }
        }//for
 //  if(next) next->changed(q);
}

