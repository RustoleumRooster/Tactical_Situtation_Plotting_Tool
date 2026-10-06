#include "PolyTypes.h"
#include <agg_basics.h>




void set_screen_corners(Scene scene,int b)
{
    screen_corners[0] = b,
    screen_corners[1] = b,
    screen_corners[2] = b,
    screen_corners[3] = scene.height + b,
    screen_corners[4] = scene.width + b,
    screen_corners[5] = scene.height + b,
    screen_corners[6] = scene.width + b,
    screen_corners[7] = b,
    screen_corners[8] = b,
    screen_corners[9] = b,
    screen_corners[10] = b,
    screen_corners[11] = scene.height + b,
    screen_corners[12] = scene.width + b,
    screen_corners[13] = scene.height + b,
    screen_corners[14] = scene.width + b,
    screen_corners[15] = b,
    screen_corners[16] = b,
    screen_corners[17] = b;
}


unsigned BezierPoly::vertex(double* x,double* y)
{
     if(increment >= nPoints+1) return agg::path_cmd_stop;
        double t=t0+((double)(increment)/nPoints)*(t1-t0);
        double t3=t*t*t;
        double t2=t*t;
        /*
        (1-t)(1-t)
        (1-2t+t2)
        (1-2t+t2)(1-t)
        1-2t+t2 -t+2t2-t3
        */
        *x= ax*(1-3*t+3*t2-t3)  +
            bx*3*(1-2*t+t2)*t   +
            cx*3*(1-t)*t2       +
            dx*t3;

        *y= ay*(1-3*t+3*t2-t3)  +
            by*3*(1-2*t+t2)*t   +
            cy*3*(1-t)*t2       +
            dy*t3;

        n_vertexes++;
        if(increment==t0)
        {
            ++increment;
            return agg::path_cmd_move_to;
        }
        ++increment;
        return agg::path_cmd_line_to;
}


unsigned CurvePoly_solid::vertex(double* x,double* y)
{
        int z1,z2;
        if(step >= nSteps+1)
        {
            increment++;
            if(increment>range[(iInterval*2)+1])
            {
                z1=corners[(iInterval*2)];
                z2=corners[(iInterval*2)+1];
                if((step-(nSteps+1))<(z2-z1))
                   {
                       *x=screen_corners[(1+z1+(step-(nSteps+1)))*2];
                       *y=screen_corners[((1+z1+(step-(nSteps+1)))*2)+1];
                    //std::cout<<*x<<" "<<*y<<" corner \n";
                       step++;
                    //   std::cout<<z1<<" "<<z2<<"\n";

                       //if(*x<31.9)
                       /*
                        {
                            std::cout<<"corner "<<*x<<"\n";
                            std::cout<<"  inc: "<<increment<<" intv: "<<iInterval<<" step: "<<step<<"/"<<nSteps<<"\n";
                            std::cout<<"       "<<interval[iInterval*2]<<" "<<interval[(iInterval*2)+1]<<" "<<range[iInterval*2]<<" "<<range[(iInterval*2)+1]<<"\n";
                            std::cout<<"  nIntervals: "<<nIntervals<<"\n";
                            paused=true;
                        }
                        */
                       return agg::path_cmd_line_to;
                   }
                else
                    {
                    increment=0;
                    iInterval++;
                    }
            }
            if(iInterval >= nIntervals)
            {
                return agg::path_cmd_stop;
            }

            step=0;
            nSteps=N_STEPS;
        }

        if(increment==0)
                t0 = interval[(iInterval*2)]-(double)range[(iInterval*2)];
            else
                t0=0.0;

        if(increment==range[(iInterval*2)+1])
            {
                t1 = interval[(iInterval*2)+1]-(double)(range[(iInterval*2)+1]+range[(iInterval*2)]);
            }
        else
            t1=1.0;

        double t=t0+(((double)step/nSteps)*(t1-t0));
        double t3=t*t*t;
        double t2=t*t;

        /*
        (1-t)(1-t)
        (1-2t+t2)
        (1-2t+t2)(1-t)
        1-2t+t2 -t+2t2-t3
        */

        *x= m_bez[range[iInterval*2]+increment].ax*(1-3*t+3*t2-t3)  +
            m_bez[range[iInterval*2]+increment].bx*3*(1-2*t+t2)*t   +
            m_bez[range[iInterval*2]+increment].cx*3*(1-t)*t2       +
            m_bez[range[iInterval*2]+increment].dx*t3;

        *y= m_bez[range[iInterval*2]+increment].ay*(1-3*t+3*t2-t3)  +
            m_bez[range[iInterval*2]+increment].by*3*(1-2*t+t2)*t   +
            m_bez[range[iInterval*2]+increment].cy*3*(1-t)*t2       +
            m_bez[range[iInterval*2]+increment].dy*t3;

        n_vertexes++;

        if(*x<31.9)
        {
            std::cout<<"miss "<<*x<<"\n";
            std::cout<<"  inc: "<<increment<<" intv: "<<iInterval<<" step: "<<step<<"/"<<nSteps<<" t: "<<t<<"\n";
            std::cout<<"       "<<interval[iInterval*2]<<" "<<interval[(iInterval*2)+1]<<" "<<range[iInterval*2]<<" "<<range[(iInterval*2)+1]<<"\n";
            std::cout<<"  nIntervals: "<<nIntervals<<"\n";
            paused=true;
        }

        if(increment==0 && step==0 && close_to[iInterval]==-2 /*&& close_to[iInterval]==-1*/)
        {
            ++step;
           //  std::cout<<*x<<" "<<*y<<"  move \n";
           return agg::path_cmd_move_to;
          // return agg::path_cmd_line_to;
        }
         //std::cout<<*x<<" "<<*y<<" \n";
        ++step;
        return agg::path_cmd_line_to;
} //CURVE POLY SOLID

