
#ifndef _PLOT_H_
#define _PLOT_H_

#include "PlotObjects.h"
#include "NoiseMap.h"

class Plot
{
public:
    int gridspacing;
    int object_group=1;
    CircleObject* circles;
    LineObject* lines;
    AngleObject* angles;
    MarkerObject* markers;
    LabelObject* labels;
    ArrowObject* arrows;
    ArbObject* islands;
    CurveObject* curves;
    PointCloudObject* pointclouds;
    PathObject* paths;

    Plot():circles(NULL),lines(NULL),angles(NULL),markers(NULL),labels(NULL),arrows(NULL),islands(NULL),curves(NULL),pointclouds(NULL),paths(NULL){}
    ~Plot()
    {
     if(circles) delete circles;
     if(lines) delete lines;
     if(angles) delete angles;
     if(markers) delete markers;
     if(labels) delete labels;
     if(arrows) delete arrows;
     if(islands) delete islands;
     if(curves) delete curves;
     if(pointclouds) delete pointclouds;
     if(paths) delete paths;
    }
    void newObjectGroup()
    {
        object_group++;
    }

    void deleteMarkersLabels(int groupno)
    {
    if(markers)
        {
            MarkerObject* m;
            MarkerObject* mm;

            m=markers;
            mm=m->next;
            while(mm !=NULL)
            {
                if(mm->group==groupno)
                {
                    m->next=mm->next;
                    mm->next=NULL;
                    delete mm;
                    mm=m->next;
                }
                else
                {
                    m=m->next;
                    mm=m->next;
                }
            }
            if(markers->group==groupno)
            {
                m=markers;
                markers=m->next;
                m->next = NULL;
                delete m;
            }
        }
    if(labels)
        {
            LabelObject* m;
            LabelObject* mm;

            m=labels;
            mm=m->next;
            while(mm !=NULL)
            {
                if(mm->group==groupno)
                {
                    m->next=mm->next;
                    mm->next=NULL;
                    delete mm;
                    mm=m->next;
                }
                else
                {
                    m=m->next;
                    mm=m->next;
                }
            }
            if(labels->group==groupno)
            {
                m=labels;
                labels=m->next;
                m->next = NULL;
                delete m;
            }
        }
    }

    void addPath()
    {
     PathObject* path = new PathObject();
     path->next = paths;
     paths = path;
    }

    void addPointCloud(int n)
    {
     PointCloudObject* cloud = new PointCloudObject(n);
     cloud->next = pointclouds;
     pointclouds = cloud;
    }

    void addCurve(bezier_object bez)
    {
    CurveObject* n = new CurveObject();

    n->n_bez=bez.n_bez;
    n->m_bez = new BezObject[bez.n_bez];

    for(int i=0;i<bez.n_bez;i++)
    {
        n->m_bez[i].init(bez.m_bez[i]);

    }

    std::cout<<"Added Curve Object: "<<bez.n_bez<<" curves \n";

    n->init();
    n->next = curves;
    n->group=object_group;
    curves = n;
    }

    void addCircle(double x,double y,double r)
    {
    CircleObject* n = new CircleObject(x,y,r);

    n->next = circles;
    n->group=object_group;
    circles = n;
    }

    void popCircle()
    {
    CircleObject* p = circles;
    circles=circles->next;
    deleteMarkersLabels(p->group);
    p->next=NULL;
    delete p;
    }

    void addArrow()
    {
    ArrowObject* n = new ArrowObject;

    n->next = arrows;
    n->group=object_group;
    arrows = n;

    }

    void popArrow()
    {
    ArrowObject* p = arrows;

    arrows=arrows->next;
    deleteMarkersLabels(p->group);
    p->next=NULL;
    delete p;
    }

    void addLabel(double x,double y)
    {
    LabelObject* n = new LabelObject(x,y);

    n->next = labels;
    n->group=object_group;
    labels = n;

    }
    void addMarker(double x,double y,int type, int group = -1)
    {
    MarkerObject* n = new MarkerObject(x,y,type,0.0);

    n->next = markers;
    if(group==-1)
        n->group=object_group;
    else
        n->group=group;
    markers = n;

    }
    void addAngle(double x,double y,double r,double t0,double t1)
    {
    AngleObject* n = new AngleObject(x,y,r,t0,t1);

    n->next = angles;
    n->group=object_group;
    angles = n;
    }
    void addLine(double x,double y,double xx,double yy)
    {
    LineObject* n = new LineObject(x,y,xx,yy);

    n->next = lines;
    n->group=object_group;
    lines = n;
    }
    void popLine()
    {
    LineObject* p = lines;
    lines=lines->next;
    deleteMarkersLabels(p->group);
    p->next=NULL;
    delete p;
    }
};


namespace patch
{
    template < typename T > std::string to_string( const T& n );
}

class PlotTool_Line
{
public:
    static void LeftClick(double x,double y);
    static void MouseMove(double x,double y);
};

enum{
 LAYER_NORMAL=0,
 LAYER_SELECTED
};

enum {
 TOOL_LINE=0,
 TOOL_COMPASS,
 TOOL_WAYPOINT,
 TOOL_PROTRACTOR,
 TOOL_ERASER,
 TOOL_WAYPOINT2
};
void SetToolPlot(Plot* plot);

class PlotTool_Compass
{
    static CircleObject* circle;
    static LineObject* line;
    static MarkerObject* mark_c;
    static MarkerObject* mark_r;
    static LabelObject* label;
    static bool bDragRadius;
    static double drag_x_begin;
    static double drag_y_begin;

    public:
    static void LeftClick(double x,double y);
    static void MouseMove(double x,double y);
    static void RightClick(double x,double y);

    static bool StartDrag(double x,double y,double r);
    static void EndDrag(double x,double y);
    static void Drag(double x,double y);
};

class PlotTool_Protractor
{
    public:
    static void LeftClick(double x,double y);
    static void MouseMove(double x,double y);
    static void RightClick(double x,double y);
};

class PlotTool_Waypoint
{
    static ArrowObject* drag1;
    static ArrowObject* drag2;
    static MarkerObject* mark;

    public:
    static void LeftClick(double x,double y);
    static void MouseMove(double x,double y);
    static void RightClick(double x,double y);

    static bool StartDrag(double x,double y,double r);
    static void EndDrag(double x,double y);
    static void Drag(double x,double y);
    static void Push_Z(double x,double y);
    static void Push_S(double x,double y);
    static void EndTool();
};

enum{
  TOOL2_LINE,
  TOOL2_CIRCLE,
  TOOL2_WP,
  TOOL2_SCATTER,
  TOOL2_COMPASS,
  TOOL2_PROTRACTOR
};

class PlotTool_Waypoint2
{
    static int drag1;
    static int max_points;
    static int tool_no;
   // static int drag2;
    //static MarkerObject* mark;

    public:
    static void LeftClick(double x,double y);
    static void MouseMove(double x,double y,double inverse_screen);
    static void RightClick(double x,double y);

    static bool StartDrag(double x,double y,double r);
    static void EndDrag(double x,double y);
    static void Drag(double x,double y);
    static void Push_Z(double x,double y);
    static void Push_S(double x,double y);
    static void ToolChange(double x,double y);
    static void EndTool(double x,double y);
    static void StartTool(double x,double y);
    static void ChangeStyle(int i,double x,double y);
    static void ApplyStyles(double x,double y);
};



class PlotTool_Eraser
{
    public:
    static void LeftClick(double x,double y);
    static void MouseMove(double x,double y,double t);
    static void RightClick(double x,double y);
};


#endif

