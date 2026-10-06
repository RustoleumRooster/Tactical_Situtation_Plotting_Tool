#include "Plot.h"
#include "PlotObjects.h"
#include <string.h>
#include <sstream>
#include "ASSERTS.h"

int tool_status;
Plot* myPlot;

using namespace std;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

void SetToolPlot(Plot* plot)
{
 myPlot=plot;
}


void PlotTool_Waypoint::LeftClick(double x,double y)
{
    if(tool_status == 0)
    {
        tool_status=1;
        myPlot->newObjectGroup();
        if(myPlot->arrows==NULL)
        {
            myPlot->addArrow();
            myPlot->arrows->setPosition(x,y,x,y);
            //myPlot->addLabel(x,y);
            myPlot->addMarker(x,y,MARKER_SMALL_CIRCLE);
            myPlot->addMarker(x,y,MARKER_SMALL_CIRCLE);

        }
        else
        {
            myPlot->addArrow();
            myPlot->arrows->setPosition(myPlot->arrows->next->x1,myPlot->arrows->next->y1,x,y);
            myPlot->addMarker(x,y,MARKER_SMALL_CIRCLE);
        }
        //myPlot->addMarker(x,y,2);
    }
    else if(tool_status == 1)
    {
        tool_status=0;
        //myPlot->lines->layer=0;
        //myPlot->markers->layer=0;
        //myPlot->markers->next->layer=0;
    }
}
void PlotTool_Waypoint::MouseMove(double x,double y)
{
    if(tool_status==1)
    {
        myPlot->arrows->setEndpoint(x,y);
        myPlot->markers->setPosition(x,y);
        //myPlot->markers->setRotation(myPlot->lines->angle());
       // myPlot->labels->setPosition(myPlot->lines->x0+myPlot->lines->length()/2*cos(myPlot->lines->angle()),
        //myPlot->lines->y0+myPlot->lines->length()/2*sin(myPlot->lines->angle()));
        //myPlot->labels->setText(patch::to_string(myPlot->lines->length()));
        //myPlot->circles->setRadius(myPlot->lines->length());
    }
}
void PlotTool_Waypoint::RightClick(double x,double y)
{
    if(tool_status==1)
    {
        myPlot->popArrow();
        tool_status=0;
    }
}

ArrowObject* PlotTool_Waypoint::drag1=NULL;
ArrowObject* PlotTool_Waypoint::drag2=NULL;
MarkerObject* PlotTool_Waypoint::mark=NULL;

bool PlotTool_Waypoint::StartDrag(double x,double y,double r)
{
    ArrowObject* p;
    MarkerObject* m;

    if(tool_status != 0)
        return false;

    drag1=NULL;
    drag2=NULL;
    mark=NULL;


    for(p = myPlot->arrows; p != NULL; p=p->next)
    {
        if(sqrt((p->x0-x)*(p->x0-x)+(p->y0-y)*(p->y0-y)) < r*Marker_small_circle_poly::radius)
        {
            drag1=p;
        }
        else if(sqrt((p->x1-x)*(p->x1-x)+(p->y1-y)*(p->y1-y)) < r*Marker_small_circle_poly::radius)
        {
            drag2=p;
        }
    }
     for(m = myPlot->markers; m != NULL; m=m->next)
        {
            if(sqrt((m->x0-x)*(m->x0-x)+(m->y0-y)*(m->y0-y)) < r*Marker_small_circle_poly::radius
                && ( (drag1 && (m->group==drag1->group)) || (drag2 && m->group==drag2->group)))
            {
                mark=m;
                mark->layer=LAYER_SELECTED;
            }
        }
    if(drag1 || drag2)
    {
        return true;
    }
    else return false;
}
void PlotTool_Waypoint::EndDrag(double x,double y)
{
    if(mark)
    {
        mark->layer=LAYER_NORMAL;
    }
    drag1=NULL;
    drag2=NULL;
    mark=NULL;
}

