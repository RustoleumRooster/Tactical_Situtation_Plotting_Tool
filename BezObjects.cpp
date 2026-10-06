
#include "PlotObjects.h"
#include "PolyTypes.h"
#include "Poly34.h"

//extern int screen_corners[18];
using namespace std;


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


//bool int open_status[16];
//0 open
//1 closed
void SearchRight(CurvePoly_solid& poly, int i, double z0, double z1,double sub_pos);

void SearchLeft(CurvePoly_solid& poly, int i, double zz0, double zz1, double sub_pos/*, double z0, double z1*/)
{
    double res_d=4.0;
    int res_i=-1;
    int close_i=-1;
    double z0=zz0;
    double z1=zz1;
   // std::cout<<i<<" left "<<z0<<" "<<z1<<"\n";
    for(int j=0;j<poly.nIntervals;j++)
            {
                if(poly.z_pos[2*j]==z1)
                {
                    close_i=j;

                }

            //    std::cout<<"? "<<i<<" "<<poly.z_pos[(2*i)+1]<<" "<<j<<" "<<poly.z_pos[2*j]<<"\n";
                if(/*(poly.interval[2*j]>poly.interval[(2*i)+1]) &&*/ poly.z_pos[2*j]>z0 && poly.z_pos[2*j]<z1)
             //   if((poly.interval[2*j]>poly.interval[(2*i)+1]) && poly.z_pos[2*j]>poly.z_pos[(2*i)+1]
             //                                                  /* && poly.z_pos[2*j]<=poly.z_pos[(2*i)]*/ && !res)
                {
                    //res=true;
                    if(poly.z_pos[2*j]<=res_d)
                    {
                     res_d=poly.z_pos[2*j];
                     res_i=j;
                    }
                }
            }
            double zpos1,zpos2;
                    int zp1,zp2;
                if(res_i>=0)
                {
                 //   std::cout<<"aaOK "<<i<<" "<<res_i<<"\n";
                    if(i==res_i) return;
                    //SearchLeft(poly, res_i, poly.z_pos[(2*res_i)+1], poly.z_pos[(2*i)]);


                    zpos1=poly.z_pos[(2*i)+1]+sub_pos;
                    zpos1=zpos1 > 4.0 ? zpos1-4.0 : zpos1;
                    zpos2=poly.z_pos[2*res_i]+sub_pos;
                    zpos2=zpos2 > 4.0 ? zpos2-4.0 : zpos2;
                  //  std::cout<<" -- "<<zpos1<<" "<<zpos2<<"\n";
                    zp1=(int)floor(zpos1);
                    zp2=(int)floor(zpos2);
                    if(zp1>zp2)
                        zp2+=4.0;
                  //  std::cout<<"    "<<zp1<<" "<<zp2<<"\n";
                    poly.addCorner(i,zp1,zp2);

                    if(res_i != i+1 && res_i > i)
                    {
                    //    std::cout<<"swap!"<<res_i<<" "<<(i+1)<<"\n";
                        //poly.swapIntervals(res_i,i+1);
                       // res_i=i+1;
                    }
                    poly.close_to[i]=res_i;
                    SearchLeft(poly, res_i, poly.z_pos[(2*res_i)+1], z1, sub_pos);
                    SearchRight(poly, res_i, poly.z_pos[(2*res_i)], poly.z_pos[(2*res_i)+1],sub_pos);
                }
                else if (close_i>=0)
                {
               //     std::cout <<"closed "<<i<<" "<<close_i<<"\n";

                    zpos1=poly.z_pos[(2*i)+1]+sub_pos;
                    zpos1=zpos1 > 4.0 ? zpos1-4.0 : zpos1;
                    zpos2=poly.z_pos[2*close_i]+sub_pos;
                    zpos2=zpos2 > 4.0 ? zpos2-4.0 : zpos2;
                  //  std::cout<<" -- "<<zpos1<<" "<<zpos2<<"\n";
                    zp1=(int)floor(zpos1);
                    zp2=(int)floor(zpos2);
                    if(zpos1>zpos2 )
                        zp2+=4.0;
                   // std::cout<<"    "<<zpos1<<" "<<zpos2<<"\n";
                   // std::cout<<"    "<<zp1<<" "<<zp2<<"\n";
                    poly.addCorner(i,zp1,zp2);

                     if(close_i != i+1 && close_i != i && close_i >i)
                    {
                   //  std::cout <<"SWAP"<<i+1<<" "<<close_i<<"\n";
                      //  poly.swapIntervals(close_i,i+1);
                        //res_i=i+1;

                    }
                    poly.close_to[i]=close_i;
                    //poly.close_to[i]=close_i;
                }
}

