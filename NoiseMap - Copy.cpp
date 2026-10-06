#include "NoiseMap.h"
#include "PolyTypes.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

float XVector::length()
{
    return sqrt(X*X + Y*Y);
}

float dotProduct(XVector v0, XVector v1)
{
    return v0.X*v1.X + v0.Y*v1.Y;
}

XVector dist(float x1,float y1, float x0, float y0)
{
    return XVector(x1-x0,y1-y0);
}


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

void NoiseMap::initGrid()
    {
        if(m_Vectors)
            delete[] m_Vectors;
        m_Vectors = new XVector[(GridIntegrals+1)*(GridIntegrals+1)];
        for(int j=0;j<GridIntegrals+1;j++)
            for(int i=0;i<GridIntegrals+1;i++)
            {
                int r = rand();
                char tr = char(r);
                m_Vectors[j*(GridIntegrals+1)+i].X = cos(PI * 2 * tr / 255);
                m_Vectors[j*(GridIntegrals+1)+i].Y = sin(PI * 2 * tr / 255);
                m_Vectors[j*(GridIntegrals+1)+i].scale(1);
            }
    }
void NoiseMap::init(float seed)
    {
        bool bAutoCorrect = true;
        if(GridIntegrals * GridResolution != MapPixels)
        {
            std::cout<<"ERROR: Invalid Noise Map Parameters!\n\n";
            return;
        }
        srand(seed);

        initGrid();

        if( m_map != NULL)
            delete[] m_map;
        if( m_fmap != NULL)
            delete[] m_fmap;

        m_map = new char[MapPixels*MapPixels];
        m_fmap = new float[MapPixels*MapPixels];

        int x,y,xx,yy;
        float z;
        int cc=0;
        XVector d00,d10,d11,d01;
        float amin = 100;
        float amax = -100;
        for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                x = i / GridResolution;
                y = j / GridResolution;
                xx = i % GridResolution;
                yy = j % GridResolution;
                d00 = XVector((float)xx/GridResolution,(float)yy/GridResolution);    //clockwise
                d10 = dist((float)xx/GridResolution,(float)yy/GridResolution,1,0);
                d11 = dist((float)xx/GridResolution,(float)yy/GridResolution,1,1);
                d01 = dist((float)xx/GridResolution,(float)yy/GridResolution,0,1);

                float u = fade((float)xx / GridResolution);
                float v = fade((float)yy / GridResolution);

                float a = lerp(u,dotProduct(d00,m_Vectors[y*(GridIntegrals+1)+x]),dotProduct(d10,m_Vectors[(y)*(GridIntegrals+1)+x+1]));
                float b = lerp(u,dotProduct(d01,m_Vectors[(y+1)*(GridIntegrals+1)+x]),dotProduct(d11,m_Vectors[(y+1)*(GridIntegrals+1)+x+1]));
                float z = lerp(v,a,b);

               // m_map[i][j]=(char)(int)(((z*0.75)-0.47)*0.90*255);
               // m_map[j*MapPixels+i]=(char)(int)((5.6+(z*0.65))*255);
                amax = std::max(amax,z);
                amin = std::min(amin,z);
                //m_map[j*MapPixels+i]=(char)(int)(((z))*247)-130;
                m_fmap[j*MapPixels+i]=z;
            }

            rescale();
            /*
            cout<<"seed: "<<seed<<"\n";
            cout<<"    float range: "<<amin<<" to "<<amax<<"\n";
            cout<<"    implied range: "<<(int)(((amin))*255)<<" to "<<(int)(((amax))*255)<<"\n";
            float sfactor = amax-amin+0.03;
            amin-=0.03;
            cout<<"    scale factor: "<<sfactor<<"\n";
            cout<<"    new range: "<< (amin-amin)/sfactor<<" to "<<(amax-amin)/sfactor<<"\n";
            cout<<"    implied range: "<<(int)(((amin-amin)/sfactor)*254)<<" to "<<(int)(((amax-amin)/sfactor)*254)<<"\n";
            int ercount = 0;
            for(int j=0;j<MapPixels;j++)
                for(int i=0;i<MapPixels;i++)
                {
                m_map[j*MapPixels+i]=(char)floor(((m_fmap[j*MapPixels+i]-amin)/sfactor)*254);
                    if(m_map[j*MapPixels+i]==0)
                    {
                        //cout<<j<<" "<<i<<" "<<(m_fmap[j*MapPixels+i])<<"\n";
                        ercount+=1;
                    }
                }
            if(ercount!=0)
                cout<<"warning: "<<ercount<<" out of range pixels\n";
*/
            //postprocess();
    }