void PlotTool_Waypoint::Push_S(double x,double y)
{
    ArrowObject* p;
    ArrowObject* n;
    if(drag1)
    {
        if(myPlot->arrows==drag1)
        {
            myPlot->newObjectGroup();
            n = new ArrowObject();
            n->next = drag1->next;
            drag1->next=n;
            n->group=myPlot->object_group;
            n->setPosition(drag1->x0,drag1->y0,
                           drag1->x0+0.5*(drag1->x1-drag1->x0),
                           drag1->y0+0.5*(drag1->y1-drag1->y0));
            drag1->setOrigin(n->x1,n->y1);
            myPlot->deleteMarkersLabels(drag1->group);
            myPlot->addMarker(drag1->x1,drag1->y1,MARKER_SMALL_CIRCLE,drag1->group);
            if(n->next==NULL)
            {
                myPlot->addMarker(n->x0,n->y0,MARKER_SMALL_CIRCLE);
                mark=myPlot->markers;
                mark->layer=LAYER_SELECTED;
            }
            myPlot->addMarker(n->x1,n->y1,MARKER_SMALL_CIRCLE);
            drag1=n;
        }
        else
            for(p=myPlot->arrows;p!=NULL;p=p->next)
            {
            if(p->next==drag1)
                {
                    myPlot->newObjectGroup();
                    n = new ArrowObject();
                    n->next = drag1->next;
                    drag1->next=n;
                    n->group=myPlot->object_group;
                    n->setPosition(drag1->x0,drag1->y0,
                                   drag1->x0+0.5*(p->x0-drag1->x0),
                                   drag1->y0+0.5*(p->y0-drag1->y0));
                    drag1->setOrigin(n->x1,n->y1);
                    myPlot->deleteMarkersLabels(drag1->group);
                    if(n->next==NULL)
                    {
                        myPlot->addMarker(n->x0,n->y0,MARKER_SMALL_CIRCLE);
                        mark=myPlot->markers;
                        mark->layer=LAYER_SELECTED;
                    }
                    myPlot->addMarker(n->x1,n->y1,MARKER_SMALL_CIRCLE);
                    myPlot->addMarker(drag1->x1,drag1->y1,MARKER_SMALL_CIRCLE,drag1->group);
                    drag1=n;
                    break;
                }
            }
    }
}
void PlotTool_Waypoint::Push_Z(double x,double y)
{
    ArrowObject* p;
    if(drag1 && drag2 && drag2 == drag1->next)
    {
        drag1->setOrigin(drag2->x0,drag2->y0);
        drag1->next = drag2->next;
        if(drag2->next==NULL)
        {
         myPlot->addMarker(drag2->x0,drag2->y0,MARKER_SMALL_CIRCLE,drag1->group);
         mark=myPlot->markers;
        }
        myPlot->deleteMarkersLabels(drag2->group);
        if(drag1->next)
        {
            myPlot->deleteMarkersLabels(drag1->next->group);
            myPlot->addMarker(drag1->next->x1,drag1->next->y1,MARKER_SMALL_CIRCLE,drag1->next->group);
            mark=myPlot->markers;
            if(drag1->next->next == NULL)
            {
                myPlot->addMarker(drag1->next->x0,drag1->next->y0,MARKER_SMALL_CIRCLE,drag1->next->group);
            }
        }
        if(mark)
            mark->layer=LAYER_SELECTED;
        drag2->next=NULL;
        delete drag2;
        drag2=drag1->next;
        //drag2=NULL;
        //EndDrag(x,y);

    }
    else if(drag1 && !drag2)
    {

        p=NULL;
        if(myPlot->arrows==drag1)
        {
            myPlot->arrows=drag1->next;
            mark=NULL;
        }
        else
            for(p=myPlot->arrows;p!=NULL;p=p->next)
            {
            if(p->next==drag1)
                {
                    p->next=drag1->next;
                    myPlot->addMarker(p->x0,p->y0,MARKER_SMALL_CIRCLE,p->group);
                    mark=myPlot->markers;
                    mark->layer=LAYER_SELECTED;
                    break;
                }
            }
        drag1->next=NULL;
        myPlot->deleteMarkersLabels(drag1->group);

        delete drag1;
        drag1=p;

    }
    else if(drag2 && !drag1 && drag2 == myPlot->arrows)
    {
        myPlot->arrows=drag2->next;
        drag2->next=NULL;
        myPlot->deleteMarkersLabels(drag2->group);
        if(myPlot->arrows)
        {
            myPlot->deleteMarkersLabels(myPlot->arrows->group);
            myPlot->addMarker(myPlot->arrows->x1,myPlot->arrows->y1,MARKER_SMALL_CIRCLE,myPlot->arrows->group);
            mark=myPlot->markers;
            mark->layer=LAYER_SELECTED;
            if(myPlot->arrows->next==NULL)
            {
                myPlot->addMarker(myPlot->arrows->x0,myPlot->arrows->y0,MARKER_SMALL_CIRCLE,myPlot->arrows->group);
            }
        }
        delete drag2;
        drag2=myPlot->arrows;
    }
}
void PlotTool_Waypoint::Drag(double x,double y)
{
    if(drag1)
    {
     drag1->setOrigin(x,y);
    // drag1->setPosition()
    }
    if(drag2)
    {
     drag2->setEndpoint(x,y);
    }
    if(mark)
    {
     mark->setPosition(x,y);
    }
     //   myPlot->markers->setPosition(x,y);
}

