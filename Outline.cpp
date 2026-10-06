#include "Outline.h"
#include "NoiseMap.h"

#include "PolyTypes.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "poly34.h"
#include "ASSERTS.h"

using namespace std;

class zmatrix
{
double* elements;

public:
    int size_m;
    int size_n;
    zmatrix(int n, int m)
    {
     elements = new double [n*m];
     size_m=m;
     size_n=n;
    }
    ~zmatrix(){delete[] elements;}
    double& operator[](int n)
    {
        //ASSERT(n<size_m*size_n);
     return elements[n];
    }
    void operator=(zmatrix& z)
    {
     if(size_m == z.size_m && size_n==z.size_n)
            memcpy(elements,z.elements,sizeof(double)*size_m*size_n);
     else std::cout<<"matrix assignment error\n";
    }
    void operator *= (double d)
    {
        for(int m=0;m<size_m;m++)
            for(int n=0;n<size_n;n++)
                elements[m*size_n+n]*=d;
    }
    void dot(zmatrix& z, zmatrix& w)
    {
    if( size_n == w.size_n && size_m == z.size_m /*&& z.size_n == w.size_m*/)
        for(int m=0;m<size_m;m++)
            for(int n=0;n<size_n;n++)
            {
                elements[m*size_n+n]=0;
                for(int i=0;i<z.size_n;i++)
                {
                    //cout<<z.elements[m*z.size_n+i]<<" x "<<w.elements[i*w.size_n+n]<<" ";
                    elements[m*size_n+n]+=z.elements[m*z.size_n+i]*w.elements[i*w.size_n+n];
                }
               // cout<<"\n";
            }
    else std::cout<<"matrix multiply error\n";
    }
    void print()
    {
     for(int m=0;m<size_m;m++)
     {
            for(int n=0;n<size_n;n++)
            {
             std::cout<<elements[m*size_n+n]<<"  ";
            }
        std::cout<<"\n";
     }
    }
    void invert()
    {
        zmatrix minor(size_n,size_m);

        if(size_m==2 && size_n==2)
        {
            double temp = elements[0];
            elements[0]=elements[3];
            elements[3]=temp;
            temp = elements[1];
            elements[1]=elements[2];
            elements[2]=temp;
            elements[1]*=-1;
            elements[2]*=-1;
            double d = calc_determinant();
            (*this)*=(1/d);
            return;
        }
        for(int m=0;m<size_m;m++)
         for(int n=0;n<size_n;n++)
            {
             zmatrix* z = new zmatrix(size_n-1,size_m-1);

             for(int mm=0;mm<size_m-1;mm++)
                for(int nn=0;nn<size_n-1;nn++)
                {
                    (*z)[mm*z->size_n+nn]=elements[(mm+(mm>=m?1:0))*size_n+nn+(nn>=n?1:0)];
                }

           // sum+=elements[n]*z->calc_determinant()*(n%2==0?1:-1);
            minor[m*size_n+n]=z->calc_determinant();
             delete z;
            }
      //  cout<<"minor matrix:\n";
      //  minor.print();

     //   minor.print();
     //   cout<<"\n";
        for(int m=0;m<size_m;m++)
         for(int n=0;n<size_n;n++)
            minor[m*size_n+n]*=(n%2==0?1:-1)*(m%2==0?1:-1);

        double det=0;
        for(int n=0;n<size_n;n++)
        {
         det+=elements[n]* minor[+n];
        }
        //cout<<"determinant: "<<det<<"\n";

     //    minor.print();
     //   cout<<"\n";

        for(int m=0;m<size_m;m++)
         for(int n=m+1;n<size_n;n++)
         {
          if(m!=n)
          {
           double temp = minor[m*size_n+n];
            minor[m*size_n+n]=minor[n*size_n+m];
            minor[n*size_n+m]=temp;
          }
         }
      //   minor.print();
     //   cout<<"\n";
         minor*=(1/det);
     //    minor.print();

         (*this)=minor;
      //  cout<<"\n";
    }
    void transpose(zmatrix& z)
    {
     if(size_m != z.size_n || size_n != z.size_m)
     {
         std::cout<<"matrix transpose error\n";
         return;
     }
         for(int m=0;m<size_m;m++)
            for(int n=0;n<size_n;n++)
            {
                elements[m*size_n+n]=z[n*z.size_n+m];
            }
    }
    double calc_determinant()
    {
        if(size_m != size_n)
        {
         std::cout<<"calc determinant error\n";
         return 0;
        }

        if(size_m==2 && size_n==2)
        {
         return elements[0]*elements[3]-elements[1]*elements[2];
        }
        else
        {
        double sum=0;
        for(int n=0;n<size_n;n++)
            {
             zmatrix* z = new zmatrix(size_n-1,size_m-1);

             for(int mm=0;mm<size_m-1;mm++)
                for(int nn=0;nn<size_n-1;nn++)
                {
                    (*z)[mm*z->size_n+nn]=elements[(mm+1)*size_n+nn+(nn>=n?1:0)];
                }
           // z->print();
           // cout<<"\n    det: "<<z->calc_determinant()<<"\n";
            sum+=elements[n]*z->calc_determinant()*(n%2==0?1:-1);
           // cout<<"sum: "<<sum<<"\n";
             delete z;
            }
        return sum;
        }
    }
};