void NoiseMap::addNoise(NoiseMap &Map2,float scale,float offset)
    {
        //cout<<"Noise Map: Adding Noise...\n";

        for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                m_fmap[j*MapPixels+i]+=(Map2.m_fmap[j*MapPixels+i]+offset)*scale;
            }
        rescale();
    }

void NoiseMap::rescale()
    {
        //cout<<"Noise Map: Rescaling...\n";
        float amin = 100;
        float amax = -100;
         for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                amax = std::max(amax,m_fmap[j*MapPixels+i]);
                amin = std::min(amin,m_fmap[j*MapPixels+i]);
            }
            //cout<<"    float range: "<<amin<<" to "<<amax<<"\n";
            //cout<<"    implied range: "<<(int)(((amin))*255)<<" to "<<(int)(((amax))*255)<<"\n";
            float sfactor = amax-amin+0.03;
            amin-=0.03;
            //cout<<"    scale factor: "<<sfactor<<"\n";
            //cout<<"    new range: "<< (amin-amin)/sfactor<<" to "<<(amax-amin)/sfactor<<"\n";
            //cout<<"    implied range: "<<(int)(((amin-amin)/sfactor)*254)<<" to "<<(int)(((amax-amin)/sfactor)*254)<<"\n";
            int ercount = 0;
            for(int j=0;j<MapPixels;j++)
                for(int i=0;i<MapPixels;i++)
                {
                m_map[j*MapPixels+i]=(char)floor(((m_fmap[j*MapPixels+i]-amin)/sfactor)*254);
                    if(m_map[j*MapPixels+i]==0)
                    {
                        //cout<<j<<" "<<i<<" "<<(m_fmap[j*MapPixels+i])<<"\n";
                        ercount+=1;
                    }
                }
            if(ercount!=0)
                std::cout<<"warning: "<<ercount<<" out of range pixels\n";
    }