void PlotTool_Waypoint::EndTool()
{

    if(tool_status==1)
    {
        myPlot->popArrow();
        tool_status=0;
    }
}

//
//=====================================================
//

int PlotTool_Waypoint2::drag1=-1;
//int PlotTool_Waypoint::drag2=-1;
//MarkerObject* PlotTool_Waypoint::mark=NULL;

void PlotTool_Waypoint2::Drag(double x,double y)
{

    if(drag1>=0)
    {
     myPlot->paths->m_points[drag1*2]=x;
     myPlot->paths->m_points[drag1*2+1]=y;
     myPlot->paths->changed();
    // drag1->setPosition()
    }
}

int PlotTool_Waypoint2::tool_no=0;
int PlotTool_Waypoint2::max_points=2;

void PlotTool_Waypoint2::LeftClick(double x,double y)
{
    if( tool_status == 1)
    {
           // tool_status=0;
           ASSERT(myPlot && myPlot->paths);
           if(myPlot->paths->n_points == max_points)
           {
            switch(tool_no)
                {
                    case TOOL2_COMPASS:
                            myPlot->paths->removeGraph(GRAPH_CIRCLES);
                            myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_COMPASS);
                            myPlot->paths->removeGraph(GRAPH_LABELS,GRAPH_LABEL_COMPASS);
                            break;

                    case TOOL2_PROTRACTOR:
                            myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_PROTRACTOR);
                            myPlot->paths->removeGraph(GRAPH_LABELS,GRAPH_LABEL_PROTRACTOR);
                            break;
                }
            StartTool(x,y);
            return;
            }
            else
            {
                Path_Draw* n;
             switch(tool_no)
                {
                    case TOOL2_COMPASS:
                        //myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_COMPASS,14,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                        //myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_COMPASS,4,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                        break;

                    case TOOL2_PROTRACTOR:
                        myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_PROTRACTOR);
                        myPlot->paths->removeGraph(GRAPH_LABELS,GRAPH_LABEL_PROTRACTOR);
                        n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_PROTRACTOR,14,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                        dynamic_cast<Path_Markers*>(n)->index=myPlot->paths->n_points-1;
                        n=myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_PROTRACTOR,4,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                        dynamic_cast<Path_Labels*>(n)->index=myPlot->paths->n_points-1;
                        break;
                }
            }
    }

    if(tool_status == 0 || tool_status==2)
    {
    if(tool_status==0)
        myPlot->addPath();

    Path_Draw* n;

    switch(tool_no)
        {
            case TOOL2_LINE:
                myPlot->paths->addGraph(GRAPH_LINES,0,2);
                myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_DIV4,4);
                myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_LENGTH,1);
                max_points=2;
            break;
            case TOOL2_CIRCLE:
                myPlot->paths->addGraph(GRAPH_LINES,GRAPH_CIRCLE_RADIUS,2);
                myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_RADIUS4,4);
                myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_LENGTH,1);
                myPlot->paths->addGraph(GRAPH_CIRCLES,0,2);
                max_points=2;
            break;
            case TOOL2_WP:
                myPlot->paths->addGraph(GRAPH_LINES,0,2);
                myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_WAYPOINT2,2);
                myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_INDEX,1);
                max_points=-1;
            break;
            case TOOL2_SCATTER:
                //myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_MARK1,1);
               // myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_INDEX,2);
                max_points=-1;
                myPlot->paths->m_style.marker_style=0;
                PlotTool_Waypoint2::ApplyStyles(x,y);
            break;
            case TOOL2_COMPASS:
                //myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_COMPASS);
                myPlot->paths->addGraph(GRAPH_LINES,0,2);
                myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_COMPASS,14,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_COMPASS,4,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                max_points=2;
              //  myPlot->paths->addGraph(GRAPH_CIRCLES,GRAPH_CIRCLE_COMPASS,2,GRAPH_FLAGS_SCREENCOORDS);
              //  myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_MARK1,1);
               // myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_MARK1,1);
               // myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_INDEX,2);
               // max_points=;
            break;
            case TOOL2_PROTRACTOR:
                myPlot->paths->addGraph(GRAPH_LINES,0,2);
                myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_RADIUS1,1);

                n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_PROTRACTOR,14,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                dynamic_cast<Path_Markers*>(n)->index=0;
                n=myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_PROTRACTOR,4,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                dynamic_cast<Path_Labels*>(n)->index=0;
                //myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_MARK1,1);
                max_points=-1;
                break;
        }
    }
    myPlot->paths->addPoint(x,y);

    if(tool_status == 0)
    {
        myPlot->paths->addPoint(x,y);
        tool_status=1;
    }
    else if(tool_status==2)
    {
       // std::cout<<"whoa\n";
     tool_status=1;
    }
}