unsigned CurvePoly::vertex(double* x,double* y)
{
   // std::cout<<increment<<"-\n";
        if(step >= nSteps+1)
        {
           // std::cout<<"z\n";
            increment++;
           // std::cout<<"inc "<<increment<<" / "<<range[(iInterval*2)+1]<<"\n";
          //  std::cout<<"n="<<nIntervals<<"\n";
            if(increment>range[(iInterval*2)+1])
            {
                increment=0;
                iInterval++;
            }
            if(iInterval >= nIntervals)
                return agg::path_cmd_stop;

            step=0;
            nSteps=N_STEPS;
        }

        if(increment==0)
                t0 = interval[(iInterval*2)]-(double)range[(iInterval*2)];
            else
                t0=0.0;

        if(increment==range[(iInterval*2)+1])
            {
                t1 = interval[(iInterval*2)+1]-(double)(range[(iInterval*2)+1]+range[(iInterval*2)]);
            }
        else
            t1=1.0;

        double t=t0+(((double)step/nSteps)*(t1-t0));
        double t3=t*t*t;
        double t2=t*t;

        /*
        (1-t)(1-t)
        (1-2t+t2)
        (1-2t+t2)(1-t)
        1-2t+t2 -t+2t2-t3
        */

        *x= m_bez[range[iInterval*2]+increment].ax*(1-3*t+3*t2-t3)  +
            m_bez[range[iInterval*2]+increment].bx*3*(1-2*t+t2)*t   +
            m_bez[range[iInterval*2]+increment].cx*3*(1-t)*t2       +
            m_bez[range[iInterval*2]+increment].dx*t3;

        *y= m_bez[range[iInterval*2]+increment].ay*(1-3*t+3*t2-t3)  +
            m_bez[range[iInterval*2]+increment].by*3*(1-2*t+t2)*t   +
            m_bez[range[iInterval*2]+increment].cy*3*(1-t)*t2       +
            m_bez[range[iInterval*2]+increment].dy*t3;

        n_vertexes++;

        if(*x < 10)
        {
         std::cout<<"t0="<<t0<<" step="<<step<<" inc="<<increment<<"   "<<*x<<","<<*y<<"\n";
         paused=true;
        }

        if(increment==0 && step==0)
        {
            ++step;
            return agg::path_cmd_move_to;
        }
        ++step;
        return agg::path_cmd_line_to;
}

unsigned LinePoly::vertex(double* x,double* y)
    {

        if ( increment == 2)
            return agg::path_cmd_stop;

        n_vertexes++;

        if( increment == 0)
        {
            *x = x0;
            *y = y0;
            ++increment;
            return agg::path_cmd_move_to;
        }

        if ( increment == 1 )
        {
            *x = x1;
            *y = y1;
            ++increment;
            return agg::path_cmd_line_to;
        }
    }