void SearchRight(CurvePoly_solid& poly, int i, double zz0, double zz1,double sub_pos/*, double z0, double z1*/)
{
    double res_d=0;
    int res_i=-1;
    double z0=zz0;
    double z1=zz1;
    //std::cout<<i<<" right "<<z0<<" "<<z1<<"\n";
    for(int j=0;j<poly.nIntervals;j++)
            {

               // std::cout<<"? "<<i<<" "<<poly.z_pos[(2*i)+1]<<" "<<j<<" "<<poly.z_pos[2*j]<<"\n";
                if(/*(poly.interval[2*j]>poly.interval[(2*i)+1]) &&*/ poly.z_pos[2*j]>z0 && poly.z_pos[2*j]<z1)
             //   if((poly.interval[2*j]>poly.interval[(2*i)+1]) && poly.z_pos[2*j]>poly.z_pos[(2*i)+1]
             //                                                  /* && poly.z_pos[2*j]<=poly.z_pos[(2*i)]*/ && !res)
                {
                    //res=true;
                    if(poly.z_pos[2*j]>res_d)
                    {
                     res_d=poly.z_pos[2*j];
                     res_i=j;
                    }
                }
            }
                if(res_i>=0)
                {
                    //SearchLeft(poly, res_i, poly.zpos[i*2]);
                    //SearchRight(poly, res_i);
                   // std::cout<<"bbOK "<<i<<" "<<res_i<<"\n";
                    poly.close_to[res_i]=-2;
                    if(i==res_i) return;
                    SearchLeft(poly, res_i, poly.z_pos[(2*res_i)+1], poly.z_pos[(2*res_i)],sub_pos);
                    //SearchRight(poly, res_i, poly.z_pos[(2*i)], poly.z_pos[(2*res_i)+1]);
                    SearchRight(poly, res_i, z0, poly.z_pos[(2*res_i)+1],sub_pos);

                }
}

int poly3_roots(double* res, polynomial3 poly)
    {
        //polynomial3 poly=FitPoly(&points[0],nPoints);

       // std::cout<<"poly3_roots  "<<poly.a<<" "<<poly.b<<" "<<poly.c<<" "<<poly.d<<"\n";
        double a,b,c,d;

        d = poly.a;//poly.d;
        c = poly.b;//poly.d;
        b = poly.c;//poly.d;
        a = poly.d;

        //cout<<"  "<<a<<" "<<b<<" "<<c<<" "<<"\n";

       // double x=0;
       // double y= x*x*x + c*x*x + b*x + a;

        int nRoots=0;
       // std::cout<<"hi\n";
        if(d>0.0001 || d<-0.0001)
        //if(d>0.0001 )
        {
         //   std::cout<<"P3: "<<c/d<<" "<<b/d<<" "<<a/d<<"\n";
            nRoots = SolveP3(&res[0],c/d,b/d,a/d);
            if(nRoots == 3)
            {
                if(res[1]==res[2] || res[0]==res[2] ) nRoots = 2;
                else if (res[0]==res[1])
                {
                 res[1]=res[2];
                 nRoots=2;
                }
            }
        }
          else
          {
                if (c<0.0001 && c> -0.0001)
                    {
                    if (b<0.0001 && b>-0.0001)
                        {
                        //no solutions
                        std::cout<<"(poly3_roots... no solution)\n";
                        nRoots= 0;
                        return 0;
                        }
                    //linear solution
                 //   std::cout<<"linear solution, a="<<a<<" b="<<b<<"\n";
                    // roots[0] = - a / b;
                     res[0] = - a / b;
                     nRoots= 1;
                    }
                else
                    {
                        // quadratic solution
                        double q = sqrt(b*b - 4*a*c), a2 = 2*c;
                        res[0] = (q-b)/a2;
                        res[1] = (-b-q)/a2;
                        nRoots= 2;
                    }
          }
    return nRoots;
    }