cubic_bezier FitBezier(double* points, int nPoints, int k=4)
{
        //ASSERT(points);
      //  ASSERT(k==4);
      //  ASSERT(nPoints<15);

        cubic_bezier bez;

        //cout<<"bezier, nPoints="<<nPoints<<":\n";

        zmatrix yvec(1,nPoints);

        for(int i=0;i<nPoints;i++)
        {
            yvec[i]=points[i*2+1];
        }
        zmatrix xvec(1,nPoints);

        for(int i=0;i<nPoints;i++)
        {
            xvec[i]=points[i*2];
        }

        //zmatrix mate(k,nPoints);

        zmatrix M_matrix(4,4);
        M_matrix[0]=1; M_matrix[1]=0; M_matrix[2]=0; M_matrix[3]=0;
        M_matrix[4]=-3; M_matrix[5]=3; M_matrix[6]=0; M_matrix[7]=0;
        M_matrix[8]=3; M_matrix[9]=-6; M_matrix[10]=3; M_matrix[11]=0;
        M_matrix[12]=-1; M_matrix[13]=3; M_matrix[14]=-3; M_matrix[15]=1;

        zmatrix T_matrix(4,nPoints);
        double t_sum=0;
        for(int i=1;i<nPoints;i++)
        {
            t_sum += sqrt(   (xvec[i]-xvec[i-1])*(xvec[i]-xvec[i-1])+
                             (yvec[i]-yvec[i-1])*(yvec[i]-yvec[i-1])   );
            //t_sum+=d;
            T_matrix[4*i]=t_sum;
        }
        for(int i=1;i<nPoints-1;i++)
        {
            T_matrix[4*i]/=t_sum;
        }
        T_matrix[0]=0;
        T_matrix[4*(nPoints-1)]=1;

        for(int i=0;i<nPoints;i++)
        {
            double t=T_matrix[4*i];
            T_matrix[4*i]  =1;
            T_matrix[4*i+1]=t;
            T_matrix[4*i+2]=t*t;
            T_matrix[4*i+3]=t*t*t;
        }

        //T_matrix.print();
        zmatrix T_matrix_trans(nPoints,4);
        T_matrix_trans.transpose(T_matrix);

        zmatrix tproduct(4,4);
        tproduct.dot(T_matrix_trans,T_matrix);
        tproduct.invert();

        zmatrix M_matrix_inv(4,4);
        M_matrix_inv=M_matrix;
        M_matrix_inv.invert();

        zmatrix term1 (4,4);
        term1.dot(M_matrix_inv,tproduct);

        zmatrix term2 (nPoints,4);
        term2.dot(term1,T_matrix_trans);

        zmatrix C_x(1,4);
        C_x.dot(term2,xvec);

        zmatrix C_y(1,4);
        C_y.dot(term2,yvec);

        //cout<<"Cx:\n";
        //C_x.print();
        //cout<<"Cy:\n";
        //C_y.print();

        bez.ax=C_x[0];
        bez.bx=C_x[1];
        bez.cx=C_x[2];
        bez.dx=C_x[3];

        bez.ay=C_y[0];
        bez.by=C_y[1];
        bez.cy=C_y[2];
        bez.dy=C_y[3];

       // cout<<bez.ax<<","<<bez.ay<<" to "<<bez.cx<<","<<bez.cy<<"\n";

        return bez;
}

void Outline::shift_trace(int z)
{
    cout<<"shift_trace "<<z<<"\n";
    line_seg* temp;
    if(z<0)
    {
        temp = new line_seg[-z];
        memcpy(temp,&m_trace[n_trace+z-1],sizeof(line_seg)*(-z));
        for(int i=n_trace-1;i>=-z;i--)
        {
            m_trace[i]=m_trace[i+z];
        }
        memcpy(&m_trace[0],temp,sizeof(line_seg)*(-z));
        delete[] temp;
        m_trace[n_trace-1]=m_trace[0];
    }
    else if(z>0)
    {
        temp = new line_seg[z];
        memcpy(temp,&m_trace[0],sizeof(line_seg)*(z));
        for(int i=0;i<n_trace-z-1;i++)
        {
            m_trace[i]=m_trace[i+z];
        }
        memcpy(&m_trace[n_trace-z-1],temp,sizeof(line_seg)*(z));
        delete[] temp;
        m_trace[n_trace-1]=m_trace[0];
    }
}