void PlotTool_Waypoint2::EndTool(double x,double y)
{
PathObject* p;
switch(tool_no)
    {
        case TOOL2_COMPASS:
            p = myPlot->paths;
            myPlot->paths= p->next;
            delete p;
            tool_status=0;
        break;
        default:
            RightClick(0,0);

            tool_status=0;
        break;
    }
}

void PlotTool_Waypoint2::StartTool(double x,double y)
{

switch(tool_no)
    {
        case TOOL2_COMPASS:
            tool_status=2;
            myPlot->addPath();
            myPlot->paths->addGraph(GRAPH_CIRCLES,GRAPH_CIRCLE_COMPASS,1,GRAPH_FLAGS_SCREENCOORDS);
            myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_MARK1,1);
            //myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_COMPASS,14,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
            //myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_COMPASS,4,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
            myPlot->paths->addPoint(x,y);
            myPlot->paths->changed();
            max_points=2;
        break;
        default:
            tool_status=0;
        break;
    }
}

void PlotTool_Waypoint2::ToolChange(double x,double y)
{
    PlotTool_Waypoint2::EndTool(x,y);

    tool_no++;
    if(tool_no>5)
        tool_no=0;
    switch(tool_no)
    {
        case TOOL2_LINE:
            cout<<"Tool: Ruler\n";
        break;
        case TOOL2_CIRCLE:
            cout<<"Tool: Circle\n";
        break;
        case TOOL2_WP:
            cout<<"Tool: Waypoint\n";
            break;
        case TOOL2_SCATTER:
            cout<<"Tool: Point\n";
        break;
         case TOOL2_COMPASS:
            cout<<"Tool: Compass\n";
              PlotTool_Waypoint2::StartTool(x,y);
        break;
        case TOOL2_PROTRACTOR:
            cout<<"Tool: Protractor\n";
              //PlotTool_Waypoint2::StartTool(x,y);
        break;
    }
}