bool CurveObject::getPoly(CurvePoly_solid& poly,Scene scene)
{
     if(m_BoundingBox.x1 - 0.0001 < scene.x0 ||
           m_BoundingBox.x0 + 0.0001 > scene.x1 ||
           m_BoundingBox.y1 - 0.0001 < scene.y0 ||
           m_BoundingBox.y0 + 0.0001 > scene.y1 )
           {
             // std::cout<<"canceled\n";
            return false;
           }

    //std::cout<<"new poly\n";
    poly.n_bez=n_bez;
    poly.n_roots=0;

    double pos[16];

    for(int i=0;i<n_bez;i++)
    {
        poly.m_bez[i].ax=SceneX(m_bez[i].ax,scene);
        poly.m_bez[i].bx=SceneX(m_bez[i].bx,scene);
        poly.m_bez[i].cx=SceneX(m_bez[i].cx,scene);
        poly.m_bez[i].dx=SceneX(m_bez[i].dx,scene);

        poly.m_bez[i].ay=SceneY(m_bez[i].ay,scene);
        poly.m_bez[i].by=SceneY(m_bez[i].by,scene);
        poly.m_bez[i].cy=SceneY(m_bez[i].cy,scene);
        poly.m_bez[i].dy=SceneY(m_bez[i].dy,scene);

        double roots[4];
        int nRoots=0;

        double polyTerms[4];
      //  std::cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.x0<<" --- "<<m_bez[i].m_BoundingBox.x1<<" ("<<scene.x0<<")\n";
      //  std::cout<<"    "<<i<<" "<<m_bez[i].ax<<" --- "<<m_bez[i].dx<<"\n";
        if(m_bez[i].m_BoundingBox.x0 - 0.0001 <scene.x0 && m_bez[i].m_BoundingBox.x1 + 0.0001 >scene.x0 &&
                !(m_bez[i].m_BoundingBox.y1 < scene.y0) &&
                !(m_bez[i].m_BoundingBox.y0 > scene.y1))
        {
            poly.m_bez[i].poly_form_x(&polyTerms[0],-screen_corners[0]);
            nRoots = poly3_roots(&roots[0],polynomial3(polyTerms[0],polyTerms[1],polyTerms[2],polyTerms[3]));
            for(int j=0;j<nRoots;j++)
            {
                double test_y = m_bez[i].test_y(roots[j]);
                if(roots[j] >0 && roots[j] - 0.0001 < 1.0 && test_y > scene.y0 && test_y < scene.y1 )
                {
                    poly.roots[poly.n_roots]=roots[j]+i;
                    pos[poly.n_roots]=(test_y-scene.y0) / (scene.y1-scene.y0);
                    poly.n_roots++;
             //       cout<<roots[j]+i<<" ... ok\n";
                }
             //   else cout<<roots[j]<<" ... outside range\n";
            }
            if(nRoots==0) // case y0 = y1 = scene.y1
            {
               // poly.roots[poly.n_roots]=i;
               // pos[poly.n_roots]=((m_bez[i].test_y(0)-scene.y0) / (scene.y1-scene.y0));
               // poly.n_roots++;
          //      cout<<"(no roots found) "<<i<<"\n";
               // std::cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.y0<<" --- "<<m_bez[i].m_BoundingBox.y1<<" ("<<scene.y0<<")\n";
               // std::cout<<"    "<<i<<" "<<m_bez[i].ay<<" --- "<<m_bez[i].dy<<"\n";
            }
        }

        if(m_bez[i].m_BoundingBox.x0 - 0.0001 < scene.x1 && m_bez[i].m_BoundingBox.x1 + 0.0001 > scene.x1 &&
                !(m_bez[i].m_BoundingBox.y1 < scene.y0) &&
                !(m_bez[i].m_BoundingBox.y0 > scene.y1))
        {
            poly.m_bez[i].poly_form_x(&polyTerms[0],-screen_corners[4]); //-512-64
            nRoots = poly3_roots(&roots[0],polynomial3(polyTerms[0],polyTerms[1],polyTerms[2],polyTerms[3]));

            for(int j=0;j<nRoots;j++)
            {
                double test_y = m_bez[i].test_y(roots[j]);
                if(roots[j] > 0 && roots[j] - 0.0001 < 1.0 && test_y > scene.y0 && test_y < scene.y1 )
                {
                    poly.roots[poly.n_roots]=roots[j]+i;
                    pos[poly.n_roots]=2.0 + ((scene.y1-test_y) / (scene.y1-scene.y0));
                    poly.n_roots++;
                }
            }
            if(nRoots==0) // case y0 = y1 = scene.y1
            {
               // poly.roots[poly.n_roots]=i;
               // pos[poly.n_roots]=2.0 + ((scene.y1-m_bez[i].test_y(0)) / (scene.y1-scene.y0));
               // poly.n_roots++;
             //   cout<<"(no roots found) "<<i<<"\n";
               // std::cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.y0<<" --- "<<m_bez[i].m_BoundingBox.y1<<" ("<<scene.y0<<")\n";
               // std::cout<<"    "<<i<<" "<<m_bez[i].ay<<" --- "<<m_bez[i].dy<<"\n";
            }
        }

        if(m_bez[i].m_BoundingBox.y0 - 0.0001 < scene.y0 && m_bez[i].m_BoundingBox.y1 + 0.0001 > scene.y0 &&
                !(m_bez[i].m_BoundingBox.x1 < scene.x0) &&
                !(m_bez[i].m_BoundingBox.x0 > scene.x1))
        {
            poly.m_bez[i].poly_form_y(&polyTerms[0],-screen_corners[1]);//-64
            nRoots = poly3_roots(&roots[0],polynomial3(polyTerms[0],polyTerms[1],polyTerms[2],polyTerms[3]));

            for(int j=0;j<nRoots;j++)
            {
                double test_x = m_bez[i].test_x(roots[j]);
                if(roots[j] >0 && roots[j] - 0.0001 < 1.0 && test_x > scene.x0 && test_x < scene.x1 )
                {
                    poly.roots[poly.n_roots]=roots[j]+i;
                    pos[poly.n_roots]=3.0 + ((scene.x1-test_x) / (scene.x1-scene.x0));
                    poly.n_roots++;
                }
            }
            if(nRoots==0) // case y0 = y1 = scene.y1
            {
                //poly.roots[poly.n_roots]=i;
                //pos[poly.n_roots]=3.0 + ((scene.x1-m_bez[i].test_x(0)) / (scene.x1-scene.x0));
                //poly.n_roots++;
         //       cout<<"(no roots found) "<<i<<"\n";
               // std::cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.y0<<" --- "<<m_bez[i].m_BoundingBox.y1<<" ("<<scene.y0<<")\n";
               // std::cout<<"    "<<i<<" "<<m_bez[i].ay<<" --- "<<m_bez[i].dy<<"\n";
            }
        }

        if(m_bez[i].m_BoundingBox.y0 - 0.0001 < scene.y1 && m_bez[i].m_BoundingBox.y1 + 0.0001 > scene.y1 &&
                !(m_bez[i].m_BoundingBox.x1 < scene.x0) &&
                !(m_bez[i].m_BoundingBox.x0 > scene.x1))
        {
            poly.m_bez[i].poly_form_y(&polyTerms[0],-screen_corners[3]);//-512-64
            nRoots = poly3_roots(&roots[0],polynomial3(polyTerms[0],polyTerms[1],polyTerms[2],polyTerms[3]));

            for(int j=0;j<nRoots;j++)
            {
                double test_x = m_bez[i].test_x(roots[j]);
                if(roots[j] >0 && roots[j]  < 1.0 && test_x > scene.x0 && test_x < scene.x1 )
                {
                    poly.roots[poly.n_roots]=roots[j]+i;
                    pos[poly.n_roots]=1.0 + ((test_x-scene.x0) / (scene.x1-scene.x0));
                    poly.n_roots++;
                }
            }
        //     std::cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.y0<<" --- "<<m_bez[i].m_BoundingBox.y1<<" ("<<scene.y1<<")\n";
        //     std::cout<<"    "<<i<<" "<<m_bez[i].ay<<" --- "<<m_bez[i].dy<<"\n";
            if(nRoots==0) // case y0 = y1 = scene.y1
            {
                //poly.roots[poly.n_roots]=i;
                //pos[poly.n_roots]=1.0 + ((m_bez[i].test_x(0)-scene.x0) / (scene.x1-scene.x0));
                //poly.n_roots++;
                cout<<"(no roots found) "<<i<<"\n";
                std::cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.y0<<" --- "<<m_bez[i].m_BoundingBox.y1<<" ("<<scene.y0<<")\n";
                std::cout<<"    "<<i<<" "<<m_bez[i].ay<<" --- "<<m_bez[i].dy<<"\n";
            }
        }
    } //for(int i=0;i<n_bez;i++)

    for(int i=0;i<poly.n_roots;i++)
        for(int j=i+1;j<poly.n_roots;j++)
        {
            double r,z;

            if(poly.roots[j]<poly.roots[i])
            {
                r=poly.roots[j];
                poly.roots[j]=poly.roots[i];
                poly.roots[i]=r;

                z = pos[j];
                pos[j]=pos[i];
                pos[i]=z;
            }
        }
/*
    std::cout<<"roots: \n";
    for(int i=0;i<poly.n_roots;i++)
    {
        std::cout<<poly.roots[i]<<" ";
    }
    std::cout<<"\n";
*/
    poly.nIntervals=0;
    //bool bInside = m_bez[0].ax - 0.0001 < scene.x1 && m_bez[0].ax + 0.0001 > scene.x0 && m_bez[0].ay - 0.0001 < scene.y1 && m_bez[0].ay + 0.0001> scene.y0;

    bool bInside = m_bez[0].ax < scene.x1 && m_bez[0].ax > scene.x0 && m_bez[0].ay < scene.y1 && m_bez[0].ay > scene.y0;

    double sub_pos;
    if(!bInside)
        sub_pos = pos[0];
    else
        sub_pos = pos[poly.n_roots-1];

    for(int i=0;i<poly.n_roots;i++)
    {
        pos[i]-=sub_pos;
        if(pos[i]<0)
            pos[i]+=4.0;
    }
/*
    std::cout<<"roots: ";
    for(int i=0;i<poly.n_roots;i++)
        std::cout<<pos[i]<<" ";
    std::cout<<"\n";
*/

    if(poly.n_roots == 0)
    {
       // std::cout<<"a\n";
        if(bInside)
        {
            poly.addInterval(0,(double) n_bez,0,n_bez-1,0,4.0);
        }
        else
        {
            return false;
        }
    }
    else if(bInside)
    {
       // std::cout<<"b\n";
        poly.addInterval(0.0,poly.roots[0],0,(int)floor(poly.roots[0]),4.0,pos[0]);
        int i;
        for(i=0;i<poly.n_roots-1;i++)
        {

            if(i%2)
            poly.addInterval(poly.roots[i],poly.roots[i+1],(int)floor(poly.roots[i]),(int)floor(poly.roots[i+1]),pos[i],pos[i+1]);
        }
        poly.addInterval(poly.roots[poly.n_roots-1],(double)n_bez, (int)floor(poly.roots[i]),n_bez-1,4.0,pos[i+1]);

        for(int i=0;i<poly.nIntervals;i++)
                //for(int i=0;i<1;i++)
                {
                //    res=false;
                //    std::cout<<i<<" "<<poly.range[i*2]<<" "<<poly.range[(i*2)+1]<<"\n";
                }
    }
    else
    {
     //   std::cout<<"c\n";
        for(int i=0;i<poly.n_roots-1;i++)
            {
               if(1-i%2)
               {
                    poly.addInterval(poly.roots[i],poly.roots[i+1],(int)floor(poly.roots[i]),(int)floor(poly.roots[i+1]),pos[i],pos[i+1]);
                  //  std::cout<<poly.roots[i]<<" "<< poly.roots[i+1]<<"\n";
               }
            }
            poly.z_pos[0]=4.0;
    }

//    std::cout<<"intervals 1-\n";
    for(int i=0;i<poly.nIntervals;i++)
        //for(int i=0;i<1;i++)
        {
        //    res=false;
            poly.addCorner(i,0,0);
            poly.close_to[i]=-1;

        }
/*
    std::cout<<"intervals 1-\n";
    for(int i=0;i<poly.nIntervals;i++)
        //for(int i=0;i<1;i++)
        {
        //    res=false;
            std::cout<<i<<" "<<poly.range[i*2]<<" "<<poly.range[(i*2)+1]<<" "<<poly.close_to[i]<<"\n";
        }
*/


    SearchLeft(poly,0,poly.z_pos[1],4.0,sub_pos);
    SearchRight(poly,0,0.0,poly.z_pos[1],sub_pos);

/*
    std::cout<<"intervals 2-\n";
    for(int i=0;i<poly.nIntervals;i++)
        //for(int i=0;i<1;i++)
        {
        //    res=false;
            std::cout<<i<<" "<<poly.range[i*2]<<" "<<poly.range[(i*2)+1]<<" "<<poly.close_to[i]<<"\n";
        }
*/

    bool bswapped=true;
    while(bswapped==true)
    {
        bswapped=false;
     //   std::cout<<"--\n";
         for(int i=0;i<poly.nIntervals;i++)
            {
               // std::cout<<"sw: "<<i<<" "<<poly.close_to[i]<<"\n";
             if(poly.close_to[i]>i && poly.close_to[i]!=i+1)
             {
               //  std::cout<<"...swapped!\n";
                int z = poly.close_to[i];
                poly.swapIntervals(i+1,poly.close_to[i]);

                poly.close_to[i]=i+1;
                for(int j=i+1;j<poly.nIntervals;j++)
                {
                    if(poly.close_to[j]==i+1)
                        poly.close_to[j]=z;
                }
                bswapped=true;

             }
        }
    }
/*
    std::cout<<"intervals 3-\n";
    for(int i=0;i<poly.nIntervals;i++)
        //for(int i=0;i<1;i++)
        {
        //    res=false;
            std::cout<<i<<" "<<poly.range[i*2]<<" "<<poly.range[(i*2)+1]<<" "<<poly.close_to[i]<<"\n";
        }
*/

    for(int i=poly.nIntervals;i>=0;i--)
            {

             if(poly.close_to[i]!=i+1 )
                poly.close_to[i+1]=-2;
            }



   //std::cout<<"intervals 3-\n";
   /*
    for(int i=0;i<poly.nIntervals;i++)
        //for(int i=0;i<1;i++)
        {
        //    res=false;
            std::cout<<i<<" "<<poly.range[i*2]<<" "<<poly.range[(i*2)+1]<<" "<<poly.close_to[i]<<"\n";
        }
        */
/*
    std::cout<<"intervals 2-\n";
    for(int i=0;i<poly.nIntervals;i++)
        {
            std::cout<<i<<" "<<poly.range[i*2]<<" "<<"\n";
        }
*/
    poly.close_to[0]=-2;

    //std::cout<<"zzzzz\n";

    return true;
} //CURVE POLY SOLID