#define BEZ_MAX_TRACE 14
#define BEZ_MAX_ERR 2.5
bezier_object Outline::MakeBezierObject()
    {
        bezier_object ret;
        double datapoints[BEZ_MAX_TRACE*2+2];
                    //for(int i=0;i<outline.n_trace;i++)
        int i=0;
        int bezc=0;
        if(n_trace < 9)
        {
            ret.n_bez=0;
            return ret;
        }
        //std::cout<<n_trace<<" points\n";
   //     for(int i=0;i<n_trace;i++)
   //        cout<<i<<": "<<m_trace[i].x0<<","<<m_trace[i].y0<<" -"<<((m_trace[i].flags & IS_EDGE)?"1":" ")<<"  \n";
        int endpoint=n_trace;

        if(bHasEdges)
        {
            if(!(m_trace[i].flags & IS_EDGE))
            {
            int z=0;
            for(int j=n_trace-2;j>=0;j--)
                    {
                     if( m_trace[j].flags & IS_EDGE )
                         {
                            z=j+1;
                            break;
                         }
                    }
                shift_trace(z-n_trace);
                //for(int i=0;i<n_trace;i++)
                //    cout<<i<<": "<<m_trace[i].x0<<","<<m_trace[i].y0<<" -"<<((m_trace[i].flags & IS_EDGE)?"1":" ")<<"  \n";
            }
            else
            {
            int z=0;
            for(int j=0;j<n_trace-1;j++)
                    {
                     if(!( m_trace[j].flags & IS_EDGE ))
                         {
                            z=j-1;
                            break;
                         }
                    }
                shift_trace(z);
            }
        {

        }
        }

       // for(int i=0;i<n_trace;i++)
        //   cout<<i<<": "<<m_trace[i].x0<<","<<m_trace[i].y0<<" -"<<((m_trace[i].flags & IS_EDGE)?"1":" ")<<"  \n";

        while(i<n_trace-1)
            {
            endpoint=n_trace;
            if(!(m_trace[i].flags & IS_EDGE && m_trace[i+1].flags & IS_EDGE))
            {
                //cout<<"no edge\n";
                for(int j=i+1;j<n_trace;j++)
                {
                 if( m_trace[j].flags & IS_EDGE )
                     {
                        endpoint=j+1;
                        break;
                     }
                }
                //cout<<"--curve "<<i<<"-"<<endpoint<<"\n";
                while(i<endpoint-1)
                    {

                        if(i+4>endpoint)
                        {
                            cout<<"ERROR: less than four points, i="<<i<<"\n";
                            return ret;
                        }
                       // if(i+5<outline.endpoint)
                        {
                            double d_sum=0;
                            int jj=0;
                            int inc=1;
                          //  std::cout<<i<<"\n";
//15
                            for(int j=0;j<BEZ_MAX_TRACE;j++)
                            {
                                if(m_trace[i+j].flags & X_ALIGNED && !(m_trace[i+j].flags & NO_Y_ROOT))
                                {
                                    datapoints[j*2]=m_trace[i+j].x0;
                                    datapoints[j*2+1]=(double)m_trace[i+j].y0+m_trace[i+j].y_offset;
                                }
                                else if(!(m_trace[i+j].flags & X_ALIGNED) && !(m_trace[i+j].flags & NO_X_ROOT))
                                {
                                    datapoints[j*2]=(double)m_trace[i+j].x0+m_trace[i+j].x_offset;
                                    datapoints[j*2+1]=m_trace[i+j].y0;
                                }
                                else
                                {
                                    datapoints[j*2]=m_trace[i+j].x0;
                                    datapoints[j*2+1]=m_trace[i+j].y0;
                                    cout<<i+j<<" WARNING no roots";
                                }
                            }
                            //if(ji<15)
                            {
                              //  datapoints[ji*2]=0;
                              //  datapoints[ji*2+1]=0;
                            }
                        if(i >= endpoint-15)
                        {
                            //cout<<"ohnoes: "<<i<<", "<<endpoint-i<<"\n";
                       //     datapoints[(endpoint-i)*2]=m_trace[0].x0;
                        //    datapoints[(endpoint-i)*2+1]=m_trace[0].y0;
                        }
                         cubic_bezier bez;
                         //double x1=bez.dx;
                         //double y1=bez.dy;
                         double total_err=100;
                         //jj=min(14,endpoint-i+1);
                         jj=std::min(BEZ_MAX_TRACE,min(endpoint-i,endpoint));
                         if(i+jj>endpoint-4&&i+jj<endpoint)
                         {
                          //cout<<"xx"<<i+jj<<"\n";
                          jj=endpoint-4-i;
                         }
                       //  cout<<"\n"<<i<<"\n";
                         while(total_err>BEZ_MAX_ERR && jj>3)
                         {
                         //    cout<<"try "<<jj<<" ";
                            bez = FitBezier(&datapoints[0],jj);
                            //std::cout<<"points: ";
                            //for(int gg=0;gg<jj;gg++)
                            //{
                            // std::cout<<datapoints[gg*2]<<","<<datapoints[gg*2+1]<<" ";
                            //}
                            //cout<<"\n";
                            double t_sum=0;
                            double e_sum=0;
                            double err_points[BEZ_MAX_TRACE];
                            double t_points[BEZ_MAX_TRACE];
                            for(int ii=1;ii<jj;ii++)
                                {
                                t_points[ii]= sqrt(     (datapoints[ii*2]-datapoints[(ii-1)*2])*(datapoints[ii*2]-datapoints[(ii-1)*2])+
                                                        (datapoints[ii*2+1]-datapoints[(ii-1)*2+1])*(datapoints[ii*2+1]-datapoints[(ii-1)*2+1]) );
                                t_sum+=t_points[ii];
                                t_points[ii]+=t_points[ii-1];
                                }
                                //cout<<"\n t_indexes: ";
                            for(int ii=0;ii<jj;ii++)
                                {
                                t_points[ii]/=t_sum;
                                //cout<< t_points[ii]<<" ";
                                double err_t = sqrt(     (datapoints[ii*2]-bez.test_x(t_points[ii]))*(datapoints[ii*2]-bez.test_x(t_points[ii]))+
                                                        (datapoints[ii*2+1]-bez.test_y(t_points[ii]))*(datapoints[ii*2+1]-bez.test_y(t_points[ii])) );
                                err_points[ii]=err_t;
                                e_sum+=err_t;

                                }
                            total_err = e_sum;
                            inc=jj;
                           //cout<<" "<<i<<", len="<<inc<<" ";
                           ////if(e_sum < BEZ_MAX_ERR)
                           // cout<<" error: "<<e_sum<<" ...ok\n";
                           //else
                           // cout<<" error: "<<e_sum<<"\n";

                            //cout<<

                            if(e_sum > BEZ_MAX_ERR)
                            {
                                double maxerr=0;
                                int maxerri=4;
                                if(i+jj+4>=endpoint)
                                {
                                    jj=endpoint-i-4;
                                    //cout<<"yo:"<<jj<<"\n";
                                }
                                //cout<<"err: ";
                                if(jj<5)
                                    maxerri=0;

                                    for(int ii=4;ii<jj;ii++)
                                    {
                                       // cout<<err_points[ii]<<" ";
                                        if(err_points[ii]>maxerr)
                                        {

                                            maxerr=err_points[ii];
                                            maxerri=ii;
                                        }

                                    }
                                    //cout<<"\n";
                                    jj=maxerri;


                            }

                         }//while err, jj

                        // cout<<" ...ok\n";

                         ret.m_bez[bezc] = bez;
                         //cout<<bezc<<": "<<bez.ax<<","<<bez.ay<<" - "<<bez.dx<<","<<bez.dy<<"\n";

                         bezc++;
                         ret.n_bez = bezc;

                         if(bezc==400)
                         {
                             std::cout<<"Error!! Reached maximum # curves...\n\n";
                             return ret;
                         }

                         //bez.ax=x1;
                         //bez.ay=y1;
                         //i+=jj;
                         i+=inc-1;
                         //cout<<i<<"\n";

                        }
                    }//while i < endpoint
                }
                else //IS_EDGE
                {
                 //   cout<<"--edge "<<i<<"-";
                    cubic_bezier bez;
                    bez.bEdge=true;
                    int j=i;
                    cout<<m_trace[i].x0<<","<<m_trace[i].y0<<"\n";
                 //   cout<<m_trace[i+1].x0<<","<<m_trace[i+1].y0<<"\n\n";
                    if(m_trace[i+1].x0==m_trace[i].x0)
                    {

                        while(m_trace[j+1].x0==m_trace[j].x0 && j<n_trace-1)
                        {
                       //     cout<<m_trace[j].x0<<","<<m_trace[j].y0<<"  ";
                            j++;
                        }

                    }
                    else if(m_trace[i+1].y0==m_trace[i].y0)
                    {

                        while(m_trace[j+1].y0==m_trace[j].y0 && j<n_trace-1)
                        {
                       //     cout<<m_trace[j].x0<<","<<m_trace[j].y0<<"  ";
                            j++;
                        }

                    }
                    else
                    {
                        cout<<"Error: Bad Edge!\n";
                        return ret;
                    }
                    //cout<<j<<"\n";
                    bez.ax=m_trace[i].x0;
                    bez.ay=m_trace[i].y0;
                    bez.bx=m_trace[j].x0;
                    bez.by=m_trace[j].y0;
                    bez.cx=m_trace[i].x0;
                    bez.cy=m_trace[i].y0;
                    bez.dx=m_trace[j].x0;
                    bez.dy=m_trace[j].y0;

                    i=j;
                    ret.m_bez[bezc] = bez;

                    //cout<<bezc<<"= "<<bez.ax<<","<<bez.ay<<" - "<<bez.dx<<","<<bez.dy<<"\n";

                    bezc++;
                    ret.n_bez = bezc;

                    if(bezc==400)
                    {
                     std::cout<<"Error!! Reached maximum # curves...\n\n";
                     return ret;
                    }
                }
            }
            std::cout<<"bezier object "<<i<<" points, "<<bezc<<" curves \n";

            std::cout<<"endpoint aligning...\n";
            for(int i=0;i<ret.n_bez-1;i++)
            {
                if(ret.m_bez[i].bEdge==false && ret.m_bez[i+1].bEdge==false)
                {
                ret.m_bez[i].dx = ret.m_bez[i].dx+0.5*(ret.m_bez[i].dx-ret.m_bez[i+1].ax);
                ret.m_bez[i].dy = ret.m_bez[i].dy+0.5*(ret.m_bez[i].dy-ret.m_bez[i+1].ay);
                ret.m_bez[i+1].ax = ret.m_bez[i].dx;
                ret.m_bez[i+1].ay = ret.m_bez[i].dy;
                }
                else if (ret.m_bez[i].bEdge==true && ret.m_bez[i+1].bEdge==false)
                {
                ret.m_bez[i+1].ax = ret.m_bez[i].dx;
                ret.m_bez[i+1].ay = ret.m_bez[i].dy;
                }
                else if (ret.m_bez[i].bEdge==false && ret.m_bez[i+1].bEdge==true)
                {
                ret.m_bez[i].dx = ret.m_bez[i+1].ax;
                ret.m_bez[i].dy = ret.m_bez[i+1].ay;
                }
            }


            if(ret.m_bez[ret.n_bez-1].bEdge==false && ret.m_bez[0].bEdge==false)
            {
                ret.m_bez[ret.n_bez-1].dx = ret.m_bez[ret.n_bez-1].dx+0.5*(ret.m_bez[ret.n_bez-1].dx-ret.m_bez[0].ax);
                ret.m_bez[ret.n_bez-1].dy = ret.m_bez[ret.n_bez-1].dy+0.5*(ret.m_bez[ret.n_bez-1].dy-ret.m_bez[0].ay);
                ret.m_bez[0].ax = ret.m_bez[ret.n_bez-1].dx;
                ret.m_bez[0].ay = ret.m_bez[ret.n_bez-1].dy;
            }
            else if(ret.m_bez[ret.n_bez-1].bEdge==true && ret.m_bez[0].bEdge==false)
            {
                ret.m_bez[0].ax = ret.m_bez[ret.n_bez-1].dx;
                ret.m_bez[0].ay = ret.m_bez[ret.n_bez-1].dy;
            }
            else if(ret.m_bez[ret.n_bez-1].bEdge==false && ret.m_bez[0].bEdge==true)
            {
                ret.m_bez[ret.n_bez-1].dx = ret.m_bez[0].ax;
                ret.m_bez[ret.n_bez-1].dy = ret.m_bez[0].ay;
            }

            //lines that are almost - but not quite - horizontal/vertical mess up root calculations
            /*
             for(int i=0;i<ret.n_bez-1;i++)
            {
             if(fabs(ret.m_bez[i].ax - ret.m_bez[i].dx) < 0.01)
             {
                 ret.m_bez[i].dx = ret.m_bez[i].ax;
                 ret.m_bez[i].cx = ret.m_bez[i].ax;
                 ret.m_bez[i].bx = ret.m_bez[i].ax;

                 ret.m_bez[i+1].ax = ret.m_bez[i].ax;
             }
             if(fabs(ret.m_bez[i].ay - ret.m_bez[i].dy) < 0.01)
             {
                 ret.m_bez[i].dy = ret.m_bez[i].ay;
                 ret.m_bez[i].cy = ret.m_bez[i].ay;
                 ret.m_bez[i].by = ret.m_bez[i].ay;

                 ret.m_bez[i+1].ay = ret.m_bez[i].ay;
             }
            }

            */
            //for(int i=0;i<bezc;i++)
            //    cout<<i<<": "<<ret.m_bez[i].ax<<","<<ret.m_bez[i].ay<<" - "<<ret.m_bez[i].dx<<","<<ret.m_bez[i].dy<<"\n";

            return ret;
    }