//bool bFrontGrab=false;
bool bHover=false;
void PlotTool_Waypoint2::MouseMove(double x,double y,double inverse_screen)
{
    if(tool_status==1 || tool_status==2)
    {
        /*
        if(bFrontGrab)
        {
         myPlot->paths->m_points[(0)*2]=x;
         myPlot->paths->m_points[(0)*2+1]=y;
         myPlot->paths->changed(0);
        }

        else
            */
        {
         myPlot->paths->m_points[(myPlot->paths->n_points-1)*2]=x;
         myPlot->paths->m_points[(myPlot->paths->n_points-1)*2+1]=y;
         myPlot->paths->changed();
        }
     return;
    }
    PathObject* p,pp;
    Path_Draw* pd;

    if(!bHover)
    for(p = myPlot->paths;p!=NULL;p=p->next)
    {
        for(int i=0;i<p->n_points;i++)
        {
            if(sqrt((p->m_points[i*2]-x)*(p->m_points[i*2]-x)+(p->m_points[i*2+1]-y)*(p->m_points[i*2+1]-y)) < inverse_screen * (Marker_small_circle_poly::radius+1))
            {
                if(p != myPlot->paths)
                {
                    PathObject* m=myPlot->paths;
                    while(m->next != p) m=m->next;
                    m->next=p->next;
                    p->next=myPlot->paths;
                    myPlot->paths=p;
                }
               // cout<<"hover\n";
                pd = myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_HIGHLITE,1,GRAPH_FLAGS_SINGLE);
                Path_Markers* pm = dynamic_cast<Path_Markers*>(pd);
                pm->index=i;
                //for(int j=0;j<p->n_points;j++)
                pd->addPoint(x,y);

                bHover=true;
                pd->changed();

               // std::cout<<"selection ["<<i<<"]:\n n_points="<<myPlot->paths->n_points<<"\n";
                for(pd = myPlot->paths->draw;pd!=NULL;pd=pd->next)
                {
               //  std::cout<<" "<<pd->graph_type<<" "<<pd->type<<" i="<<pd->iPoint<<"\n";
                }
                return;
            }
        }
    }
    else
    {
        p = myPlot->paths;
        for(int i=0;i<p->n_points;i++)
        {
            if(sqrt((p->m_points[i*2]-x)*(p->m_points[i*2]-x)+(p->m_points[i*2+1]-y)*(p->m_points[i*2+1]-y)) < inverse_screen * (Marker_small_circle_poly::radius+4))
            {
                return;
            }
        }

        myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_HIGHLITE);
        //cout<<"no hover\n";
        bHover=false;
    }
}


void PlotTool_Waypoint2::Push_Z(double x,double y)
{
    if(!bHover || !myPlot->paths ) return;

    Path_Draw* pd = myPlot->paths->findGraph(GRAPH_MARKERS,GRAPH_MARKERS_HIGHLITE);
    if(pd!=NULL)
    {
        myPlot->paths->removePoint(dynamic_cast<Path_Markers*>(pd)->index);
        myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_HIGHLITE);
    }
    bHover=false;


}

const static int scale_intervals[10]={1,2,5,10,25,50,100,200,500,1000};

void PlotTool_Waypoint2::ChangeStyle(int style_no,double x,double y)
{
    if(!(bHover || tool_status==1) || !myPlot->paths) return;

    switch (style_no)
    {
        case 1:
            myPlot->paths->m_style.marker_style++;
            if(myPlot->paths->m_style.marker_style>3)
                myPlot->paths->m_style.marker_style=0;
            break;
        case 2:
            myPlot->paths->m_style.line_style++;
            if(myPlot->paths->m_style.line_style>1)
                myPlot->paths->m_style.line_style=0;
            break;
        case 3:
            myPlot->paths->m_style.index_style++;
            if(myPlot->paths->m_style.index_style>1)
                myPlot->paths->m_style.index_style=0;
            break;
        case 4:
            myPlot->paths->m_style.scale_style++;
            if(myPlot->paths->m_style.scale_style>2)
                myPlot->paths->m_style.scale_style=0;
            break;
        case 5:
            myPlot->paths->m_style.color_style++;
            if(myPlot->paths->m_style.color_style>5)
                myPlot->paths->m_style.color_style=0;
            break;
        case -1:
            myPlot->paths->m_style.scale_interval++;
            if(myPlot->paths->m_style.scale_interval>9)
                myPlot->paths->m_style.scale_interval=9;
            break;
        case -2:
            myPlot->paths->m_style.scale_interval--;
            if(myPlot->paths->m_style.scale_interval<0)
                myPlot->paths->m_style.scale_interval=0;
            break;
    }
    ApplyStyles(x,y);
}