unsigned ArbPoly::vertex(double* x,double* y)
    {
        if ( increment == nPoints)
            return agg::path_cmd_stop;


        n_vertexes++;

        *x = point[increment*2];
        *y = point[increment*2+1];


        if( increment%2 == 0)
        {
            ++increment;
            return agg::path_cmd_move_to;
        }

        ++increment;
        return agg::path_cmd_line_to;
    }

unsigned Marker_dot_poly::vertex(double* x,double* y)
    {
        if(increment==nPoints+1)
            return agg::path_cmd_stop;

        n_vertexes++;

        *x= x0+cos(PI*2*increment/nPoints)*radius;
        *y= y0+sin(PI*2*increment/nPoints)*radius;

        if( increment == 0)
        {
            ++increment;
            return agg::path_cmd_move_to;
        }

        ++increment;
        return agg::path_cmd_line_to;
    }

unsigned Marker_small_circle_poly::vertex(double* x,double* y)
    {
        if(increment==nPoints+1)
            return agg::path_cmd_stop;

        n_vertexes++;

        *x= x0+cos(PI*2*increment/nPoints)*radius;
        *y= y0+sin(PI*2*increment/nPoints)*radius;

        if( increment == 0)
        {
            ++increment;
            return agg::path_cmd_move_to;
        }

        ++increment;
        return agg::path_cmd_line_to;
    }