int FILL_H1=1;
int FILL_H2=0;

void Outline::fill_polygon(int x0,int y0)
{
    fill_polygon(x0,y0,1,0);
}

void Outline::fill_polygon(int x0,int y0,int h1, int h2)
    {
        steps=0;
        int x=x0;
        int y=y0;

        FILL_H1=h1;
        FILL_H2=h2;

        if(m_map[y*MapPixels+x]!=FILL_H1)
            {
            std::cout<<"invalid for fill\n";
            return;
            }

        //cout<<"filling in polygon...\n";

        while(m_map[y*MapPixels+x-1]==FILL_H1)
            x--;
        //cout<<"a\n";
        if(x<x0)
            {
            fill_line_down(x,x0,y0+1);
            fill_line_up(x,x0,y0-1);
            }
        //cout<<"b\n";
        while(m_map[y*MapPixels+x]==FILL_H1)
        {
            m_map[y*MapPixels+x]=FILL_H2;
            x++;
        }
        //cout<<"c\n";
        //cout<<y0<<"\n";
        fill_line_up(x0,x,y0-1);
        fill_line_down(x0,x,y0+1);

        //cout<<"filled "<<steps<< " rows!\n";
    }


void Outline::fill_line_up(int x0,int x1,int y0)
    {
        int x=x0;
        int temp;
        steps++;
        //if(steps>500) return;

        //cout<<"up "<< y0<<"  "<<x0<<","<<x1<<"\n";

        while(m_map[y0*MapPixels+x-1]==FILL_H1)
            x--;

        if(x<x0)
            fill_line_down(x,x0,y0+1);

        while(x<=x1)
        {
            if(m_map[y0*MapPixels+x]==FILL_H1)
                {
                    temp=x;
                    m_map[y0*MapPixels+x]=FILL_H2;
                    while(m_map[y0*MapPixels+x+1]==FILL_H1)
                    {
                        m_map[y0*MapPixels+x+1]=FILL_H2;
                        x++;
                    }
                    fill_line_up(temp,x,y0-1);
                }

            if(x<=x1)
                x++;
        }
        if(x-1>x1)
            fill_line_down(x1,x-1,y0+1);
    }