bool CurveObject::getPoly(CurvePoly& poly,Scene scene)
{
     if(m_BoundingBox.x1 + 2.0001 < scene.x0 ||
           m_BoundingBox.x0 - 2.0001 > scene.x1 ||
           m_BoundingBox.y1 + 2.0001 < scene.y0 ||
           m_BoundingBox.y0 - 2.0001 > scene.y1 )
           {
             // std::cout<<"canceled\n";
            return false;
           }
    //std::cout<<"curve, n_bez="<<n_bez<<"\n";

    poly.n_bez=n_bez;
    poly.n_roots=0;

    for(int i=0;i<n_bez;i++)
    {
        poly.m_bez[i].ax=SceneX(m_bez[i].ax,scene);
        poly.m_bez[i].bx=SceneX(m_bez[i].bx,scene);
        poly.m_bez[i].cx=SceneX(m_bez[i].cx,scene);
        poly.m_bez[i].dx=SceneX(m_bez[i].dx,scene);

        poly.m_bez[i].ay=SceneY(m_bez[i].ay,scene);
        poly.m_bez[i].by=SceneY(m_bez[i].by,scene);
        poly.m_bez[i].cy=SceneY(m_bez[i].cy,scene);
        poly.m_bez[i].dy=SceneY(m_bez[i].dy,scene);
/*
        std::cout<<i<<"  "<<m_bez[i].m_BoundingBox.x0<<" "<<
                     m_bez[i].m_BoundingBox.x1<<" "<<
                     m_bez[i].m_BoundingBox.y0<<" "<<
                     m_bez[i].m_BoundingBox.y1<<"\n";
*/
        double roots[4];
        int nRoots=0;

        double polyTerms[4];
/*
        if(i<5)
        {
        std::cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.x0<<" --- "<<m_bez[i].m_BoundingBox.x1<<" ("<<scene.x0<<")\n";
        std::cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.y0<<" --- "<<m_bez[i].m_BoundingBox.y1<<" ("<<scene.x0<<")\n";
        std::cout<<"    "<<i<<" "<<m_bez[i].ax<<" --- "<<m_bez[i].dx<<"\n";
        }
*/
        if(m_bez[i].m_BoundingBox.x0 - 0.0001 < scene.x0 && m_bez[i].m_BoundingBox.x1 + 0.0001 > scene.x0 &&
                !(m_bez[i].m_BoundingBox.y1 + 0.0001 < scene.y0) &&
                !(m_bez[i].m_BoundingBox.y0 - 0.0001 > scene.y1))
        {
            poly.m_bez[i].poly_form_x(&polyTerms[0],-screen_corners[0]);
            nRoots = poly3_roots(&roots[0],polynomial3(polyTerms[0],polyTerms[1],polyTerms[2],polyTerms[3]));
            for(int j=0;j<nRoots;j++)
            {
                if(roots[j] + 0.0001 > 0 && roots[j] - 0.0001 < 1.0 && m_bez[i].test_y(roots[j]) + 0.0001 > scene.y0 && m_bez[i].test_y(roots[j]) - 0.0001 < scene.y1 )
                {
                    //discard identicals
                    if(poly.n_roots==0 || fabs(roots[j]+i - poly.roots[poly.n_roots-1]) > 0.0001)
                    {
                     //   cout<<scene.x0<<"\n";
                     //   cout<<"test_x "<<i<<" "<<roots[j]+i<<" "<<m_bez[i].test_x(roots[j]+0.01)<<"\n";
                        poly.roots[poly.n_roots]=roots[j]+i;

                        //in-out test
                        double dx = (m_bez[i].test_x(roots[j]+0.01) - m_bez[i].test_x(roots[j]-0.01));
                        if( fabs(dx) - 0.0001 > 0 )
                           {
                            poly.root_flag[poly.n_roots] = dx>0?1:-1;
                            poly.n_roots++;
                           //cout<<" ok\n";
                           }
                        else
                        {
                            poly.root_flag[poly.n_roots] = 0;
                            //cout<<"not ok "<<roots[j]<<"\n";
                        }
                    }
                }
             /*
             else
                {cout<<roots[j]<<"("<<i<<") ... outside range\n";
                cout<<polyTerms[0]<<" "<<polyTerms[1]<<" "<<polyTerms[2]<<" "<<polyTerms[3]<<"\n";
                }
            */
            }
         //   if(nRoots==0)
         //   {
         //       cout<<i<<"  "<<m_bez[i].ax<<" "<<m_bez[i].bx<<" "<<m_bez[i].cx<<" "<<m_bez[i].cx<<" "<<"\n";
         //       cout<<"error, no roots?\n";
          //  }
        }//X0

        if(m_bez[i].m_BoundingBox.x0 - 0.0001 < scene.x1 && m_bez[i].m_BoundingBox.x1 + 0.0001 > scene.x1 &&
                !(m_bez[i].m_BoundingBox.y1 + 0.0001 < scene.y0) &&
                !(m_bez[i].m_BoundingBox.y0 - 0.0001 > scene.y1))
        {
            poly.m_bez[i].poly_form_x(&polyTerms[0],-screen_corners[4]);
            nRoots = poly3_roots(&roots[0],polynomial3(polyTerms[0],polyTerms[1],polyTerms[2],polyTerms[3]));

            for(int j=0;j<nRoots;j++)
            {
                if(roots[j] + 0.0001  >0 && roots[j] - 0.0001 <1.0 && m_bez[i].test_y(roots[j]) + 0.0001 > scene.y0 && m_bez[i].test_y(roots[j]) - 0.0001 < scene.y1 )
                {
                   if(poly.n_roots==0 || fabs(roots[j]+i - poly.roots[poly.n_roots-1]) > 0.0001)
                    {
                    poly.roots[poly.n_roots]=roots[j]+i;

                    double dx = (m_bez[i].test_x(roots[j]+0.01) - m_bez[i].test_x(roots[j]-0.01));
                    if( fabs(dx) - 0.0001 > 0 )
                       {
                        poly.root_flag[poly.n_roots] = dx<0?1:-1;
                        poly.n_roots++;
                       }
                    else
                        poly.root_flag[poly.n_roots] = 0;


                  //  cout<<roots[j]+i<<"("<<i<<") ... ok! x1\n";
                    }
                   // else cout<<roots[j]+i<<"("<<i<<") ... outside range x1\n";
                //   cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.x0<<" --- "<<m_bez[i].m_BoundingBox.x1<<" ("<<scene.x1<<")\n";
                }

               // cout<<"     test_0 = "<<m_bez[i].test_x(0)<<"\n";
               // cout<<"     test_y = "<<m_bez[i].test_y(roots[j])<<"\n";
            }
        } //X1

         if(m_bez[i].m_BoundingBox.y0 - 0.0001 < scene.y0 && m_bez[i].m_BoundingBox.y1 + 0.0001 > scene.y0 &&
                !(m_bez[i].m_BoundingBox.x1 + 0.0001 < scene.x0) &&
                !(m_bez[i].m_BoundingBox.x0 - 0.0001 > scene.x1))
        {
            poly.m_bez[i].poly_form_y(&polyTerms[0],-screen_corners[1]);
            nRoots = poly3_roots(&roots[0],polynomial3(polyTerms[0],polyTerms[1],polyTerms[2],polyTerms[3]));

            for(int j=0;j<nRoots;j++)
            {
                if(roots[j] + 0.0001 > 0 && roots[j] - 0.0001 < 1.0 && m_bez[i].test_x(roots[j])  + 0.0001  > scene.x0 && m_bez[i].test_x(roots[j]) - 0.0001 < scene.x1 )
                {
                   // cout<<roots[j]+i<<" "<<m_bez[i].test_x(roots[j])<<"/"<<scene.x1<<"\n";
                   if(poly.n_roots==0 || fabs(roots[j]+i - poly.roots[poly.n_roots-1]) > 0.0001)
                    {
                        poly.roots[poly.n_roots]=roots[j]+i;

                        double dy = (m_bez[i].test_y(roots[j]+0.01) - m_bez[i].test_y(roots[j]-0.01));
                        if( fabs(dy) - 0.0001 > 0 )
                           {
                            poly.root_flag[poly.n_roots] = dy>0?1:-1;
                            poly.n_roots++;
                           }
                        else
                            poly.root_flag[poly.n_roots] = 0;


                       //    cout<<roots[j]+i<<"("<<i<<") ... ok! y0\n";
                        //   cout<<"bez "<<i<<" "<<m_bez[i].m_BoundingBox.x0<<" --- "<<m_bez[i].m_BoundingBox.x1<<" ("<<scene.x1<<")\n";
                    }
             //   else cout<<roots[j]+i<<"("<<i<<") ... outside range y0\n";
               // cout<<"     test_0 = "<<m_bez[i].test_x(0)<<"\n";
               // cout<<"     test_y = "<<m_bez[i].test_y(roots[j])<<"\n";
                }
            }
        }//Y0

         if(m_bez[i].m_BoundingBox.y0 - 0.0001 < scene.y1 && m_bez[i].m_BoundingBox.y1 + 0.0001 > scene.y1 &&
                !(m_bez[i].m_BoundingBox.x1 + 0.0001 < scene.x0) &&
                !(m_bez[i].m_BoundingBox.x0 - 0.0001 > scene.x1))
        {
            poly.m_bez[i].poly_form_y(&polyTerms[0],-screen_corners[3]);
            nRoots = poly3_roots(&roots[0],polynomial3(polyTerms[0],polyTerms[1],polyTerms[2],polyTerms[3]));

            for(int j=0;j<nRoots;j++)
            {
                if(roots[j] + 0.0001 > 0 && roots[j] - 0.0001 <1.0 && m_bez[i].test_x(roots[j]) + 0.0001  > scene.x0 && m_bez[i].test_x(roots[j]) - 0.0001 < scene.x1 )
                {
                    if(poly.n_roots==0 || fabs(roots[j]+i - poly.roots[poly.n_roots-1]) > 0.0001)
                    {
                    //    cout<<scene.y1<<"\n";
                    //    cout<<"test_y1 "<<i<<" "<<roots[j]+i<<" "<<m_bez[i].test_y(roots[j]+0.01)<<"\n";

                        poly.roots[poly.n_roots]=roots[j]+i;

                        double dy = (m_bez[i].test_y(roots[j]+0.01) - m_bez[i].test_y(roots[j]-0.01));
                        if( fabs(dy) - 0.0001 > 0 )
                           {
                            poly.root_flag[poly.n_roots] = dy<0?1:-1;
                            poly.n_roots++;
                           }
                        else
                            poly.root_flag[poly.n_roots] = 0;

                      //  cout<<roots[j]+i<<"("<<i<<") ... "<<(int)poly.root_flag[poly.n_roots]<<".... ok!\n";

                    }
              //   cout<<roots[j]+i<<"("<<i<<") ... ok! y1\n";
                }
              //  else cout<<roots[j]+i<<"("<<i<<") ... outside range y1\n";
              //  cout<<"     test_0 = "<<m_bez[i].test_x(0)<<"\n";
              //  cout<<"     test_x = "<<m_bez[i].test_x(roots[j])<<"\n";
            }
        }//Y1
    } //for(int i=0;i<n_bez;i++)

    //Sort roots by t
    for(int i=0;i<poly.n_roots;i++)
        for(int j=i+1;j<poly.n_roots;j++)
        {
            double r;
            char c;
            if(poly.roots[j]<poly.roots[i])
            {
                r=poly.roots[j];
                poly.roots[j]=poly.roots[i];
                poly.roots[i]=r;

                c=poly.root_flag[j];
                poly.root_flag[j]=poly.root_flag[i];
                poly.root_flag[i]=c;
            }
        }
/*
    std::cout<<"roots: \n";
    for(int i=0;i<poly.n_roots;i++)
    {
        std::cout<<poly.roots[i]<<"("<<(int)poly.root_flag[i]<<")  ";
    }
    std::cout<<"\n";
*/
    //Should be: 1 -1 1 -1
    for(int i=0;i<poly.n_roots-1;i++)
    {
        if(poly.root_flag[i] == 0) cout<<"Error-- Bad Root!\n";
        else if(poly.root_flag[i] == poly.root_flag[i+1])
        {
            for(int j=i+1;j<poly.n_roots-1;j++)
            {
                poly.roots[j]=poly.roots[j+1];
                poly.root_flag[j]=poly.root_flag[j+1];
            }
            poly.n_roots--;
           // cout<<"Removed one root...\n";
        }

    }
/*
    std::cout<<"roots: \n";
    for(int i=0;i<poly.n_roots;i++)
    {
        std::cout<<poly.roots[i]<<"("<<(int)poly.root_flag[i]<<")  ";
    }
    std::cout<<"\n";
*/
    poly.nIntervals=0;

    //std::cout<<"ok2\n";

    if(poly.n_roots == 0)
    {
       // cout<<"a\n";
        if(m_bez[0].ax + 0.0001 < scene.x1 && m_bez[0].ax - 0.0001 > scene.x0 && m_bez[0].ay + 0.0001 < scene.y1 && m_bez[0].ay - 0.0001 > scene.y0)
        {
            poly.addInterval(0,(double) n_bez,0,n_bez-1);
        }
        else
        {
            return false;
        }
    }
    else if(m_bez[0].ax + 0.0001 < scene.x1 && m_bez[0].ax - 0.0001 > scene.x0 && m_bez[0].ay + 0.0001< scene.y1 && m_bez[0].ay - 0.0001> scene.y0)
    {
       // cout<<"b\n";
      //  cout<<"ax "<<m_bez[0].ax<<" scene.x1 "<<scene.x1<<"\n";
      //  cout<<"bb: "<<m_bez[0].m_BoundingBox.x0<<" "<<m_bez[0].m_BoundingBox.x1<<"\n";
        if(poly.root_flag[0] == -1)
        {
            if(poly.n_roots%2==0)
                {
                poly.addInterval(0.0,poly.roots[0],0,(int)floor(poly.roots[0]));
                int i;
                for(i=0;i<poly.n_roots-1;i++)
                {
                    if(i%2)
                    poly.addInterval(poly.roots[i],poly.roots[i+1],(int)floor(poly.roots[i]+0.0001),(int)floor(poly.roots[i+1]-0.0001));
                }
                poly.addInterval(poly.roots[poly.n_roots-1],(double)n_bez, (int)floor(poly.roots[i]),n_bez-1);
                }
            else
            {
                poly.addInterval(0.0,poly.roots[0],0,(int)floor(poly.roots[0]));
                int i;
                for(i=0;i<poly.n_roots-1;i++)
                {
                    if(1-i%2)
                    poly.addInterval(poly.roots[i],poly.roots[i+1],(int)floor(poly.roots[i]+0.0001),(int)floor(poly.roots[i+1]-0.0001));
                }
                poly.addInterval(poly.roots[poly.n_roots-1],(double)n_bez, (int)floor(poly.roots[i]),n_bez-1);
            }
        }
    }
    else
    {
       // cout<<"c\n";
        int j=0;
/*
        double x = m_bez[(int)floor(poly.roots[j])].test_x(poly.roots[j]);
        double y = m_bez[(int)floor(poly.roots[j])].test_y(poly.roots[j]);

        if(!(x +0.0001 < scene.x1 && x - 0.0001 > scene.x0 && y + 0.0001 < scene.y1 && y - 0.0001 > scene.y0))
        {
         "ah\n";
         j++;
        }
        */
       // if ( m_bez[j].test_x(poly.roots[j+1]

            if(poly.n_roots%2)
            {
                if(poly.root_flag[1] == 1)
                     for(int i=1;i<poly.n_roots-1;i++)
                        {
                           if(i%2)
                           {
                                poly.addInterval(poly.roots[i],poly.roots[i+1],(int)floor(poly.roots[i]+0.0001),(int)floor(poly.roots[i+1]-0.0001));
                               // cout<<"interval: "<<poly.roots[i]<<" "<<poly.roots[i+1]<<" "<<(int)floor(poly.roots[i]+0.0001)<<" "<<(int)floor(poly.roots[i+1]-0.0001)<<"\n";
                           }
                        }
                else if(poly.root_flag[0] == 1)
                    for(int i=0;i<poly.n_roots-1;i++)
                    {
                       if(1-i%2)
                       {
                            poly.addInterval(poly.roots[i],poly.roots[i+1],(int)floor(poly.roots[i]+0.0001),(int)floor(poly.roots[i+1]-0.0001));
                           // cout<<"interval: "<<poly.roots[i]<<" "<<poly.roots[i+1]<<" "<<(int)floor(poly.roots[i]+0.0001)<<" "<<(int)floor(poly.roots[i+1]-0.0001)<<"\n";
                       }
                    }
            }
            else if(poly.root_flag[0] == 1)
            {
                for(int i=0;i<poly.n_roots-1;i++)
                    {
                       if(1-i%2)
                       {
                            poly.addInterval(poly.roots[i],poly.roots[i+1],(int)floor(poly.roots[i]+0.0001),(int)floor(poly.roots[i+1]-0.0001));
                           // cout<<"interval: "<<poly.roots[i]<<" "<<poly.roots[i+1]<<" "<<(int)floor(poly.roots[i]+0.0001)<<" "<<(int)floor(poly.roots[i+1]-0.0001)<<"\n";
                       }
                    }
            }
            else
            {
                for(int i=0;i<poly.n_roots-2;i++)
                    {
                       if(i%2)
                       {
                            poly.addInterval(poly.roots[i],poly.roots[i+1],(int)floor(poly.roots[i]+0.0001),(int)floor(poly.roots[i+1]-0.0001));
                          //  cout<<"interval: "<<poly.roots[i]<<" "<<poly.roots[i+1]<<" "<<(int)floor(poly.roots[i]+0.0001)<<" "<<(int)floor(poly.roots[i+1]-0.0001)<<"\n";
                       }
                    }
              //  poly.addInterval(poly.roots[poly.n_roots-1],poly.roots[0],(int)floor(poly.roots[i]+0.0001),(int)floor(poly.roots[i+1]-0.0001));
              //  cout<<"interval: "<<poly.roots[i]<<" "<<poly.roots[i+1]<<" "<<(int)floor(poly.roots[i]+0.0001)<<" "<<(int)floor(poly.roots[i+1]-0.0001)<<"\n";
            }
    }
    //std::cout<<"ok3\n";
    if(poly.nIntervals > 0)
        return true;
    return false;
} //CURVE POLY