void PlotTool_Waypoint2::ApplyStyles(double x,double y)
{
    if( !myPlot->paths) return;


    Path_Draw* n;


    myPlot->paths->removeGraph(GRAPH_MARKERS,-1);
    myPlot->paths->removeGraph(GRAPH_LINES,-1);
    myPlot->paths->removeGraph(GRAPH_LABELS,-1);

  //
    switch(myPlot->paths->m_style.marker_style)
    {
        case 0:
            n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_MARK1,1);
            n->init();
            break;
        case 1:
            n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_RADIUS1,1);
            n->init();
            break;
        case 2:
            n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_WAYPOINT2,2);
            n->init();
            break;
        case 3:
            n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_ARROW,1);
            n->init();
            break;
            /*
        case 3:
            n=myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_SAMEDISTANCE,1,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_INTERVAL);
            dynamic_cast<Path_Labels*>(n)->interval=50;
            n->init();
            n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_SAMEDISTANCE,1,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_INTERVAL);
            dynamic_cast<Path_Markers*>(n)->interval=50;
            n->init();
            break;
            */
    }

    switch(myPlot->paths->m_style.line_style)
    {
        case 0:

            break;
        case 1:
           //myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_RADIUS1,1);
           // myPlot->paths->draw->init();
            n=myPlot->paths->addGraph(GRAPH_LINES,0,2);
           // dynamic_cast<Path_Lines*>(n)->lines
            n->init();
            break;
    }



    switch(myPlot->paths->m_style.index_style)
    {
        case 0:

            break;
        case 1:
         //   n=myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_ALPHABET,1);
         //   n->init();
         n=myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_INDEX,1);
            n->init();

            break;
    }

    switch(myPlot->paths->m_style.scale_style)
    {
        case 0:
           break;
        case 1:
            n=myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_SD_INV,1,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_INTERVAL | GRAPH_FLAGS_INVERSE);
            dynamic_cast<Path_Labels*>(n)->interval=scale_intervals[myPlot->paths->m_style.scale_interval];
            n->init();
            n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_SD_INV,1,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_INTERVAL | GRAPH_FLAGS_INVERSE);
            dynamic_cast<Path_Markers*>(n)->interval=scale_intervals[myPlot->paths->m_style.scale_interval];
            n->init();
            break;
        case 2:
             n=myPlot->paths->addGraph(GRAPH_LABELS,GRAPH_LABEL_SAMEDISTANCE,1,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_INTERVAL);
            dynamic_cast<Path_Labels*>(n)->interval=scale_intervals[myPlot->paths->m_style.scale_interval];
            n->init();
            n=myPlot->paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_SAMEDISTANCE,1,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_INTERVAL);
            dynamic_cast<Path_Markers*>(n)->interval=scale_intervals[myPlot->paths->m_style.scale_interval];
            n->init();
            break;
    }
}


void PlotTool_Waypoint2::RightClick(double x,double y)
{

if(tool_status==1)
    {
    //myPlot->paths->popPoint();
    switch(tool_no)
        {
            case TOOL2_PROTRACTOR:
                myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_PROTRACTOR);
                myPlot->paths->removeGraph(GRAPH_LABELS,GRAPH_LABEL_PROTRACTOR);
                myPlot->paths->popPoint();
                if(myPlot->paths->n_points==1)
                {
                 PathObject* p = myPlot->paths;
                 myPlot->paths= p->next;
                 delete p;
                }
                tool_status=0;
                break;
            case TOOL2_COMPASS:

                myPlot->paths->popPoint();

                if(myPlot->paths->n_points==1)
                {
                   myPlot->paths->removeGraph(GRAPH_LINES);
                    tool_status=2;
                }
                else
                {
                    myPlot->paths->changed();
                    myPlot->paths->removeGraph(GRAPH_CIRCLES);
                    myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_COMPASS);
                    myPlot->paths->removeGraph(GRAPH_LABELS,GRAPH_LABEL_COMPASS);
                    StartTool(x,y);
                }

            break;/*
            case TOOL2_SCATTER:
             myPlot->paths->popPoint();
                if(myPlot->paths->n_points==0)
                {
                 PathObject* p = myPlot->paths;
                 myPlot->paths= p->next;
                 delete p;
                }
                tool_status=0;
                break;*/
            default:
                myPlot->paths->popPoint();
                if(myPlot->paths->n_points==1)
                {
                 PathObject* p = myPlot->paths;
                 myPlot->paths= p->next;
                 delete p;
                }
                tool_status=0;
                myPlot->paths->changed();
            break;
        }
    }
}