void Outline::fill_line_down(int x0,int x1,int y0)
    {
        int x=x0;
        int temp;

        steps++;
        //if(steps>500) return;

        //cout<<"down "<< y0<<" ";

        while(m_map[y0*MapPixels+x-1]==FILL_H1)
            x--;

        if(x<x0)
            fill_line_up(x,x0,y0-1);

        while(x<=x1)
        {
            if(m_map[y0*MapPixels+x]==FILL_H1)
                {
                    temp=x;
                    m_map[y0*MapPixels+x]=FILL_H2;
                    while(m_map[y0*MapPixels+x+1]==FILL_H1)
                    {
                        m_map[y0*MapPixels+x+1]=FILL_H2;
                        x++;
                    }
                    fill_line_down(temp,x,y0+1);
                }

            if(x<=x1)
                x++;
        }
        if(x-1>x1)
            fill_line_up(x1,x-1,y0-1);
    }

int Outline::cubic_intersect(double* res ,double* points,int nPoints)
    {
        polynomial3 poly=FitPoly(&points[0],nPoints);

        //cout<<"  "<<poly.a<<" "<<poly.b<<" "<<poly.c<<" "<<poly.d<<"\n";
        double a,b,c,d;

        a = poly.a;//poly.d;
        b = poly.b;//poly.d;
        c = poly.c;//poly.d;
        d = poly.d;

        //cout<<"  "<<a<<" "<<b<<" "<<c<<" "<<"\n";

        double x=0;
        double y= x*x*x + c*x*x + b*x + a;

        double roots[4];
        int nRoots=0;

        if(d>0.0001 || d<-0.0001)
        {
           // cout<<"P3!\n";
            nRoots = SolveP3(&roots[0],c/d,b/d,a/d);

        }
          else
          {
                if (c<0.0001 && c> -0.0001)
                    {
                    if (b<0.0001 && b>-0.0001)
                        {
                        //no solutions
                        nRoots= 0;
                        }
                    //linear solution
                     roots[0] = -a / b;
                     nRoots= 1;
                    }
                else
                    {
                        // quadratic solution
                        double q = sqrt(b*b - 4*a*c), a2 = 2*c;
                        roots[0] = (q-b)/a2;
                        roots[1] = (-b-q)/a2;
                        nRoots= 2;
                    }
          }
         if(nRoots==1)
         {
            *res=roots[0];
            return 1;
         }
         else
         {
             x=5;
             int n=0;
             for(int q=0;q<nRoots;q++)
                {
                    x = fabs(roots[q])<fabs(x)?roots[q]:x;
                    if(x>-3 && x <3)
                    {
                        *res=x;
                        n=1;
                    }
                }
            return n;
         }
    return 0;
    }

#define X_STEPS 4
#define Y_STEPS 4