bool BezObject::getPoly(BezierPoly& poly,Scene scene)
{
    if(m_BoundingBox.x1 < scene.x0 ||
           m_BoundingBox.x0 > scene.x1 ||
           m_BoundingBox.y1 < scene.y0 ||
           m_BoundingBox.y0 > scene.y1 )
           {
          //  std::cout<<"rejected...\n";
            return false;
           }
   poly.Set(SceneX(ax,scene),SceneX(bx,scene),SceneX(cx,scene),SceneX(dx,scene),
               SceneY(ay,scene),SceneY(by,scene),SceneY(cy,scene),SceneY(dy,scene),0.0,1.0,(len/8)/((scene.x1-scene.x0)/scene.width));
    return true;
}
bool BezObject::getSpecialPoly(BezierPoly& poly,Scene scene)
{
    if(m_BoundingBox.x1 < scene.x0 ||
           m_BoundingBox.x0 > scene.x1 ||
           m_BoundingBox.y1 < scene.y0 ||
           m_BoundingBox.y0 > scene.y1 )
           {
          //  std::cout<<"rejected...\n";
            return false;
           }
   poly.Set(SceneX2(ax,scene),SceneX2(bx,scene),SceneX2(cx,scene),SceneX2(dx,scene),
               SceneY2(ay,scene),SceneY2(by,scene),SceneY2(cy,scene),SceneY2(dy,scene),0.0,1.0,(len/2)/((scene.x1-scene.x0)/scene.width));

    //std::cout<<SceneX2(ax,scene)<<","<<SceneY2(ay,scene)<<"\n";

    return true;
}