unsigned Marker_dash_poly::vertex(double* x,double* y)
    {
        n_vertexes++;
        if( increment == 0)
        {
            *x= x0+cos(rot)*length;
            *y= y0+sin(rot)*length;
            ++increment;
            return agg::path_cmd_move_to;
        }
        else if( increment == 1)
        {
            *x= x0-cos(rot)*length;
            *y= y0-sin(rot)*length;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else
            return agg::path_cmd_stop;
    }

unsigned Marker_cross_poly::vertex(double* x,double* y)
    {
        n_vertexes++;
        if( increment == 0)
        {
            *x= x0-length+0.3333;
            *y= y0+0.3333;
            ++increment;
            return agg::path_cmd_move_to;
        }
        else if( increment == 1)
        {
            *x= x0+length+0.3333;
            *y= y0+0.3333;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else if( increment == 2)
        {
            *x= x0+0.3333;
            *y= y0-length+0.3333;
            ++increment;
            return agg::path_cmd_move_to;
        }
        else if( increment == 3)
        {
            *x= x0+0.3333;
            *y= y0+length+0.3333;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else
            return agg::path_cmd_stop;
    }

unsigned Marker_square_poly::vertex(double* x,double* y)
    {
        n_vertexes++;
        if( increment == 0)
        {
            *x= x0-length+0.3333;
            *y= y0-length+0.3333;
            ++increment;
            return agg::path_cmd_move_to;
        }
        else if( increment == 1)
        {
            *x= x0+length+0.3333;
            *y= y0-length+0.3333;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else if( increment == 2)
        {
            *x= x0+length+0.3333;
            *y= y0+length+0.3333;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else if( increment == 3)
        {
            *x= x0-length+0.3333;
            *y= y0+length+0.3333;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else if( increment == 4)
        {
            *x= x0-length+0.3333;
            *y= y0-length+0.3333;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else
            return agg::path_cmd_stop;
    }


unsigned Marker_small_arrow_poly::vertex(double* x,double* y)
    {
        n_vertexes++;
        if( increment == 0)
        {
            *x= x0+cos(-PI*4/5+rot)*length;
            *y= y0+sin(-PI*4/5+rot)*length;
            ++increment;
            return agg::path_cmd_move_to;
        }
        else if( increment == 1)
        {
            *x= x0;
            *y= y0;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else if( increment == 2)
        {
            *x= x0+cos(PI*4/5+rot)*length;
            *y= y0+sin(PI*4/5+rot)*length;
            ++increment;
            return agg::path_cmd_line_to;
        }
        else
            return agg::path_cmd_stop;
    }

unsigned ArbPoly_solid::vertex(double* x,double* y)
    {
        if ( increment == nPoints && point[(nPoints-1)*2]==point[0] && point[(nPoints-1)*2+1]==point[1])
            return agg::path_cmd_stop;
        if ( increment == nPoints+1)
            return agg::path_cmd_stop;

        n_vertexes++;

        if ( increment == nPoints)
        {
        *x = point[0];
        *y = point[1];
        }
        else
        {
        *x = point[increment*2];
        *y = point[increment*2+1];
        }

       // cout<<increment<<" "<<*x<<" "<<*y<<"\n";

        if( increment == 0)
        {
            ++increment;
            return agg::path_cmd_move_to;
        }

        ++increment;
        return agg::path_cmd_line_to;
    }

unsigned ArcPoly_solid::vertex(double* x, double* y) {
    //cout<<increment<<"\n";
        n_vertexes++;

        if(increment>100)
        {
            std::cout<<"run away!!\n";
            std::cout<<"arc "<<arcInc<<"\n";
            std::cout<<"n arcs "<<nArcs<<"\n";
            std::cout<<"corners "<<nCorners<<"\n";
            std::cout<<"corner arc "<<cornerArc<<"\n";
            paused=true;
            return agg::path_cmd_stop;
        }
        if(nArcs==0)
        {
            if(increment==nCorners+1 || nCorners==0)
                return agg::path_cmd_stop;
            if(increment==nCorners)
            {
                *x = corner[0];
                *y = corner[1];
               // cout<<increment<<" xx "<<*x<<", "<<*y<<"\n";
                ++increment;
                return agg::path_cmd_line_to;
            }

            *x = corner[increment*2];
            *y = corner[increment*2+1];
           // cout<<increment<<" "<<*x<<", "<<*y<<"\n";
            ++increment;
            if(increment==1)
                return agg::path_cmd_move_to;
            else
                return agg::path_cmd_line_to;
        }
       if(nCorners>0 && arcInc==cornerArc && increment>=nArcPoints[arcInc] && increment<nArcPoints[arcInc]+nCorners)
       {
           *x = corner[(increment-nArcPoints[arcInc])*2];
           *y = corner[((increment-nArcPoints[arcInc])*2)+1];
          // cout<<increment<<" x "<<*x<<", "<<*y<<"\n";
           ++increment;

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
           // cout<<increment<<" "<<*x<<", "<<*y<<"\n";
            ++increment;
            return agg::path_cmd_line_to;
           // agg::path_cmd
        }

        if ( arcInc+1<nArcs && increment >= nArcPoints[arcInc] )
        {
            increment=0;
            ++arcInc;
        }


        *x = center.x + radius * cos( theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1)) );
        *y = center.y + radius * sin( theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1)) );

        double t = theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1));

        if(increment ==0 && arcInc == 0)
        {

            //cout<<increment<<" ("<<*x<<", "<<*y<<")\n";
            //cout<<nCorners<<" corners\n";
           // cout<<increment<<" * "<<*x<<", "<<*y<<"\n";
            ++increment;
            return agg::path_cmd_move_to;
        }/*
        if ( increment == 0 )
        {
          //  cout<<increment<<" "<<arcInc<<" "<<"("<<theta[arcInc*2]<<" "<<theta[arcInc*2+1]<<" "<<nArcPoints[arcInc]<<")"<<" move to "<<t<<"\n";
            ++increment;
            return agg::path_cmd_line_to;

        }*/
       //cout<<increment<<" "<<*x<<", "<<*y<<"\n";

       // cout<<increment<<" "<<arcInc<<"/"<<nArcs<<"->"<<nArcPoints[arcInc]<<" "<<" line to "<<t<<"\n";
        ++increment;
        return agg::path_cmd_line_to;
    }

unsigned ArcPoly::vertex(double* x, double* y)
{

        if ( arcInc+1 == nArcs && increment == nArcPoints[arcInc])
            return agg::path_cmd_stop;
        //else if(arcInc+1 == nArcs && increment == nArcPoints[arcInc])
        //{
        //*x = center.x + radius * cos( theta[0]);
        //*y = center.y + radius * sin( theta[0]);
        //++increment;
        //return agg::path_cmd_line_to;
        //}

        if ( arcInc+1<nArcs && increment == nArcPoints[arcInc] )
        {
            increment=0;
            ++arcInc;
        }

        n_vertexes++;

        *x = center.x + radius * cos( theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1)) );
        *y = center.y + radius * sin( theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1)) );

        //double t = theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1));

        if ( increment == 0 )
        {
           // std::cout<<"("<<theta[arcInc*2]*180/PI<<" to "<<theta[arcInc*2+1]*180/PI<<", "<<nArcPoints[arcInc]<<" points)\n"<<arcInc<<","<<increment<<" move to "<<t*180/PI<<"\n";
            ++increment;
            return agg::path_cmd_move_to;

        }

        //std::cout<<arcInc<<","<<increment<<" line to "<<t*180/PI<<"\n";
        ++increment;

        return agg::path_cmd_line_to;
    }