void Outline::trace(int x0,int y0)
    {
        n_trace = 0;
        cout<<"tracing... "<<x0<<","<<y0<<"\n";

        if(m_map[(y0-1)*MapPixels+x0-1]==0 && m_map[(y0-1)*MapPixels+x0]==0 &&
           m_map[(y0-1)*MapPixels+x0+1]==0 && m_map[(y0)*MapPixels+x0-1]==0 &&
           m_map[(y0)*MapPixels+x0+1]==0 && m_map[(y0+1)*MapPixels+x0-1]==0 &&
           m_map[(y0+1)*MapPixels+x0]==0 && m_map[(y0+1)*MapPixels+x0+1]==0 )
        {
         cout<<"...isolated single square\n";
         m_map[y0*MapPixels+x0]=0;
         return;
        }

        open0(x0,y0);

        cout<<"n_trace= "<<n_trace<<"\n";

        int xx,yy,xx1,yy1;
        xx=m_trace[0].x0;
        xx1=m_trace[0].x0;
        yy=m_trace[0].y0;
        yy1=m_trace[0].y0;
        for(int i=1;i<=n_trace;i++)
        {
         xx = min(xx,  m_trace[i].x0);
         xx1 = max(xx1,  m_trace[i].x0);
         yy = min(yy,  m_trace[i].y0);
         yy1 = max(yy1,  m_trace[i].y0);
        }
        trace_area=(xx1-xx)*(yy1-yy);

        m_trace[n_trace]=m_trace[1];

         for(int i=0;i<=max_trace;i++)
         {
          m_trace[i].x_offset=0;
          m_trace[i].y_offset=0;
          m_trace[i].flags=0;
         }

        //  for(int i=0;i<n_trace;i++)
        //    cout<<m_trace[i].x0<<","<<m_trace[i].y0<<"  ";
        //cout<<"\n";

        postprocess();

        if(n_trace>=max_trace-1) {cout<<"Polyline limit reached ("<<max_trace<<") \n";}

        fill_polygon(x0,y0);

        //int height=160;


        if(false)
        for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                if((unsigned char)m_heightmap->m_map[j*MapPixels+i] == m_height)
                {
                    m_map[j*MapPixels+i]=2;
                }
                 else
                {
                     m_map[j*MapPixels+i]=0;
                }
            }
        for(int i=0;i<n_trace;i++)
        {
            bool bPrint = false;
            {
                double points[(X_STEPS*2+1)*2];
                //if(false)
                double h = m_heightmap->height_fmap(m_height);
                if(m_trace[i].y0 >= X_STEPS && m_trace[i].y0 <  MapPixels-X_STEPS && m_trace[i].x0 >= X_STEPS && m_trace[i].x0 <  MapPixels-X_STEPS)
                {
                    for(int j=0;j<X_STEPS*2+1;j++)
                    {
                        points[j*2+1]=(double)m_heightmap->m_fmap[m_trace[i].y0* MapPixels+m_trace[i].x0-X_STEPS+j]-h;
                        points[j*2]=(double)j-X_STEPS;
                    }
                    //polynomial3 poly;
                    if(bPrint)
                    {
                        cout<<i<<" h="<<h<<"\n";
                        for(int ii=0;ii<X_STEPS*2+1;ii++)
                        {
                            cout<<points[ii*2+1]<<" ";
                        }
                    }

                    double res;
                    int got_res = cubic_intersect(&res,&points[0],X_STEPS*2+1);

                    if(got_res > 0)
                    {
                        m_trace[i].x_offset=res;
                        if(bPrint)
                        cout<<"... "<<res<<"";

                    }
                    else
                    {
                        if(bPrint)
                        cout<<"no x root ";
                        m_trace[i].flags |= NO_X_ROOT;
                      //  m_trace[i].flags |= X_ALIGNED;
                    }
                    if(bPrint)
                    cout<<"\n";
                }

                if(m_trace[i].y0 >= Y_STEPS && m_trace[i].y0 <  MapPixels-Y_STEPS && m_trace[i].x0 >= Y_STEPS && m_trace[i].x0 <  MapPixels-Y_STEPS)
                {
                    for(int j=0;j<Y_STEPS*2+1;j++)
                    {
                        points[j*2+1]=(double)m_heightmap->m_fmap[(m_trace[i].y0-Y_STEPS+j)* MapPixels+m_trace[i].x0]-h;
                        points[j*2]=(double)j-Y_STEPS;
                    }

                    for(int ii=0;ii<Y_STEPS*2+1;ii++)
                    {
                      //  cout<<points[ii*2+1]<<" ";
                    }


                    double res;
                    int got_res = cubic_intersect(&res,&points[0],Y_STEPS*2+1);

                    if(got_res >0)
                    {
                    //  cout<<"... "<<res<<"";
                        m_trace[i].y_offset=res;
                    }
                    else
                    {
                    //    cout<<"no y root ";
                        m_trace[i].flags |= NO_Y_ROOT;
                    }
                  //  cout<<"\n";
                }
                //cout<<i<<" "<<m_trace[i].x0<<","<<m_trace[i].y0<<" "<<m_trace[i].x_offset<<"/"<<m_trace[i].y_offset<<"\n";
                m_map[m_trace[i].y0*MapPixels+m_trace[i].x0]=6;
            }
        }//for i < n_trace

        for(int i=0;i<n_trace-1;i++)
            {

            //double s = (double)(m_trace[i+1].y0-m_trace[i].y0)
            //                    /((m_trace[i+1].x0+m_trace[i+1].x_offset)-(m_trace[i].x0+m_trace[i].x_offset));
            //double s = (((double)m_trace[i+1].y0+m_trace[i+1].y_offset)-(m_trace[i].y0+m_trace[i].y_offset))
            //                    /(m_trace[i+1].x0-m_trace[i].x0);
            //cout<<s<<"\n";


            if((fabs(m_trace[i].y_offset) < fabs(m_trace[i].x_offset) || m_trace[i].flags & NO_X_ROOT) && !(m_trace[i].flags & NO_Y_ROOT) )
                {
                    m_trace[i].flags |= X_ALIGNED;
                }
            }
        for(int i=0;i<n_trace;i++)
            {
                if(m_trace[i].x0<X_STEPS+2 || m_trace[i].x0>  MapPixels-X_STEPS-2)
                {
                    m_trace[i].flags |= X_ALIGNED;
                    m_trace[i].flags != NO_X_ROOT;
                }
                if(m_trace[i].y0<X_STEPS+2 || m_trace[i].y0> MapPixels-X_STEPS-2)
                {
                    m_trace[i].flags &= ~X_ALIGNED;
                    m_trace[i].flags != NO_Y_ROOT;
                }

                 if(m_trace[i].x0==2 || m_trace[i].x0==MapPixels-3)
                {
                    m_trace[i].flags |= IS_EDGE;
                    bHasEdges=true;
                }
                if(m_trace[i].y0==2 || m_trace[i].y0== MapPixels-3)
                {
                    m_trace[i].flags |= IS_EDGE;
                    bHasEdges=true;
                }
            }
    }

void Outline::postprocess()
    {
        line_seg trace_2[1200];
        int n=0;
        for(int i=0;i<n_trace;i++)
        {
            //cout<<i<<"  "<<m_trace[i].x0<<" , "<< m_trace[i].y0<<"\n";
        }
        //cout<<"postprocess:\n";
        int c=0;
        int cc=0;
        bool r=false;
        for(int i=0;i<n_trace;i++)
        {
            trace_2[n] = m_trace[i];
           // cout<<n<<"  "<<trace_2[n].x0<<" , "<< trace_2[n].y0<<"\n";
           //cout<<i<<"  "<<m_trace[i].x0<<" , "<< m_trace[i].y0<<"\n";
            n++;
            double d = sqrt((m_trace[i].x0-m_trace[i+1].x0)*(m_trace[i].x0-m_trace[i+1].x0)+
                     (m_trace[i].y0-m_trace[i+1].y0)*(m_trace[i].y0-m_trace[i+1].y0));

            double d2 = sqrt((m_trace[i].x0-m_trace[i+2].x0)*(m_trace[i].x0-m_trace[i+2].x0)+
                     (m_trace[i].y0-m_trace[i+2].y0)*(m_trace[i].y0-m_trace[i+2].y0));

            //cout<<i<<" "<<d<<"\n";
            if(d > 4 && i < n_trace-1)
            {
                int slices = d / 4;
                for(int j=1;j<slices+1;j++)
                {
                trace_2[n].x0=m_trace[i].x0+(int)(((double)m_trace[i+1].x0-m_trace[i].x0)*j/(slices+1));
                trace_2[n].y0=m_trace[i].y0+(int)(((double)m_trace[i+1].y0-m_trace[i].y0)*j/(slices+1));
                //  cout<< "  "<<m_trace[i].x0+(int)((double)m_trace[i+1].x0-m_trace[i].x0)*j/(slices+1) <<",";
                //  cout<< m_trace[i].y0+(int)((double)m_trace[i+1].y0-m_trace[i].y0)*j/(slices+1)<<"\n";
                //  cout<<n<<"---"<<trace_2[n].x0<<" , "<< trace_2[n].y0<<"\n";
                n++;
                c++;
                trace_2[n].flags |= IS_INSERTION;
                }
                r=false;
            }
            else if(d<1.5 && d2 < 4 && !r && i<n_trace-1)
            {
                cc++;
                n--;
                r=true;
            }
            else r=false;
        }//for int i=0;i<n_trace

        if(n>=max_trace) cout<<"ERROR! max trace reached (postp)\n";

        n_trace=n;
        for(int i=0;i<n_trace;i++)
        {
            m_trace[i] = trace_2[i];

            //if(m_trace[i].x0<6 || m_trace[i].x0> 506)
            if(m_trace[i].x0<X_STEPS+2 || m_trace[i].x0>  MapPixels-X_STEPS-2)
            {
                m_trace[i].x_offset=0;
            }
            //if(m_trace[i].y0<6 || m_trace[i].y0> 506)
            if(m_trace[i].y0<X_STEPS+2 || m_trace[i].y0> MapPixels-X_STEPS-2)
            {
                m_trace[i].y_offset=0;
            }
        }
        cout<<"added "<<c<<" points\n";
        cout<<"removed "<<cc<<" points\n";
        cout<<"total points = "<<n<<"\n";
    }