void NoiseMap::postprocess()
    {
        int bins[255];
        for(int i=0;i<255;i++)
            bins[i]=0;

        int c=0;
        int cc=0;
        int z;
        int steps=12;
        int floor = 160;
        int stepsize=(256-floor)/steps;
        for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                z=(int)(unsigned char)m_map[j*MapPixels+i];
                if(z<=floor)
                        m_map[j*MapPixels+i]=0;
                else
                {
                    //bins[z]+=1;
                    for(int k=0;k<steps;k++)
                    {
                        if(z-floor>k*stepsize && z-floor<=(k+1)*stepsize)
                        {
                            m_map[j*MapPixels+i]=(char)((k+4)*256/(steps+4))+1;
                            //cout<<(int)(unsigned char)m_map[j*MapPixels+i]<<"\n";
                           // if(k==0)
                            //    bins[z]+=1;
                        }
                    }

                }/*
                else
                {

                    cout<<"Value out of range\n";
                    cout<<m_fmap[j*MapPixels+i]<<"\n";
                    cout<<z<<"\n";
                    c++;
                }*/
            }
        for(int i=0;i<255;i++)
        {
            if(bins[i]>0)
                std::cout<<i<<": "<<bins[i]<<"\n";
            cc+=bins[i];
        }
        if(c>0)
            std::cout<<"WARNING: bad pixels: "<<c<<"/"<<cc<<"\n";
    }




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
bezier_object Outline::MakeBezierObject()
    {
        bezier_object ret;
        double datapoints[30];
                    //for(int i=0;i<outline.n_trace;i++)
        int i=0;
        int bezc=0;
        if(n_trace < 9)
        {
            ret.n_bez=0;
            return ret;
        }
        //std::cout<<n_trace<<" points\n";
        while(i<n_trace)
                    {
                       // if(i+5<outline.n_trace)
                        {
                            double d_sum=0;
                            int jj=0;
                            int inc=1;
                            //std::cout<<i<<"\n";

                            for(int j=0;j<15;j++)
                            {
                                datapoints[j*2]=m_trace[i+j].x0;
                                datapoints[j*2+1]=m_trace[i+j].y0;
                            }
                            //if(ji<15)
                            {
                              //  datapoints[ji*2]=0;
                              //  datapoints[ji*2+1]=0;
                            }
                        if(i >= n_trace-15)
                        {
                            //cout<<"ohnoes: "<<i<<", "<<n_trace-i<<"\n";
                            datapoints[(n_trace-i)*2]=m_trace[0].x0;
                            datapoints[(n_trace-i)*2+1]=m_trace[0].y0;
                        }
                         cubic_bezier bez;
                         //double x1=bez.dx;
                         //double y1=bez.dy;
                         double total_err=100;
                         //jj=min(14,n_trace-i+1);
                         jj=std::min(14,n_trace-i+1);
                         if(i+jj>n_trace-4&&i+jj<=n_trace)
                         {
                         // cout<<"xx"<<i+jj<<"\n";
                          jj=n_trace-4-i;
                         }
                         while(total_err>5 && jj>3)
                         {
                            bez = FitBezier(&datapoints[0],jj);
                           // std::cout<<"points: ";
                            for(int gg=0;gg<jj;gg++)
                            {
                             //std::cout<<datapoints[gg*2]<<","<<datapoints[gg*2+1]<<" ";
                            }
                           // cout<<"\n";
                            double t_sum=0;
                            double e_sum=0;
                            double err_points[30];
                            double t_points[30];
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
                           // cout<<" error: "<<e_sum;

                            //cout<<

                            if(e_sum > 5)
                            {
                                double maxerr=0;
                                int maxerri=4;
                                if(i+jj+4>=n_trace)
                                {
                                    jj=n_trace-i-4;
                                }
                                for(int ii=4;ii<jj;ii++)
                                {
                                    if(err_points[ii]>maxerr)
                                    {

                                        maxerr=err_points[ii];
                                        maxerri=ii;
                                    }

                                }
                                jj=maxerri;
                               // if(jj<4) jj=4;
                            }

                         }
                       //  cout<<" ...done\n";

                         ret.m_bez[bezc] = bez;

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
                    }
                    std::cout<<"bezier object "<<i<<" points, "<<bezc<<" curves \n";

                    std::cout<<"endpoint aligning...\n";
                    for(int i=0;i<ret.n_bez-1;i++)
                    {
                        ret.m_bez[i].dx = ret.m_bez[i].dx+0.5*(ret.m_bez[i].dx-ret.m_bez[i+1].ax);
                        ret.m_bez[i].dy = ret.m_bez[i].dy+0.5*(ret.m_bez[i].dy-ret.m_bez[i+1].ay);
                        ret.m_bez[i+1].ax = ret.m_bez[i].dx;
                        ret.m_bez[i+1].ay = ret.m_bez[i].dy;
                    }

                    ret.m_bez[ret.n_bez-1].dx = ret.m_bez[ret.n_bez-1].dx+0.5*(ret.m_bez[ret.n_bez-1].dx-ret.m_bez[0].ax);
                    ret.m_bez[ret.n_bez-1].dy = ret.m_bez[ret.n_bez-1].dy+0.5*(ret.m_bez[ret.n_bez-1].dy-ret.m_bez[0].ay);
                    ret.m_bez[0].ax = ret.m_bez[ret.n_bez-1].dx;
                    ret.m_bez[0].ay = ret.m_bez[ret.n_bez-1].dy;

                    return ret;
    }
void Outline::fill_polygon(int x0,int y0)
    {
        steps=0;
        int x=x0;
        int y=y0;

        if(m_map[y*MapPixels+x]==0)
            {
            std::cout<<"invalid for fill\n";
            return;
            }
        //cout<<"filling in polygon...\n";

        while(m_map[y*MapPixels+x-1]==1)
            x--;

        if(x<x0)
            {
            fill_line_down(x,x0,y0+1);
            fill_line_up(x,x0,y0-1);
            }

        while(m_map[y*MapPixels+x]==1)
        {
            m_map[y*MapPixels+x]=0;
            x++;
        }
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

        while(m_map[y0*MapPixels+x-1]==1)
            x--;

        if(x<x0)
            fill_line_down(x,x0,y0+1);

        while(x<=x1)
        {
            if(m_map[y0*MapPixels+x]==1)
                {
                    temp=x;
                    m_map[y0*MapPixels+x]=0;
                    while(m_map[y0*MapPixels+x+1]==1)
                    {
                        m_map[y0*MapPixels+x+1]=0;
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

        while(m_map[y0*MapPixels+x-1]==1)
            x--;

        if(x<x0)
            fill_line_up(x,x0,y0-1);

        while(x<=x1)
        {
            if(m_map[y0*MapPixels+x]==1)
                {
                    temp=x;
                    m_map[y0*MapPixels+x]=0;
                    while(m_map[y0*MapPixels+x+1]==1)
                    {
                        m_map[y0*MapPixels+x+1]=0;
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

void Outline::trace(int x0,int y0)
    {

        //int x0=100;
        //int y0=0;
        //int x,y;
        n_trace = 0;
        //std::cout<<"tracing... "<<x0<<","<<y0<<" ... ";
/*
        for(int i=0;i<512;i++)
        {
            if(m_map[i*512+x0]==1)
            {
                x=x0;
                y=i;
                cout<<"outlining... "<<x<<" "<<y<<"\n";
                break;
            }
        }*/

        open0(x0,y0);

        m_trace[n_trace]=m_trace[1];
        n_trace++;
        std::cout<<"  polylines: "<<n_trace<<"\n";
        if(n_trace>=(max_trace-1)) {std::cout<<"Polyline limit reached !\n";}

        //std::cout<<n_trace<<" points!\n";

        fill_polygon(x0,y0);

        simplify2();

       // for(int i=0;i<n_trace;i++)
        //{
        // m_map[m_trace[i].y0*512+m_trace[i].x0]=3;
        //}
    }
void Outline::simplify2()
    {
        if(n_trace == 0) return;
        line_seg smap[max_trace];
        int SMAP_SIZE=max_trace;
        int j=0;
        int ii;
        int c=0;
        double slope=0;
        double slope1=0;
        bool pull;

        for(int i=2;i<n_trace-1;i++)
        {
            pull=false;
            if(m_trace[i+1].x0-m_trace[i].x0==0 && m_trace[i+1].x0-m_trace[i].x0==0)
            {
                slope=slope1; //eliminate double point
                //pull=true;
                continue;
            }
            else if(m_trace[i+1].x0-m_trace[i].x0==0 )
            {
                //cout<<m_trace[i].x0<<","<<m_trace[i].y0<<"   "<<m_trace[i+1].x0<<","<<m_trace[i+1].y0<<"\n";
                slope=999999;
            }
            else
            {
                slope = double(m_trace[i].y0-m_trace[i-2].y0) / (m_trace[i].x0-m_trace[i-2].x0);
            }
          //  if(i>0 && slope == slope1 && c<5)
        //    if(i>0 && c<3 )
        if(false)
            {
            c++;
            }
            else
            {
//             ASSERT(j<1200);
             smap[j]=m_trace[i];
             j++;
             c=0;
             //cout<<slope-slope1<<"\n";
            }
            slope1=slope;
        }

        line_seg* amap = new line_seg[j+10];
        int AMAP_SIZE=j+10;

        for(int i=0;i<j;i++)
        {
            //ASSERT(5+i<AMAP_SIZE);
            amap[5+i]=smap[i];
        }
        for(int i=0;i<5;i++)
        {
            //ASSERT(i<AMAP_SIZE);
           // ASSERT(j+5+i<AMAP_SIZE);
            amap[i]=smap[j-5+i];
            amap[j+5+i]=smap[i];
        }
        double dist;
        double angle,a,d,x,y,sum,a2,d2;
        bool turned;
    for(int r=0;r<2;r++)
    {
        for(int i=5;i<j+5;i++)
        {
            sum=0;
            for(int ii=1;ii>0;ii--)
                {
                   // if(amap[i-ii].x0-amap[i+ii].x0==0)
                   //     slope=999999;
                   // slope = double(amap[i-ii].y0-amap[i+ii].y0) / (amap[i-ii].x0-amap[i+ii].x0);
               //    ASSERT(i-ii<AMAP_SIZE && i-ii>=0);
               //    ASSERT(i+ii<AMAP_SIZE);
                    if(amap[i-ii].x0-amap[i+ii].x0 != 0)
                        angle = atan2(double(amap[i-ii].y0-amap[i+ii].y0),(amap[i-ii].x0-amap[i+ii].x0));
                    else
                        angle = amap[i-ii].y0 < amap[i+ii].y0 ? PI/2 : -PI/2;
                    x=amap[i].x0;
                    y=amap[i].y0;
                    a = atan2(y-amap[i-ii].y0,x-amap[i-ii].x0) - angle;
                    d = sqrt((x-amap[i-ii].x0)*(x-amap[i-ii].x0)+(y-amap[i-ii].y0)*((y-amap[i-ii].y0)));
                    a2 = atan2(y-amap[i+ii].y0,x-amap[i+ii].x0) - angle;
                    d2 = sqrt((x-amap[i+ii].x0)*(x-amap[i+ii].x0)+(y-amap[i+ii].y0)*((y-amap[i+ii].y0)));
                    sum+=fabs(d*sin(a)*d*cos(a))/2+fabs(d2*sin(a2)*d2*cos(a2))/2;
                    //cout<<d*sin(a)<<" ";
                    //
                }
              //  ASSERT(i<AMAP_SIZE);
                amap[i].slope=sum;
            //cout<<sum<<"\n";
        }
        //for(int i=0;i<j+10;i++)
       // {
       //     cout<<amap[i].x0<<" "<<amap[i].y0<<"\n";
       // }

        for(int i=5;i<j+5;i++)
        {
           // ASSERT(i-5<SMAP_SIZE && i-5 >= 0);
            smap[i-5].slope=(
                             amap[i].slope);
                           /* -( amap[i-2].slope+amap[i+2].slope
                             +amap[i-1].slope+amap[i+1].slope
                             +amap[i-3].slope+amap[i+3].slope)/6);*/
        }
        bool above = smap[0].slope>0;

        c=0;
        double peak=0;
//        above = smap[0].slope>0;
        smap[0].key=true;
        for(int i=0;i<j;i++)
        {

          //  if(smap[i].slope > r*0.5)
            {

               smap[i].key=true;
              // above=!above;
             //  peak=0;
               //smap[i].key=true;
            c++;
            }

        }
    }//for r
    //cout<<"  c="<<c<<"\n";


        memcpy(&m_trace,&smap,j*sizeof(line_seg));
        n_trace=j;
        //cout<<"  polygon: "<<j<<"\n";

        delete[] amap;
    }
void Outline::simplify()
    {
        if(n_trace == 0) return;
        line_seg smap[max_trace];
        int j=0;
        int ii;
        int c=0;
        double slope,slope1;

        for(int i=0;i<n_trace-1;i++)
        {


         if(m_trace[i+1].x0-m_trace[i].x0==0)
            slope=999999;
         else
            slope = double(m_trace[i+1].y0-m_trace[i].y0) / (m_trace[i+1].x0-m_trace[i].x0);
         if(i>0 && slope == slope1)
         {
            c++;
         }
         else
         {
             smap[j]=m_trace[i];
             j++;
         }
         slope1=slope;

         //ii=0;
         //cout<<slope<<"\n";
            /*
             while( (m_trace[i+2+ii].x0==m_trace[i].x0?999999:
                   abs(double(m_trace[i+2+ii].y0-m_trace[i].y0) / (m_trace[i+2+ii].x0-m_trace[i].x0)) - slope) < 0.01)
             {
                 //cout<<i<<"\n";
                ii++;
                c++;
             }*/
         //i+=ii;
        }
        memcpy(&m_trace,&smap,j*sizeof(line_seg));
        n_trace=j;
        //std::cout<<"polygon: "<<j<<"\n";
    }
void Outline::open0(int x0,int y0)
    {
        int x=x0;
        int y=y0;
        if(n_trace>=max_trace-1)return;

        m_trace[n_trace].x0=x;
        m_trace[n_trace].y0=y;
        n_trace++;
        if(m_trace[2].x0==x && m_trace[2].y0==y && n_trace > 3)
        {
            return;
        }
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
        if(m_trace[2].x0==x && m_trace[2].y0==y && n_trace > 3)
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
        if(m_trace[2].x0==x && m_trace[2].y0==y && n_trace > 3)
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
        if(m_trace[2].x0==x && m_trace[2].y0==y && n_trace > 3)
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
            open1(x,y);
        }
        //else cout<<"Stop!\n";
    }
void Outline::init(NoiseMap& hMap,unsigned char height)
    {
        if(m_map)
            delete[] m_map;
        m_map = new char[MapPixels*MapPixels];

        n_trace=0;

         for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                if((unsigned char)hMap.m_map[j*MapPixels+i] > height)
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

             m_map[MapPixels+i]=0;
             m_map[(MapPixels-2)*MapPixels+i]=0;
             m_map[i*MapPixels+1]=0;
             m_map[i*MapPixels+(MapPixels-2)]=0;
        }
    }