bool PlotTool_Waypoint2::StartDrag(double x,double y,double r)
{
    if(tool_status != 0) return false;

    for(PathObject* p = myPlot->paths;p!=NULL;p=p->next)
    {
        for(int i=0;i<p->n_points;i++)
        {
            if(sqrt((p->m_points[i*2]-x)*(p->m_points[i*2]-x)+(p->m_points[i*2+1]-y)*(p->m_points[i*2+1]-y)) < r*(Marker_small_circle_poly::radius+1))
            {
                if(p != myPlot->paths)
                {
                    PathObject* m=myPlot->paths;
                    while(m->next != p) m=m->next;
                    m->next=p->next;
                    p->next=myPlot->paths;
                    myPlot->paths=p;
                }

                Path_Draw* n=NULL;

                switch(tool_no)
                {
                    case TOOL2_PROTRACTOR:
                       // std::cout<<"way cool!\n";
                        //int ii=i<1?1:i;
                        int ii=i>p->n_points-2?p->n_points-2:i-1;

                        if(ii<0)
                            break;

                        std::cout<<"protractor :"<<ii<<"\n";
                        n = p->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_PROTRACTOR,14,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                        dynamic_cast<Path_Markers*>(n)->index=ii;
                        n->addPoint(p->m_points[ii*2],p->m_points[ii*2+1]);

                        n = p->addGraph(GRAPH_LABELS,GRAPH_LABEL_PROTRACTOR,4,GRAPH_FLAGS_SINGLE | GRAPH_FLAGS_SCREENCOORDS);
                        dynamic_cast<Path_Labels*>(n)->index=ii;
                        n->addPoint(p->m_points[ii*2],p->m_points[ii*2+1]);
                        break;
                }

                drag1=i;
                return true;
            }
        }
    }

    return false;
}

void PlotTool_Waypoint2::EndDrag(double x,double y)
{
    switch(tool_no)
    {
        case TOOL2_PROTRACTOR:
                myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_PROTRACTOR);
                myPlot->paths->removeGraph(GRAPH_LABELS,GRAPH_LABEL_PROTRACTOR);
                break;
    }
    drag1=-1;

}

void PlotTool_Waypoint2::Push_S(double x,double y)
{
    if(myPlot->paths && tool_status==0 && !bHover)
    {
     myPlot->paths->addPoint(x,y);
     myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_HIGHLITE);
     tool_status=1;
    }
    else if(myPlot->paths && bHover && tool_status==0)
    {
        Path_Draw* pd = myPlot->paths->findGraph(GRAPH_MARKERS,GRAPH_MARKERS_HIGHLITE);
        if(pd!=NULL)
        {
            int i = dynamic_cast<Path_Markers*>(pd)->index;
            if(i>0)
            {
                double x = myPlot->paths->m_points[(i-1)*2] + 0.5*( myPlot->paths->m_points[i*2] - myPlot->paths->m_points[(i-1)*2] );
                double y = myPlot->paths->m_points[(i-1)*2+1] + 0.5*( myPlot->paths->m_points[i*2+1] - myPlot->paths->m_points[(i-1)*2+1] );
                myPlot->paths->addPoint(x,y,i);
            }
            //myPlot->paths->removeGraph(GRAPH_MARKERS,GRAPH_MARKERS_HIGHLITE);
        }
    }

}