void Outline::open0(int x0,int y0)
    {
        int x=x0;
        int y=y0;
        if(n_trace>=max_trace-1)return;

        m_trace[n_trace].x0=x;
        m_trace[n_trace].y0=y;

        if(m_trace[1].x0==x && m_trace[1].y0==y && n_trace > 3)
        {
            return;
        }
        n_trace++;

     //   std::cout<<n_trace<<"\n";
        //" "<<x<<","<<y<<" ";
        while (m_map[y*MapPixels+x-1]==1 && m_map[(y-1)*MapPixels+x-1]==0)
        {
       //     cout<<"*";
            x--;
        }
       // cout<<"\n";
        if(m_map[(y-1)*MapPixels+x-1]==1 && m_map[(y-2)*MapPixels+x-1]==0)
        {
            open0(x-1,y-1);
        }
        else if(m_map[(y+1)*MapPixels+x-1]==1 && m_map[y*MapPixels+x-1]==0)
        {
            open0(x-1,y+1);
        }
        else if(m_map[(y-1)*MapPixels+x-1]==1 && m_map[(y-2)*MapPixels+x-1]==1)
        {
         //   cout<<"Switch 1\n";
            open1(x-1,y-1);
        }
        else if(m_map[(y+1)*MapPixels+x-1]==0 && m_map[(y)*MapPixels+x-1]==0)
        {
         //   cout<<"Switch 3\n";
            if(x==x0 && y==y0)
                n_trace--;
            open3(x,y);
        }
        //else cout<<"Stop!\n";
    }
void Outline::open1(int x0,int y0)
    {
        int x=x0;
        int y=y0;

        if(n_trace>=max_trace-1)return;

        m_trace[n_trace].x0=x;
        m_trace[n_trace].y0=y;

        if(m_trace[1].x0==x && m_trace[1].y0==y && n_trace > 3)
        {
            return;
        }
        n_trace++;

       // std::cout<<n_trace<<"\n";
        //cout<<n_trace<<" "<<x<<","<<y<<" ";
        while (m_map[(y-1)*MapPixels+x]==1 && m_map[(y-1)*MapPixels+x+1]==0)
        {
        //    cout<<"*";
            y--;
        }
        //cout<<"\n";
        if(m_map[(y-1)*MapPixels+x-1]==1 && m_map[(y-1)*MapPixels+x]==0)
        {
            open1(x-1,y-1);
        }
        else if(m_map[(y-1)*MapPixels+x+1]==1 && m_map[(y-1)*MapPixels+x+2]==0)
        {
            open1(x+1,y-1);
        }
        else if(m_map[(y-1)*MapPixels+x-1]==0 && m_map[(y-1)*MapPixels+x]==0)
        {
           // cout<<"Switch 0\n";
            if(x==x0 && y==y0)
                n_trace--;
            open0(x,y);
        }
         else if(m_map[(y-1)*MapPixels+x]==1 && m_map[(y-1)*MapPixels+x+1]==1)
        {
           // cout<<"Switch 2\n";
            open2(x+1,y-1);
        }
       // else cout<<"Stop!\n";
    }
void Outline::open3(int x0,int y0)
    {
        int x=x0;
        int y=y0;

        if(n_trace>=max_trace-1)return;

        m_trace[n_trace].x0=x;
        m_trace[n_trace].y0=y;

        if(m_trace[1].x0==x && m_trace[1].y0==y && n_trace > 3)
        {
            return;
        }
        n_trace++;

      // std::cout<<n_trace<<"\n";
       // cout<<n_trace<<" "<<x<<","<<y<<" ";
        while (m_map[(y+1)*MapPixels+x]==1 && m_map[(y+1)*MapPixels+x-1]==0)
        {
        //    cout<<"*";
            y++;
        }
        //cout<<"\n";
        if(m_map[(y+1)*MapPixels+x-1]==1 && m_map[(y+1)*MapPixels+x-2]==0)
        {
            open3(x-1,y+1);
        }
        else if(m_map[(y+1)*MapPixels+x+1]==1 && m_map[(y+1)*MapPixels+x]==0)
        {
            open3(x+1,y+1);
        }
        else if(m_map[(y+1)*MapPixels+x+1]==0 && m_map[(y+1)*MapPixels+x]==0)
        {
            //cout<<"Switch 2\n";
            if(x==x0 && y==y0)
                n_trace--;
            open2(x,y);
        }
         else if(m_map[(y+1)*MapPixels+x-2]==1 && m_map[(y+1)*MapPixels+x-1]==1)
        {
            //cout<<"Switch 0\n";
            open0(x-1,y+1);
        }
       // else cout<<"Stop!\n";
    }
void Outline::open2(int x0,int y0)
    {
        int x=x0;
        int y=y0;

        if(n_trace>=max_trace-1)return;

        m_trace[n_trace].x0=x;
        m_trace[n_trace].y0=y;

        if(m_trace[1].x0==x && m_trace[1].y0==y && n_trace > 3)
        {
            return;
        }
        n_trace++;

       // std::cout<<n_trace<<"\n";
       // cout<<n_trace<<" "<<x<<","<<y<<"";
        while (m_map[y*MapPixels+x+1]==1 && m_map[(y+1)*MapPixels+x+1]==0)
        {
       //     cout<<"*";
            x++;
        }
        //cout<<"\n";
        if(m_map[(y-1)*MapPixels+x+1]==1 && m_map[(y)*MapPixels+x+1]==0)
        {
            open2(x+1,y-1);
        }
        else if(m_map[(y+1)*MapPixels+x+1]==1 && m_map[(y+2)*MapPixels+x+1]==0)
        {
            open2(x+1,y+1);
        }
        else if(m_map[(y+1)*MapPixels+x+1]==1 && m_map[(y+2)*MapPixels+x+1]==1)
        {
           // std::cout<<"Switch 3\n";
            open3(x+1,y+1);
        }
        else if(m_map[(y-1)*MapPixels+x+1]==0 && m_map[y*MapPixels+x+1]==0)
        {
           // std::cout<<"Switch 1\n";
            if(x==x0 && y==y0)
                n_trace--;
            open1(x,y);
        }
        //else cout<<"Stop!\n";
    }
