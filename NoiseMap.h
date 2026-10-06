#ifndef _NOISEMAP_H_
#define _NOISEMAP_H_
#include <iostream>



class XVector
{
    public:
    float X;
    float Y;
    XVector()
    {}
    XVector(float x,float y)
    {
        X=x;
        Y=y;
    }
    float length();
    XVector normalize()
    {
        float l = this->length();
        X = X/l;
        Y = Y/l;
        return *this;
    }
    XVector scale(float f)
    {
        X = X*f;
        Y = Y*f;
        return *this;
    }
};

static double lerp(double t, double a, double b) { return a + t * (b - a); }
static double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }

/*
const int MapPixels = 512;
const int GridResolution = 128;
const int GridIntegrals = 4;
*/

class NoiseMap
{
public:

    const int MapPixels;
    const int GridResolution;
    const int GridIntegrals;

//    char m_map[MapPixels][MapPixels];
  //  XVector m_Vectors[GridIntegrals+1][GridIntegrals+1];

   char* m_map = NULL;
   float* m_fmap = NULL;
   XVector* m_Vectors = NULL;
   double z_offset=0;
   double z_factor=0;

    NoiseMap():
        MapPixels(512),
        GridResolution(32),
        GridIntegrals(16)
    {
    }
    NoiseMap(int pixels,int grid_integrals):
        MapPixels(pixels),
        GridResolution(pixels/grid_integrals),
        GridIntegrals(grid_integrals)
    {
    }
    ~NoiseMap()
    {
        if( m_map != NULL)
            delete[] m_map;
        if( m_fmap != NULL)
            delete[] m_fmap;
        if(m_Vectors != NULL)
            delete[] m_Vectors;
    }
    void initGrid();
    void init(float seed);
    void addNoise(NoiseMap &Map2,float scale,float offset = -0.5);
    void rescale();
    void postprocess();
    double height_fmap(int h)
    {
        return (double)h/254*z_factor+z_offset;
    }
};
/*
struct line_seg
{
  int x0;
  int y0;
  //int slope;
};*/





#endif
