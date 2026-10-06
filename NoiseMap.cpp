#include "NoiseMap.h"
#include "PolyTypes.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "poly34.h"

using namespace std;

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
        cout<<"Noise Map: Adding Noise...\n";

        for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                m_fmap[j*MapPixels+i]+=(Map2.m_fmap[j*MapPixels+i]+offset)*scale;
            }
        rescale();
    }

void NoiseMap::rescale()
    {
        cout<<"Noise Map: Rescaling...\n";
        float amin = 100;
        float amax = -100;
         for(int j=0;j<MapPixels;j++)
            for(int i=0;i<MapPixels;i++)
            {
                amax = std::max(amax,m_fmap[j*MapPixels+i]);
                amin = std::min(amin,m_fmap[j*MapPixels+i]);
            }
            cout<<"    float range: "<<amin<<" to "<<amax<<"\n";
            cout<<"    implied range: "<<(int)(((amin))*255)<<" to "<<(int)(((amax))*255)<<"\n";
            float sfactor = amax-amin+0.03;
            z_factor = sfactor;

            amin-=0.03;
            z_offset = amin;
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