void Outline::init(NoiseMap& hMap,unsigned char height,unsigned char base_height)
    {
        if(m_map)
            delete[] m_map;
        m_map = new char[MapPixels*MapPixels];

        m_heightmap = &hMap;
        m_height = height;
        n_trace=0;

         for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                if((unsigned char)hMap.m_map[j*MapPixels+i] >= base_height)
                {
                    m_map[j*MapPixels+i]=1;
                }
                else
                {
                     m_map[j*MapPixels+i]=0;
                }
            }

        for(int i=0;i<MapPixels;i++)
        {
             m_map[i]=0;
             m_map[(MapPixels-1)*MapPixels+i]=0;
             m_map[i*MapPixels]=0;
             m_map[i*MapPixels+(MapPixels-1)]=0;
/*
             m_map[MapPixels+i]=0;
             m_map[(MapPixels-2)*MapPixels+i]=0;
             m_map[i*MapPixels+1]=0;
             m_map[i*MapPixels+(MapPixels-2)]=0;
            */
        }

         for(int i=1;i<MapPixels-1;i++)
         {
            if( m_map[MapPixels+i] == 1)
                    fill_polygon(i,1);
            if( m_map[(MapPixels-2)*MapPixels+i] == 1)
                fill_polygon(i,MapPixels-2);

            if( m_map[i*MapPixels+1] == 1)
                fill_polygon(1,i);
            if( m_map[i*MapPixels+MapPixels-2] == 1)
                fill_polygon(MapPixels-2,i);
         }

         int z0 = MapPixels/4;
         int z1 = MapPixels-z0;

         for(int j=z0;j<z1;j++)
            for(int i=z0;i<z1;i++)
                if(m_map[j*MapPixels+i]==1)
                    fill_polygon(i,j,1,2);
/*
         for(int j=1;j<MapPixels-1;j++)
            for(int i=1;i<MapPixels-1;i++)
                if(m_map[j*MapPixels+i]==1)
                    fill_polygon(i,j,1,0);

        for(int j=z0;j<z1;j++)
            for(int i=z0;i<z1;i++)
                if(m_map[j*MapPixels+i]==2)
                    fill_polygon(i,j,2,1);
*/
        for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                if((unsigned char)hMap.m_map[j*MapPixels+i] >= height && m_map[j*MapPixels+i]==2)
                {
                    m_map[j*MapPixels+i]=1;
                }
                else
                {
                     m_map[j*MapPixels+i]=0;
                }
            }

    }

void Outline::finalize_map(NoiseMap* hMap,unsigned char height)
{
    ASSERT(hMap);

    char* t_map = new char[MapPixels*MapPixels];


   for(int j=0;j<MapPixels;j++)
        for(int i=0;i<MapPixels;i++)
        {
            if((unsigned char)hMap->m_map[j*MapPixels+i] >= height && m_map[j*MapPixels+i]==1)
            {
                t_map[j*MapPixels+i]=hMap->m_map[j*MapPixels+i];
            }
            else
            {
                t_map[j*MapPixels+i]=0;
            }
        }

    for(int h=height;h>0;h--)
        for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                if(t_map[j*MapPixels+i]==0 && hMap->m_map[j*MapPixels+i]<h && (  t_map[(j-1)*MapPixels+i+1] > 0 ||
                                                                                t_map[(j)*MapPixels+i+1] > 0 ||
                                                                                t_map[(j+1)*MapPixels+i+1] > 0 ||
                                                                                t_map[(j-1)*MapPixels+i-1] > 0 ||
                                                                                t_map[(j)*MapPixels+i-1] > 0 ||
                                                                                t_map[(j+1)*MapPixels+i-1] > 0 ||
                                                                                t_map[(j-1)*MapPixels+i] > 0 ||
                                                                                t_map[(j+1)*MapPixels+i] > 0
                                                                               ))
                {
                    t_map[j*MapPixels+i]=hMap->m_map[j*MapPixels+i];
                }
            }
}

polynomial3 FitPoly(double* points, int nPoints, int k)
{
        ASSERT(points);
        ASSERT(k==4);
        //ASSERT(nPoints<19);


        zmatrix mate(k,nPoints);
        for(int i=0;i<nPoints;i++)
        {
         mate[i*mate.size_n]=1;
         mate[i*mate.size_n+1]=points[i*2];
         mate[i*mate.size_n+2]=points[i*2]*points[i*2];
         mate[i*mate.size_n+3]=points[i*2]*points[i*2]*points[i*2];
        }
    //    mate.print();
        zmatrix trans(nPoints,k);
        trans.transpose(mate);

    //    trans.print();
    //    cout<<"\n";

        zmatrix product(k,k);
        product.dot(trans,mate);
    //    product.print();
     //   cout<<"\n";

        product.invert();
    //    product.print();
     //   cout<<"\n";

        zmatrix product2(nPoints,k);
        product2.dot(product,trans);

    //    product2.print();
    //    cout<<"\n";

        zmatrix yvec(1,nPoints);
        for(int i=0;i<nPoints;i++)
        {
            yvec[i]=points[i*2+1];
        }
      //  yvec.print();

        zmatrix product3(1,k);
        product3.dot(product2,yvec);
       // cout<<"fitted polynomial:\n";
       // product3*=;
       // product3.print();
       // cout<<"\n\n";
        polynomial3 poly;
        poly.a=product3[0];
        poly.b=product3[1];
        poly.c=product3[2];
        poly.d=product3[3];
/*
        poly.err=0;
        for(int i=0;i<nPoints;i++)
        {
            poly.err+=(points[i*2+1]-poly.test(points[i*2]))*
                    (points[i*2+1]-poly.test(points[i*2]));
        }
*/
        return poly;
}
