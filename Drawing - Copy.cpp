#define _GLIBCXX_USE_CXX11_ABI 0
#include <agg_basics.h>
#include <agg_rendering_buffer.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_scanline_p.h>
#include <agg_renderer_scanline.h>
#include "agg_font_win32_tt.h"
#include <agg_path_storage.h>
// Color
//#include <agg_pxlfmt_rgba.h>

// Color
//#include <agg_pxlfmt_rgba.h>

// For unclosed curve
#include <agg_conv_unclose_polygon.h>

// Specifically for the curve
#include <agg_conv_stroke.h>
#include <agg_conv_bspline.h>

#include <agg_conv_curve.h>
#include <agg_conv_contour.h>

// To save time picking a color
#define AGG_ARGB32
#include <pixel_formats.h>
#include <interactive_polygon.h>
#include <agg_renderer_primitives.h>

//
#include <irrlicht.h>

//
#include <math.h>
#define PI 3.1415926539
// basic file operations
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace irr;

class MapPoly;

int do_stuff(const unsigned char* data,irr::video::IImage* img);
unsigned int big(const unsigned char* data);
unsigned int little(const unsigned char* data);
const unsigned char* findRecord(const unsigned char* startpos,unsigned int nRecord);
MapPoly* readRecord(const unsigned char* startpos);
bool renderPolygon(irr::video::IImage* pImage,MapPoly* poly) ;

unsigned int fsize=0;;



const int bytesPerPixel = 4; /// red, green, blue
const int fileHeaderSize = 14;
const int infoHeaderSize = 40;

void generateBitmapImage(unsigned char *image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int width);
unsigned char* createBitmapInfoHeader(int height, int width);


void generateBitmapImage(unsigned char *image, int height, int width, char* imageFileName){

    unsigned char* fileHeader = createBitmapFileHeader(height, width);
    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4-(width*bytesPerPixel)%4)%4;

    FILE* imageFile = fopen(imageFileName, "wb");

    fwrite(fileHeader, 1, fileHeaderSize, imageFile);
    fwrite(infoHeader, 1, infoHeaderSize, imageFile);

    int i;
    for(i=0; i<height; i++){
        fwrite(image+(i*width*bytesPerPixel), bytesPerPixel, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char* createBitmapFileHeader(int height, int width){
    int fileSize = fileHeaderSize + infoHeaderSize + bytesPerPixel*height*width;

    static unsigned char fileHeader[] = {
        0,0, /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize    );
    fileHeader[ 3] = (unsigned char)(fileSize>> 8);
    fileHeader[ 4] = (unsigned char)(fileSize>>16);
    fileHeader[ 5] = (unsigned char)(fileSize>>24);
    fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width){
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0, /// number of color planes
        0,0, /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(infoHeaderSize);
    infoHeader[ 4] = (unsigned char)(width    );
    infoHeader[ 5] = (unsigned char)(width>> 8);
    infoHeader[ 6] = (unsigned char)(width>>16);
    infoHeader[ 7] = (unsigned char)(width>>24);
    infoHeader[ 8] = (unsigned char)(height    );
    infoHeader[ 9] = (unsigned char)(height>> 8);
    infoHeader[10] = (unsigned char)(height>>16);
    infoHeader[11] = (unsigned char)(height>>24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(bytesPerPixel*8);

    return infoHeader;
}

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

class MyEventReceiver : public IEventReceiver
{
public:
    // We'll create a struct to record info on the mouse state
    struct SMouseState
    {
        core::position2di Position;
        bool LeftButtonDown;
        int WheelPos;
        SMouseState() : LeftButtonDown(false),WheelPos(0) { }

    } MouseState;

    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember the mouse state
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            switch(event.MouseInput.Event)
            {
            case EMIE_LMOUSE_PRESSED_DOWN:
                MouseState.LeftButtonDown = true;
                break;

            case EMIE_LMOUSE_LEFT_UP:
                MouseState.LeftButtonDown = false;
                break;

            case EMIE_MOUSE_MOVED:
                MouseState.Position.X = event.MouseInput.X;
                MouseState.Position.Y = event.MouseInput.Y;
                break;

            case EMIE_MOUSE_WHEEL:
                if(event.MouseInput.Wheel > 0)
                    MouseState.WheelPos++;
                else
                    MouseState.WheelPos--;
                break;

            default:
                // We won't use the wheel
                break;
            }
        }
        else if(event.EventType ==   irr::EET_KEY_INPUT_EVENT)
        {
                KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
        }

        return false;
    }
    virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

    const SMouseState & GetMouseState(void) const
    {
        return MouseState;
    }

    MyEventReceiver()
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
    }
private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};



struct Point
{
    double X;
    double Y;
};


struct Scene
{
    double x0;
    double y0;
    double x1;
    double y1;
    unsigned width;
    unsigned height;
};

inline double SceneX(double x, Scene scene)
{
return 64+(x-scene.x0)/(scene.x1-scene.x0)*scene.width;
}

inline double SceneY(double y, Scene scene)
{
return 64+(y-scene.y0)/(scene.y1-scene.y0)*scene.height;
}

int n_vertexes=0;
bool paused=false;
double time;

class LinePoly {
public:
    unsigned increment;
    double x0;
    double y0;
    double x1;
    double y1;
    LinePoly(){}
    LinePoly(double x,double y,double xx, double yy) : x0(x),y0(y),x1(xx),y1(yy) {}
    ~LinePoly(){}

    void rewind(unsigned) {increment=0;}

    void set0(double x,double y)
    {
        x0=x;
        y0=y;
    }
    void set1(double xx, double yy)
    {
        x1=xx;
        y1=yy;
    }

    unsigned vertex(double* x,double* y)
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
};

//arbitrary multiline poly
class ArbPoly {
public:
    unsigned increment;
    double point[20];
    unsigned nPoints;
    ArbPoly():nPoints(0){}
    //LinePoly(double x,double y,double xx, double yy) : x0(x),y0(y),x1(xx),y1(yy) {}
    ~ArbPoly(){}

    void rewind(unsigned) {increment=0;}
    void AddPoint(double x,double y)
    {
        point[nPoints*2]=x;
        point[nPoints*2+1]=y;
        nPoints++;
    }

    unsigned vertex(double* x,double* y)
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
};

class Marker_dot_poly {
    public:
    unsigned increment;
    static const int nPoints= 4;
    static const int radius=3.5;
    double x0;
    double y0;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }

    unsigned vertex(double* x,double* y)
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
};

class Marker_small_circle_poly {
    public:
    unsigned increment;
    static const int nPoints= 12;
    static const int radius=5;
    double x0;
    double y0;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }

    unsigned vertex(double* x,double* y)
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
};


class Marker_small_arrow_poly {
    public:
    unsigned increment;

    static const int length=10;
    double x0;
    double y0;
    double rot;

    void rewind(unsigned) {increment=0;}
    void setPosition(double x, double y)
    {
        x0=x;
        y0=y;
    }
     void setRotation(double r)
    {
        rot=r;
    }

    unsigned vertex(double* x,double* y)
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
};

class ArbPoly_solid {
public:
    unsigned increment;
    double point[20];
    unsigned nPoints;
    ArbPoly_solid():nPoints(0){}
    //LinePoly(double x,double y,double xx, double yy) : x0(x),y0(y),x1(xx),y1(yy) {}
    ~ArbPoly_solid(){}

    void rewind(unsigned) {increment=0;}
    void AddPoint(double x,double y)
    {
        if(nPoints>0 && point[(nPoints-1)*2] == x && point[(nPoints-1)*2+1] == y)
            return;

        point[nPoints*2]=x;
        point[nPoints*2+1]=y;
        nPoints++;
    }

    unsigned vertex(double* x,double* y)
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
};

/*

class ArcPoly_solid {
public:

    agg::point_d center;
    unsigned increment;
    unsigned arcInc;
    unsigned int nPoints;
    double theta[8];
    unsigned int nArcPoints[4];
    int nArcs;
    double radius;

    int cornerArc;
    int nCorners;
    double corner[8];

    ArcPoly_solid():nArcs(0),nPoints(5),nCorners(0){}
    ArcPoly_solid(agg::point_d cent,double r,int n,double t0,double t1)
        : increment(0),
        center(cent),
        radius(r),
        nPoints(n),
        nArcs(1),
        nCorners(0)
    {
        theta[0]=t0;
        theta[1]=t1;
    }
    ~ArcPoly_solid()
    {
    }
    void SwapCorners(int a,int b)
    {
            double temp = corner[a*2];
            corner[a*2]=corner[b*2];
            corner[b*2]=temp;
            temp = corner[a*2+1];
            corner[a*2+1]=corner[b*2+1];
            corner[b*2+1]=temp;
    }
    ArcPoly_solid& setNPoints(int n)
    {
        nPoints=n;
        return *this;
    }
    ArcPoly_solid& setPosition(double x,double y)
    {
        center.x=x;
        center.y=y;
        return *this;
    }
    ArcPoly_solid& setRadius(double r)
    {
        radius=r;
        return *this;
    }
    ArcPoly_solid& addArc(double t0,double t1)
    {
    if(nArcs<4)
        {
        theta[nArcs*2]=t0;
        theta[nArcs*2+1]=t1;
        nArcPoints[nArcs]=max(4,2+(int)(nPoints*(t1-t0)/(PI*2)));
        //cout<<nArcPoints[nArcs]<<"\n";
        nArcs++;
        }
    return *this;
    }

    void rewind(unsigned) {
        increment = 0;
        arcInc=0;
    }


    unsigned vertex(double* x, double* y) {
    //cout<<increment<<"\n";

        if(increment>100)
        {cout<<"run away!!\n";
        cout<<"arc "<<arcInc<<"\n";
        cout<<"corners "<<nCorners<<"\n";
        cout<<"corner arc "<<cornerArc<<"\n";
        paused=true;return agg::path_cmd_stop;
        }

       if(nCorners>0 && arcInc==cornerArc && increment>=nArcPoints[arcInc] && increment<nArcPoints[arcInc]+nCorners)
       {
           *x = corner[(increment-nArcPoints[arcInc])*2];
           *y = corner[((increment-nArcPoints[arcInc])*2)+1];
           ++increment;
           //cout<<increment<<" "<<*x<<", "<<*y<<"\n";
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
        ++increment;
        return agg::path_cmd_line_to;
       // agg::path_cmd
        }

        if ( arcInc+1<nArcs && increment >= nArcPoints[arcInc] )
        {
            increment=0;
            ++arcInc;
        }

        n_vertexes++;

        *x = center.x + radius * cos( theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1)) );
        *y = center.y + radius * sin( theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1)) );

        double t = theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1));

        if(increment ==0 && arcInc == 0)
        {
            ++increment;
            //cout<<increment<<" ("<<*x<<", "<<*y<<")\n";
            return agg::path_cmd_move_to;
        }
        //if ( increment == 0 )
        //{
          //  cout<<increment<<" "<<arcInc<<" "<<"("<<theta[arcInc*2]<<" "<<theta[arcInc*2+1]<<" "<<nArcPoints[arcInc]<<")"<<" move to "<<t<<"\n";
          //  ++increment;
          //  return agg::path_cmd_line_to;

        //}
       // cout<<increment<<" "<<*x<<", "<<*y<<"\n";

       // cout<<increment<<" "<<arcInc<<"/"<<nArcs<<"->"<<nArcPoints[arcInc]<<" "<<" line to "<<t<<"\n";
        ++increment;

        return agg::path_cmd_line_to;
    }
};
*/

class ArcPoly_solid {
public:

    agg::point_d center;
    unsigned increment;
    unsigned arcInc;
    unsigned int nPoints;
    double theta[8];
    unsigned int nArcPoints[4];
    int nArcs;
    double radius;

    int cornerArc;
    int nCorners;
    double corner[8];

    ArcPoly_solid():nArcs(0),nPoints(5),nCorners(0){}
    ArcPoly_solid(agg::point_d cent,double r,int n,double t0,double t1)
        : increment(0),
        center(cent),
        radius(r),
        nPoints(n),
        nArcs(1),
        nCorners(0)
    {
        theta[0]=t0;
        theta[1]=t1;
    }
    ~ArcPoly_solid()
    {
    }
    void SwapCorners(int a,int b)
    {
            double temp = corner[a*2];
            corner[a*2]=corner[b*2];
            corner[b*2]=temp;
            temp = corner[a*2+1];
            corner[a*2+1]=corner[b*2+1];
            corner[b*2+1]=temp;
    }
    ArcPoly_solid& setNPoints(int n)
    {
        nPoints=n;
        return *this;
    }
    ArcPoly_solid& setPosition(double x,double y)
    {
        center.x=x;
        center.y=y;
        return *this;
    }
    ArcPoly_solid& setRadius(double r)
    {
        radius=r;
        return *this;
    }
    ArcPoly_solid& addArc(double t0,double t1)
    {
    if(nArcs<4)
        {
        theta[nArcs*2]=t0;
        theta[nArcs*2+1]=t1;
        nArcPoints[nArcs]=max(4,2+(int)(nPoints*(t1-t0)/(PI*2)));
        //cout<<nArcPoints[nArcs]<<"\n";
        nArcs++;
        }
    return *this;
    }
    void AddPoint(double x,double y)
    {
        if(x==corner[(nCorners-1)*2] && y==corner[(nCorners-1)*2+1])
            return;
        corner[nCorners*2]=x;
        corner[nCorners*2+1]=y;
        nCorners++;
    }
    void InsertPoint(double x,double y)
    {
        for(int i=nCorners;i>0;i--)
        {
            corner[i*2]=corner[(i-1)*2];
            corner[i*2+1]=corner[(i-1)*2+1];
        }
        corner[0]=x;
        corner[1]=y;
        nCorners++;
    }
    void rewind(unsigned) {
        increment = 0;
        arcInc=0;
    }


    unsigned vertex(double* x, double* y) {
    //cout<<increment<<"\n";
        n_vertexes++;

        if(increment>100)
        {
            cout<<"run away!!\n";
            cout<<"arc "<<arcInc<<"\n";
            cout<<"n arcs "<<nArcs<<"\n";
            cout<<"corners "<<nCorners<<"\n";
            cout<<"corner arc "<<cornerArc<<"\n";
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
};


class ArcPoly {
public:

    agg::point_d center;
    unsigned increment;
    unsigned arcInc;
    unsigned int nPoints;
    double theta[8];
    unsigned int nArcPoints[4];
    int nArcs;
    double radius;

    ArcPoly():nArcs(0),nPoints(5){}
    ArcPoly(agg::point_d cent,double r,int n,double t0,double t1)
        : increment(0),
        center(cent),
        radius(r),
        nPoints(n),
        nArcs(1)
    {
        theta[0]=t0;
        theta[1]=t1;
    }
    ~ArcPoly()
    {
    }
    ArcPoly& setNPoints(int n)
    {
        nPoints=n;
        return *this;
    }
    ArcPoly& setPosition(double x,double y)
    {
        center.x=x;
        center.y=y;
        return *this;
    }
    ArcPoly& setRadius(double r)
    {
        radius=r;
        return *this;
    }
    ArcPoly& addArc(double t0,double t1)
    {
    if(nArcs<4)
        {
        theta[nArcs*2]=t0;
        theta[nArcs*2+1]=t1;
        nArcPoints[nArcs]=max(4,2+(int)(nPoints*(t1-t0)/(PI*2)));
        //cout<<nArcPoints[nArcs]<<"\n";
        nArcs++;
        }
    return *this;
    }

    void rewind(unsigned) {
        increment = 0;
        arcInc=0;
    }


    unsigned vertex(double* x, double* y) {

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

        double t = theta[arcInc*2] + (increment * (theta[arcInc*2+1]-theta[arcInc*2]) / (nArcPoints[arcInc]-1));

        if ( increment == 0 )
        {
          //  cout<<increment<<" "<<arcInc<<" "<<"("<<theta[arcInc*2]<<" "<<theta[arcInc*2+1]<<" "<<nArcPoints[arcInc]<<")"<<" move to "<<t<<"\n";
            ++increment;
            return agg::path_cmd_move_to;

        }

       // cout<<increment<<" "<<arcInc<<"/"<<nArcs<<"->"<<nArcPoints[arcInc]<<" "<<" line to "<<t<<"\n";
        ++increment;

        return agg::path_cmd_line_to;
    }
};

class PlotObject
{
public:
    int layer=0;
    int group=0;
    struct BoundingBox
    {
        double x0,x1,y0,y1;
    };
    BoundingBox m_BoundingBox;


    PlotObject()
    {
    }
    ~PlotObject()
    {
//        if(next !=NULL) delete next;
    }
    //virtual bool clip(Scene scene);
};

class LineObject : public PlotObject
{
public:
    double x0,y0,x1,y1;
    double m_length;
    LineObject* next;
    LineObject(double x,double y,double xx,double yy) : x0(x),y0(y),x1(xx),y1(yy),m_length(0),next(NULL)
    {
        m_BoundingBox.x0 = min(x0,x1);
        m_BoundingBox.x1 = max(x0,x1);
        m_BoundingBox.y0 = min(y0,y1);
        m_BoundingBox.y1 = max(y0,y1);

        m_length = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));

        if(x0>x1 || (x1==x0 && y1<y0))
        {
         double temp=x0;
         x0=x1;
         x1=temp;
         temp=y0;
         y0=y1;
         y1=temp;
        }

    }
    void SetNewEndpoint(double xx, double yy)
    {


        m_BoundingBox.x0 = min(x0,x1);
        m_BoundingBox.x1 = max(x0,x1);
        m_BoundingBox.y0 = min(y0,y1);
        m_BoundingBox.y1 = max(y0,y1);

        x1=xx;
        y1=yy;

        m_length = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
/*
        if(x0>x1 || (x1==x0 && y1<y0))
        {
            x0=xx;
            y0=yy;
        }
        else
        {

        }*/

    }
    ~LineObject()
    {
     if(next != NULL) delete next;
    }
    bool touchLine(double x,double y, double t)
    {
     double a = atan2(y-y0,x-x0) - angle();
     double d = sqrt((x-x0)*(x-x0)+(y-y0)*((y-y0)));
     return abs(d*sin(a))<t;
    }

    double length(){return m_length;}

    double angle()
    {
    if(x1-x0 != 0)
        return atan2(y1-y0,x1-x0);
    else return y0 < y1 ? PI/2 : -PI/2;
    }

    double iangle()
    {
    if(x1-x0 != 0)
        return atan2(y0-y1,x0-x1);
    else return y0 < y1 ? -PI/2 : PI/2;
    }

    bool getPoly(LinePoly& poly,Scene scene)
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
            poly.set0(SceneX(x0,scene),SceneY(max(y0,scene.y0),scene));
            poly.set1(SceneX(x0,scene),SceneY(min(y1,scene.y1),scene));
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
    }
};


class ArbObject : public PlotObject
{
public:
    double point[20];
    unsigned nPoints;
    ArbObject* next;

    ArbObject() : nPoints(0),next(NULL)
    {
    }

    void AddPoint(double x, double y)
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
            m_BoundingBox.x0 = min(m_BoundingBox.x0,x);
            m_BoundingBox.x1 = max(m_BoundingBox.x1,x);
            m_BoundingBox.y0 = min(m_BoundingBox.y0,y);
            m_BoundingBox.y1 = max(m_BoundingBox.y1,y);
        }
        point[nPoints*2]=x;
        point[nPoints*2+1]=y;
        nPoints++;
    }
    ~ArbObject()
    {
     if(next != NULL) delete next;
    }

    bool getPoly(ArbPoly& poly,Scene scene)
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
            if( min(xx0,xx1)>scene.x1 || max(xx0,xx1)<scene.x0 ||
               min(yy0,yy1)>scene.y1 || max(yy0,yy1)<scene.y0 )
                continue;

            if(xx1-xx0 == 0 && xx0>=scene.x0 && xx0<=scene.x1)
            {
                poly.AddPoint(SceneX(xx0,scene),
                              SceneY(max(yy0,scene.y0),scene));
                poly.AddPoint(SceneX(xx0,scene),
                              SceneY(min(yy1,scene.y1),scene));
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
    }

    bool getPoly(ArbPoly_solid& poly,Scene scene)
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
            if( min(xx0,xx1)>scene.x1 || max(xx0,xx1)<scene.x0 ||
               min(yy0,yy1)>scene.y1 || max(yy0,yy1)<scene.y0 )
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
                                  SceneY(max(point[i*2+1],scene.y0),scene));
                    poly.AddPoint(SceneX(point[i*2],scene),
                                  SceneY(min(point[(i+1)*2+1],scene.y1),scene));
                }
                else
                {
                    poly.AddPoint(SceneX(point[i*2],scene),
                                  SceneY(min(point[i*2+1],scene.y1),scene));
                    poly.AddPoint(SceneX(point[i*2],scene),
                                  SceneY(max(point[(i+1)*2+1],scene.y0),scene));
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
    const int dog[18] ={  64,64,
                        512+64,64,
                        512+64,512+64,
                        64,512+64,
                        64,64,
                        512+64,64,
                        512+64,512+64,
                        64,512+64,
                        64,64
                        };
    void addCorners(ArbPoly_solid& poly,int a,int b)
    {
        int c =b;
        if(a>=b) c+=4;
        //cout<<a<<" "<<b<<"\n";
        for(int i=a;i<c;i++)
        {
            poly.AddPoint(dog[i*2],dog[i*2+1]);
        }
    }
};

class CircleObject : public PlotObject
{
    public:
    double x0,y0,radius;
    bool bInvert=false;
    CircleObject* next;
    //ArcPoly m_poly;

    CircleObject(double x,double y,double r) : x0(x),y0(y),radius(r),next(NULL)
    {
        m_BoundingBox.x0=x0-radius;
        m_BoundingBox.x1=x0+radius;
        m_BoundingBox.y0=y0-radius;
        m_BoundingBox.y1=y0+radius;
    }

     ~CircleObject()
    {
     if(next != NULL) delete next;
    }
    bool withinRadius(double x,double y)
    {
     return (sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))<radius);
    }
    bool touchRadius(double x,double y,double t)
    {
     return (sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))<radius+t &&
             sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))>radius-t);
    }
    setRadius(double r)
    {
        radius=r;
        m_BoundingBox.x0=x0-radius;
        m_BoundingBox.x1=x0+radius;
        m_BoundingBox.y0=y0-radius;
        m_BoundingBox.y1=y0+radius;
    }
    setPosition(double x,double y)
    {
        x0=x;
        y0=y;
        m_BoundingBox.x0=x0-radius;
        m_BoundingBox.x1=x0+radius;
        m_BoundingBox.y0=y0-radius;
        m_BoundingBox.y1=y0+radius;
    }

    bool getPoly(ArcPoly_solid& poly,Scene scene)
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
                poly.corner[cc]=64;//scene.x0;
                poly.corner[cc+1]=64;//scene.y0;
                cc++;
                } else skip[0]=1;
            if(sqrt((scene.x1-x0)*(scene.x1-x0)+(scene.y0-y0)*(scene.y0-y0))<radius)
                {
                poly.corner[cc*2]=64+512;//scene.x1;
                poly.corner[cc*2+1]=64;//scene.y0;
                cc++;

                } else skip[1]=1;
            if(sqrt((scene.x1-x0)*(scene.x1-x0)+(scene.y1-y0)*(scene.y1-y0))<radius)
                {
                poly.corner[cc*2]=64+512;//scene.x1;
                poly.corner[cc*2+1]=64+512;//scene.y1;
                cc++;
                } else skip[2]=1;
            if(sqrt((scene.x0-x0)*(scene.x0-x0)+(scene.y1-y0)*(scene.y1-y0))<radius)
                {
                poly.corner[cc*2]=64;//scene.x0;
                poly.corner[cc*2+1]=64+512;//scene.y1;
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


    bool getPoly(ArcPoly& poly,Scene scene)
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
};

class MarkerObject : public PlotObject
{
    public:
    int type;
    double x0;
    double y0;
    double rot;
    MarkerObject* next;

    MarkerObject(double x,double y, int t, double dir = 0.0) : x0(x),y0(y),type(t),rot(dir)
    {
        switch(type)
        {
        case 0:
            m_BoundingBox.x0=x0-Marker_small_circle_poly::radius;
            m_BoundingBox.x1=x0+Marker_small_circle_poly::radius;
            m_BoundingBox.y0=y0-Marker_small_circle_poly::radius;
            m_BoundingBox.y1=y0+Marker_small_circle_poly::radius;
            break;
        case 1:
            m_BoundingBox.x0=x0-Marker_small_arrow_poly::length;
            m_BoundingBox.x1=x0+Marker_small_arrow_poly::length;
            m_BoundingBox.y0=y0-Marker_small_arrow_poly::length;
            m_BoundingBox.y1=y0+Marker_small_arrow_poly::length;
            break;
        }
    }

    void setPosition(double x,double y)
    {
        x0=x;
        y0=y;
        switch(type)
        {
        case 0:
            m_BoundingBox.x0=x0-Marker_small_circle_poly::radius;
            m_BoundingBox.x1=x0+Marker_small_circle_poly::radius;
            m_BoundingBox.y0=y0-Marker_small_circle_poly::radius;
            m_BoundingBox.y1=y0+Marker_small_circle_poly::radius;
            break;
        case 1:
            m_BoundingBox.x0=x0-Marker_small_arrow_poly::length;
            m_BoundingBox.x1=x0+Marker_small_arrow_poly::length;
            m_BoundingBox.y0=y0-Marker_small_arrow_poly::length;
            m_BoundingBox.y1=y0+Marker_small_arrow_poly::length;
            break;
        }
    }
    void setRotation(double r)
    {
       rot=r;
    }
    bool clip(Scene scene)
    {
        if(x0 < scene.x0 ||
            x0 > scene.x1 ||
            y0 < scene.y0 ||
            y0 > scene.y1 )
        {
            return false;
        }
        return true;
    }
};


class LabelObject : public PlotObject
{
    public:
    int type;
    double x0;
    double y0;
    char text[12]="test";
    LabelObject* next;

    LabelObject(double x,double y) : x0(x),y0(y)
    {
    }

    void setPosition(double x,double y)
    {
        x0=x;
        y0=y;
    }

    void setText(std::string str)
    {
        strcpy(text,str.c_str());
    }
};

class AngleObject : public PlotObject
{
    public:
    double x0,y0,radius;
    bool bInvert=false;
    AngleObject* next;
    //ArcPoly m_poly;

    double theta0=0;
    double theta1=PI*2;

    AngleObject(double x,double y,double r,double t0,double t1) : x0(x),y0(y),radius(r),theta0(t0),theta1(t1),next(NULL)
    {
        //cout<<t0<<" "<<t1<<"\n";
       /* if(( t0 < PI && t1 > PI) || (t0 < -PI && t1 > -PI) || (t0 < PI*3 && t1 > PI*3))
            m_BoundingBox.x0=x0-radius;
        else
            m_BoundingBox.x0=min(x0,min(x0+cos(t0)*radius,x0+cos(t1)*radius));

        if(( t0 < 0 && t1 > 0) || (t0 < PI*2 && t1 > PI*2))
            m_BoundingBox.x1=x0+radius;
        else
            m_BoundingBox.x1=max(x0,max(x0+cos(t0)*radius,x0+cos(t1)*radius));

        if(( t0 < -PI/2 && t1 > -PI/2) || ( t0 < PI*3/2 && t1 > PI*3/2))
            m_BoundingBox.y0=y0-radius;
        else
            m_BoundingBox.y0=min(y0,min(y0+sin(t0)*radius,y0+sin(t1)*radius));

        if(( t0 < PI/2 && t1 > PI/2) || ( t0 < PI*5/2 && t1 > PI*5/2))
            m_BoundingBox.y1=y0+radius;
        else
            m_BoundingBox.y1=max(y0,max(y0+sin(t0)*radius,y0+sin(t1)*radius));*/
        RecalcBoundingBox();
    }
    void SetPosition(double x,double y)
    {
     x0=x;
     y0=y;
     RecalcBoundingBox();
    }
    void RecalcBoundingBox()
    {
        if(( theta0 < PI && theta1 > PI) || (theta0 < -PI && theta1 > -PI) || (theta0 < PI*3 && theta1 > PI*3))
            m_BoundingBox.x0=x0-radius;
        else
            m_BoundingBox.x0=min(x0,min(x0+cos(theta0)*radius,x0+cos(theta1)*radius));

        if(( theta0 < 0 && theta1 > 0) || (theta0 < PI*2 && theta1 > PI*2))
            m_BoundingBox.x1=x0+radius;
        else
            m_BoundingBox.x1=max(x0,max(x0+cos(theta0)*radius,x0+cos(theta1)*radius));

        if(( theta0 < -PI/2 && theta1 > -PI/2) || ( theta0 < PI*3/2 && theta1 > PI*3/2))
            m_BoundingBox.y0=y0-radius;
        else
            m_BoundingBox.y0=min(y0,min(y0+sin(theta0)*radius,y0+sin(theta1)*radius));

        if(( theta0 < PI/2 && theta1 > PI/2) || ( theta0 < PI*5/2 && theta1 > PI*5/2))
            m_BoundingBox.y1=y0+radius;
        else
            m_BoundingBox.y1=max(y0,max(y0+sin(theta0)*radius,y0+sin(theta1)*radius));
    }
    void SetNewAngle(double r,double t0,double t1)
    {
        radius=r;
        theta0=t0;
        theta1=t1;
        RecalcBoundingBox();
    }

     ~AngleObject()
    {
     if(next != NULL) delete next;
    }
/*
    bool getPoly(ArcPoly& poly,Scene scene)
    {
        poly.nArcs=0;
        poly.setPosition(SceneX(x0,scene),64+(y0-scene.y0)/(scene.y1-scene.y0)*scene.width).setRadius(radius/((scene.x1-scene.x0)/scene.width)).setNPoints(14+radius/(3*(scene.x1-scene.x0)/scene.width));

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
    bool getPoly_solid(ArcPoly_solid& poly,Scene scene)
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
                  //  poly.AddPoint(64+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              64+(corner[j*2+1]-scene.y0)/(scene.y1-scene.y0)*scene.width);

                  //  poly.AddPoint(64+(corner[(j+1)*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              64+(corner[(j+1)*2+1]-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    continue;
                }*/
                //cout<<"x "<<j<<"\n";

                //local bounding box check
                if( min(xx0,xx1)>scene.x1 || max(xx0,xx1)<scene.x0 ||
                   min(yy0,yy1)>scene.y1 || max(yy0,yy1)<scene.y0 )
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
                    poly.AddPoint(64+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              64+(max(corner[j*2+1],scene.y0)-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    poly.AddPoint(64+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              64+(min(corner[(j+1)*2+1],scene.y1)-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    }
                    else
                    {
                    poly.AddPoint(64+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              64+(min(corner[j*2+1],scene.y1)-scene.y0)/(scene.y1-scene.y0)*scene.width);
                    poly.AddPoint(64+(corner[j*2]-scene.x0)/(scene.x1-scene.x0)*scene.width,
                              64+(max(corner[(j+1)*2+1],scene.y0)-scene.y0)/(scene.y1-scene.y0)*scene.width);
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
                //cout<<"first: "<<first<<"\n";
                //cout<<"last: "<<last<<"\n";
                //cout<<"status: "<<status<<"\n";
                //cout<<"entry: "<<entry<<"\n";
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

    const int dog[18] ={  64,64,
                        512+64,64,
                        512+64,512+64,
                        64,512+64,
                        64,64,
                        512+64,64,
                        512+64,512+64,
                        64,512+64,
                        64,64
                        };
    void addCorners(ArcPoly_solid& poly,int a,int b,bool insert=false)
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
};

class FontDraw
{
    typedef agg::font_engine_win32_tt_int32 font_engine_type;
    typedef agg::font_cache_manager<font_engine_type> font_manager_type;

    font_engine_type             m_feng;
    font_manager_type            m_fman;

    // Pipeline to process the vectors glyph paths (curves + contour)
    typedef agg::conv_curve<font_manager_type::path_adaptor_type> conv_curve_type;
    typedef agg::conv_contour<conv_curve_type> conv_contour_type;

    conv_curve_type m_curves;
    conv_contour_type m_contour;

    int    m_rentype=0;
    double m_weight=6;
    double m_height=14;
    double m_width=20;



public:

    FontDraw(HDC dc) : m_feng(dc),
        m_fman(m_feng),
        m_curves(m_fman.path_adaptor()),
        m_contour(m_curves)
        {}

 template<class Rasterizer, class Scanline, class RenSolid, class RenBin>
    unsigned draw_text(LabelObject* labels,Scene scene,Rasterizer& ras, Scanline& sl,
                       RenSolid& ren_solid, RenBin& ren_bin)
    {
        agg::glyph_rendering gren = agg::glyph_ren_native_mono;

/*
        switch(m_rentype)
        {
        case 0: gren = agg::glyph_ren_native_mono;  break;
        case 1: gren = agg::glyph_ren_native_gray8; break;
        case 2: gren = agg::glyph_ren_outline;      break;
        case 3: gren = agg::glyph_ren_agg_mono;     break;
        case 4: gren = agg::glyph_ren_agg_gray8;    break;
        }*/

        unsigned num_glyphs = 0;

        m_contour.width(-m_weight * m_height * 0.05);

        //m_feng.hinting(m_hinting.status());
        m_feng.height(m_height+2);

        // Font width in Windows is strange. MSDN says,
        // "specifies the average width", but there's no clue what
        // this "average width" means. It'd be logical to specify
        // the width with regard to the font height, like it's done in
        // FreeType. That is, width == height should mean the "natural",
        // not distorted glyphs. In Windows you have to specify
        // the absolute width, which is very stupid and hard to use
        // in practice.
        //-------------------------
        m_feng.width(((m_width) == m_height) ? 0.0 : m_width / 2.4);
       // m_feng.italic(true);
        m_feng.flip_y(true);

        agg::trans_affine mtx;
        //mtx *= agg::trans_affine_skewing(-0.3, 0);

       // mtx *= agg::trans_affine_rotation(agg::deg2rad(45.0));
        //mtx *= agg::trans_affine_translation(10,10);

        m_feng.transform(mtx);

        ren_bin.color(agg::rgba8(0,0,0,255));
        //ren_bin.color(agg::rgba(0.6*0.8,0.6*0.8,0.85*0.8));
        gren = agg::glyph_ren_native_mono;
        m_contour.width(-4 * m_height * 0.05);
        double yoff=1;
        for(int j=0;j<2;j++)
        {

            if(j==1)
            {
                yoff=0;
                m_feng.height(m_height);
                m_feng.width(((m_width) == m_height) ? 0.0 : m_width / 2.4);
            switch(m_rentype)
                {
                case 0: gren = agg::glyph_ren_native_mono;  break;
                case 1: gren = agg::glyph_ren_native_gray8; break;
                case 2: gren = agg::glyph_ren_outline;      break;
                case 3: gren = agg::glyph_ren_agg_mono;     break;
                case 4: gren = agg::glyph_ren_agg_gray8;    break;
                }
            m_contour.width(-m_weight * m_height * 0.05);
            ren_bin.color(agg::rgba(0.6*0.8,0.6*0.8,0.85*0.8,1));
            }
            if(m_feng.create_font("Courier New", gren))
            {
            m_fman.precache(' ', 127);
            for(LabelObject* label = labels; label != NULL; label = label->next)
            {
                double x = SceneX(label->x0,scene)+yoff;//10.0+i*100;
                double y = SceneY(label->y0,scene)+yoff;//10.0+i*100;//60/*height()*/ - m_height - 10.0;

                if(x<64||x>512+64||y<64||y>512+64)
                    continue;
                //double y = y0;
                    const char* p = label->text;
                    while(*p)
                    {
                        const agg::glyph_cache* glyph = m_fman.glyph(*p);
                        if(glyph)
                        {
                          /*  if(m_kerning.status())
                            {
                                m_fman.add_kerning(&x, &y);
                            }*/
    /*
                            if(x >= 512width() - m_height)
                            {
                                x = 10.0;
                                y0 -= m_height;
                                if(y0 <= 120) break;
                                y = y0;
                            }
    */
                            m_fman.init_embedded_adaptors(glyph, x, y);

                            switch(glyph->data_type)
                            {
                            case agg::glyph_data_mono:
                                //ren_bin.color(agg::rgba(0.6*0.8,0.6*0.8,0.85*0.8));
                                agg::render_scanlines(m_fman.mono_adaptor(),
                                                      m_fman.mono_scanline(),
                                                      ren_bin);
                                break;

                            case agg::glyph_data_gray8:
                                //ren_solid.color(/*agg::rgba8(255, 255, 255)*/agg::rgba(0.6*0.8,0.6*0.8,0.85*0.8));
                                agg::render_scanlines(m_fman.gray8_adaptor(),
                                                      m_fman.gray8_scanline(),
                                                      ren_solid);
                                break;

                            case agg::glyph_data_outline:
                                ras.reset();
                                if(fabs(m_weight) <= 0.01)
                                {
                                    // For the sake of efficiency skip the
                                    // contour converter if the weight is about zero.
                                    //-----------------------
                                    ras.add_path(m_curves);
                                }
                                else
                                {
                                    ras.add_path(m_contour);
                                }
                                //ren_solid.color(agg::rgba(0.6*0.8,0.6*0.8,0.85*0.8));
                                agg::render_scanlines(ras, sl, ren_solid);
                                break;
                            }

                            // increment pen position
                            x += glyph->advance_x;
                            y += glyph->advance_y;
                            ++num_glyphs;
                        }
                        ++p;
                    }
                }//for label
            }//if
            else cout<<"no font\n";
        }//for j
        return num_glyphs;
    }

};

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

    Plot():circles(NULL),lines(NULL),angles(NULL),markers(NULL),labels(NULL){}
    ~Plot()
    {
     if(circles) delete circles;
     if(lines) delete lines;
     if(angles) delete angles;
     if(markers) delete markers;
     if(labels) delete labels;
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

    void addCircle(double x,double y,double r)
    {
    CircleObject* n = new CircleObject(x,y,r);

    n->next = circles;
    n->group=object_group;
    circles = n;

    }
    void addLabel(double x,double y)
    {
    LabelObject* n = new LabelObject(x,y);

    n->next = labels;
    n->group=object_group;
    labels = n;

    }
    void addMarker(double x,double y,int type, double rot=0.0)
    {
    MarkerObject* n = new MarkerObject(x,y,type,rot);

    n->next = markers;
    n->group=object_group;
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
};


bool renderImage(irr::video::IImage* pImage,irr::video::ITexture* imgtex,Plot& plot,FontDraw& myFont,Scene scene,bool makeBMP = false)
{
    if (!pImage)
        return false;

    // Create AGG stuff, setting the Irrlicht image->getData() as the buffer

    // Yes, it's technically u32 data, but Anti-Grain treats it as pixels composed of 1 byte/8 bit colors.
    irr::u8* imgDataPtr = (irr::u8*) pImage->lock();
    irr::core::dimension2du imgSize = pImage->getDimension();

    //ArcPoly poly(agg::point_d( (wx-scene.x0)/(scene.x1-scene.x0)*512,(wy-scene.y0)/(scene.y1-scene.y0)*512),50/s,PI*2*50/s/5,PI,-PI);//50/s,PI*2*50/s/10
    //ArcPoly poly;
    //poly.setPosition((0-scene.x0)/(scene.x1-scene.x0)*512,(0-scene.y0)/(scene.y1-scene.y0)*512).setNPoints(30).setRadius(50/((scene.x1-scene.x0) / 512));
    //double z=PI*1/8;
    //poly.addArc(-PI,0);
    //poly.addArc(PI/3,PI*2/3);

    agg::rendering_buffer renderingBuffer;
    renderingBuffer.attach(imgDataPtr, imgSize.Width, imgSize.Height, pImage->getPitch());
    // Alternatively:
    //agg::rendering_buffer renderingBuffer( ... same args as passed to .attach() );

    agg::pixfmt_argb32 pixelFormat(renderingBuffer);
    agg::renderer_base<agg::pixfmt_argb32> rendererBase(pixelFormat);
    agg::scanline_p8 scanLine;
    agg::rasterizer_scanline_aa<> ras;
    agg::renderer_primitives<agg::pixfmt_argb32> prim(pixelFormat);

    typedef agg::renderer_scanline_aa_solid<agg::renderer_base<agg::pixfmt_argb32> > renderer_solid;
    typedef agg::renderer_scanline_bin_solid<agg::renderer_base<agg::pixfmt_argb32> > renderer_bin;

    renderer_solid ren_solid(rendererBase);
    renderer_bin ren_bin(rendererBase);

    //cout<<renderingBuffer.height()<<"   "<<renderingBuffer.width()<<"\n";
    renderingBuffer.clear(0);

    agg::path_storage ps;
    agg::conv_stroke<agg::path_storage> pg(ps);

    agg::path_storage ps2;
    agg::conv_stroke<agg::path_storage> pg2(ps2);

    LinePoly gpoly;
    if(plot.gridspacing > 0)
    {
        for(int g=plot.gridspacing*(int)(scene.x0/plot.gridspacing);g<scene.x1;g+=plot.gridspacing)
        {
            //cout<<g<<"\n";
            gpoly.set0(SceneX(g,scene),64);
            gpoly.set1(SceneX(g,scene),64+512);
            ps.concat_path(gpoly);
        }
        for(int g=plot.gridspacing*(int)(scene.y0/plot.gridspacing);g<scene.y1;g+=plot.gridspacing)
        {
            //cout<<g<<"\n";
            gpoly.set0(64,SceneY(g,scene));
            gpoly.set1(64+512,SceneY(g,scene));
            ps.concat_path(gpoly);
        }
        ras.add_path(pg);
        pg.width(0.5);
        agg::render_scanlines_bin_solid(ras, scanLine, rendererBase, agg::rgba(0.3,0.3,0.7,0.5));

        ps.remove_all();
    }
    ras.reset();

    ArcPoly_solid apoly_s;
    ArcPoly apoly;
    for(CircleObject* circle=plot.circles;circle !=NULL;circle=circle->next)
        {
        if(circle->getPoly(apoly,scene))
            {
            switch(circle->layer)
                {
                case 0:
                    ps.concat_path(apoly);
                    break;
                case 1:
                    ps2.concat_path(apoly);
                    break;
                }
            }
            //ras.add_path(apoly_s);
        //if(circle->getPoly(apoly,scene))
        //    ps.concat_path(apoly);
        }
    for(AngleObject* Angle=plot.angles;Angle !=NULL;Angle=Angle->next)
        {
        if(Angle->getPoly_solid(apoly_s,scene))
            {
           ras.add_path(apoly_s);
        //if(Angle->getPoly(apoly,scene))
        //    ps.concat_path(apoly_s);
            }
           // ps.end_poly();
        }
    LinePoly lpoly;
    for(LineObject* line=plot.lines;line !=NULL;line=line->next)
        {
        if(line->getPoly(lpoly,scene))
            {
            switch(line->layer)
                {
                case 0:
                    ps.concat_path(lpoly);
                    break;
                case 1:
                    ps2.concat_path(lpoly);
                    break;
                }
            }
        }
    ArbObject arb;

    arb.AddPoint(25,-25);
    arb.AddPoint(-80,150);
    arb.AddPoint(-25,-25);

    ArbObject arb2;/*
    arb2.AddPoint(0,-50);
    arb2.AddPoint(50,0);
    arb2.AddPoint(0,50);
    arb2.AddPoint(-50,0);*/

    ArbPoly_solid ap_s;
    ArbPoly ap;
    //if(arb.getPoly(ap_s,scene))
    //    ps.concat_path(ap_s);

    //if(arb.getPoly(ap,scene))
    //    ps.concat_path(ap);
    //if(arb.getPoly(ap_s,scene))
    //{
    //    ps.concat_path(ap_s);
    //}
    // if(arb2.getPoly(ap,scene))
       // ps.concat_path(ap);
    //if(arb2.getPoly(ap_s,scene))
    {
    //    ras.add_path(ap_s);
    }

    //AngleObject ang_ob(0,0,50,-PI/3+time/1000,PI/3+time/1000);

    pg.width(1.5);
    pg2.width(1.5);

    //agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.5,0.6,0.9));
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.5,0.2,0.85,0.4+sin(time/1000)*0.08));
    ras.reset();
    ras.add_path(pg);

    Marker_small_circle_poly marker_sc_poly;
    Marker_small_arrow_poly marker_sa_poly;
    Marker_dot_poly marker_d_poly;
    //prim.l
    for(MarkerObject* m = plot.markers; m!=NULL; m=m->next)
    {
        if(m->clip(scene))
        {
            switch( m->type)
            {
                case 0:
                    marker_sc_poly.setPosition(SceneX(m->x0,scene),SceneY(m->y0,scene));
                    //ps.concat_path(marker_sc_poly);
                    switch(m->layer)
                        {
                        case 0:
                            ps.concat_path(marker_sc_poly);
                            break;
                        case 1:
                            ps2.concat_path(marker_sc_poly);
                            break;
                        }
                    break;
                case 1:
                    marker_sa_poly.setPosition(SceneX(m->x0,scene),SceneY(m->y0,scene));
                    marker_sa_poly.setRotation(m->rot);
                    switch(m->layer)
                        {
                        case 0:
                            ps.concat_path(marker_sa_poly);
                            break;
                        case 1:
                            ps2.concat_path(marker_sa_poly);
                            break;
                        }
                    break;
                case 2:
                    marker_d_poly.setPosition(SceneX(m->x0,scene),SceneY(m->y0,scene));
                    switch(m->layer)
                        {
                        case 0:
                            ras.add_path(marker_d_poly);
                            //prim.line_color(agg::rgba(0.6,0.6,0.85,0.8));
                            //prim.ellipse(SceneX(m->x0,scene),SceneY(m->y0,scene),3,3);
                            break;
                        case 1:
                            ras.add_path(marker_d_poly);
                            //prim.line_color(agg::rgba(1,1,0.8,1));
                            //prim.ellipse(SceneX(m->x0,scene),SceneY(m->y0,scene),3,3);
                            break;
                        }
                    break;
            }
        }
        else{
        }
    }
    ras.add_path(pg);
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.6,0.6,0.85,0.8));
    ras.reset();

    ras.add_path(pg2);
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(1,1,0.8,1));

    myFont.draw_text(plot.labels,scene,ras, scanLine, ren_solid, ren_bin);
/*
    ps.remove_all();

    circle.bInvert=true;
    circle2.bInvert=true;
    circle3.bInvert=true;
    circle4.bInvert=true;

    if(circle.getPoly(poly,scene))
        ps.concat_path(poly);
    if(circle2.getPoly(poly,scene))
        ps.concat_path(poly);
    if(circle3.getPoly(poly,scene))
        ps.concat_path(poly);
    if(circle4.getPoly(poly,scene))
        ps.concat_path(poly);

    pg.width(2);

    ras.add_path(pg);
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.4,0.1,0.1,0.5));
*/
    if(makeBMP) generateBitmapImage(imgDataPtr,imgSize.Width,imgSize.Height,"bmpTest.bmp");

    // memcopy results directly to our texture
    //---------------------------
    if(imgtex != NULL)
    {
        irr::core::dimension2d<u32> tDim = imgtex->getSize();
        irr::core::dimension2d<u32> iDim = pImage->getDimension();

        u32 imgPitch = pImage->getBytesPerPixel() * iDim.Width;
        u32 txtPitch = imgtex->getPitch();

        char* txtPtr = (char*)imgtex->lock(irr::video::ETLM_WRITE_ONLY);
        //char* imgPtr = (char*)img->lock();
        for(u32 c = 0; c < iDim.Height; c++)
        {
            memcpy(txtPtr, imgDataPtr, imgPitch);
            txtPtr += txtPitch;
            imgDataPtr += imgPitch;
        }
        imgtex->unlock();
    }
    pImage->unlock();
    return true;
}


int main() {
    irr::core::dimension2du screenSize(640,640);
    irr::video::SColor white(0xffffffff);
    irr::core::vector2di zeroVector(0);

    MyEventReceiver receiver;

    irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_BURNINGSVIDEO,screenSize,16,false,false,false,&receiver);

    if ( !device ) return 1;

    irr::video::IVideoDriver* videoDriver = device->getVideoDriver();

    videoDriver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
    videoDriver->setTextureCreationFlag(irr::video::ETCF_ALLOW_NON_POWER_2, true);

    irr::core::recti srcRect(0,0,(irr::s32)screenSize.Width,(irr::s32)screenSize.Height);

    irr::video::IImage* img = videoDriver->createImage(irr::video::ECF_A8R8G8B8, screenSize);

    double d = 3421.244;
    std::string astring = patch::to_string(d);
//    Draw(img);
    irr::video::ITexture* tex = videoDriver->addTexture(irr::io::path("image name"), img);
    bool bMouseDown=false;
    bool fDown=false;
    bool pDown=false;
    bool aDown=false;
    bool zDown=false;
    Scene scene = {-256,-256,256,256,512,512};
    int MouseWheelPos = receiver.GetMouseState().WheelPos;
    double mousex;
    double mousey;
    double toolx;
    double tooly;
    srand(43242);
    Plot myPlot;
    for(int i=0;i<5;i++)
    {
    //myPlot.addCircle(rand()%1000-500,rand()%1000-500,rand()%200);
    //myPlot.addLine(rand()%1000-500,rand()%1000-500,rand()%1000-500,rand()%1000-500);
    }

    //myPlot.addCircle(0,0,50);
    //myPlot.addCircle(20,35,75);
    //double t0=PI*3/4;
    //double t1=PI*5/4;
    //double ra=50;
double    z=0;
    //myPlot.addAngle(0,0,50, 4.2725,5.0579);
    //myPlot.addAngle(0,0,50, 4.2725,5.0579);
    //myPlot.addAngle(0,0,50, 4.2725,5.0579);
    //myPlot.addAngle(0,0,50, 4.2725,5.0579);
    //myPlot.addAngle(0,0,50,-PI/8+PI*1/4,PI/8+PI*1/4);
    //myPlot.addAngle(0,0,50,-PI/8+PI*3/4,PI/8+PI*3/4);
    //myPlot.addAngle(0,0,50,-PI/8+PI*5/4,PI/8+PI*5/4);
    //myPlot.addAngle(0,0,50,PI/4,PI*3/4);
    //myPlot.addAngle(0,0,50,PI*3/4,PI*5/4);
   // myPlot.addAngle(0,0,50,-PI*3/4,-PI*1/4);
    //myPlot.addAngle(0,0,50,PI*3/4,PI/4);
   //myPlot.addCircle(0,0,50);
    /*cout<<"x0: "<<myPlot.angles->m_BoundingBox.x0<<"\n";
    cout<<"x1: "<<myPlot.angles->m_BoundingBox.x1<<"\n";
    cout<<"y0: "<<myPlot.angles->m_BoundingBox.y0<<"\n";
    cout<<"y1: "<<myPlot.angles->m_BoundingBox.y1<<"\n";*/
    myPlot.gridspacing=128;

    HDC dc = ::GetDC(0);
    FontDraw myFont(dc);

    int tool_status=0;
    int tool_number=0;

    double oldtoolx;
    double oldtooly;
    double clickx;
    double clicky;
    double myangle=0;
    double lasttime=0;
    while ( device->run() ) {

        //mousex=receiver.GetMouseState().Position.X;
        //mousey=receiver.GetMouseState().Position.Y;

        myangle+=(time-lasttime)/2000;
        lasttime=time;
        if(myangle>PI*2) myangle=0;
       // if(!paused)
       //     cout<<"angles = "<<myangle<<"  "<<myangle+PI/4<<"\n";
        //myPlot.angles->SetNewAngle(50,myangle,PI/4+myangle);
        //myPlot.angles->next->SetNewAngle(50,myangle+PI/2,PI/4+myangle+PI/2);
        //myPlot.angles->next->next->SetNewAngle(50,myangle-PI/2,PI/4+myangle-PI/2);
        //myPlot.angles->next->next->next->SetNewAngle(50,myangle-PI,PI/4+myangle-PI);
        toolx = ((((double)receiver.GetMouseState().Position.X-64)/scene.width)*(scene.x1-scene.x0))+scene.x0;
        tooly = ((((double)receiver.GetMouseState().Position.Y-64)/scene.width)*(scene.y1-scene.y0))+scene.y0;
        if(receiver.GetMouseState().LeftButtonDown == true)
        {
            if(bMouseDown==false)
            {
            mousex=receiver.GetMouseState().Position.X;
            mousey=receiver.GetMouseState().Position.Y;
            clickx=mousex;
            clicky=mousey;
            oldtoolx=toolx;
            oldtooly=tooly;
           // toolx = ((((double)receiver.GetMouseState().Position.X-64)/scene.width)*(scene.x1-scene.x0))+scene.x0;
           // tooly = ((((double)receiver.GetMouseState().Position.Y-64)/scene.width)*(scene.y1-scene.y0))+scene.y0;

            //cout<<"click: "<<toolx<<", "<<tooly<<"\n";

            }
            else
            {

                if(abs(mousex-receiver.GetMouseState().Position.X) > 0.001 || abs(mousey-receiver.GetMouseState().Position.Y) > 0.001)
                {
                    double s = (scene.x1-scene.x0)/scene.width;
                    scene.x0 += (mousex-receiver.GetMouseState().Position.X)*s;
                    scene.y0 += (mousey-receiver.GetMouseState().Position.Y)*s;
                    scene.x1 += (mousex-receiver.GetMouseState().Position.X)*s;
                    scene.y1 += (mousey-receiver.GetMouseState().Position.Y)*s;
                    //cout<<"scene: "<<" "<<scene.x0<<","<<scene.y0<<","<<scene.x1<<","<<scene.y1<<"\n";
                   //cout<<mousex-receiver.GetMouseState().Position.X<<","<<mousey-receiver.GetMouseState().Position.Y<<"\n";
                    mousex=receiver.GetMouseState().Position.X;
                    mousey=receiver.GetMouseState().Position.Y;
                    //toolx = ((mousex/scene.width)*(scene.x1-scene.x0))+scene.x0;
                    //tooly = ((mousey/scene.width)*(scene.y1-scene.y0))+scene.y0;
                    cout<<"vertexes: "<<n_vertexes<<"\n";
                }
            }
            bMouseDown=true;
        }
        else if (receiver.GetMouseState().LeftButtonDown == false)
        {
            if(bMouseDown==true)
            {
                if(mousex-clickx == 0 &&
                    mousey-clicky == 0)
                {
                    if(tool_number==0)
                    {
                        if(tool_status == 0)
                        {
                            tool_status=1;
                            myPlot.newObjectGroup();
                            myPlot.addLine(toolx,tooly,toolx,tooly);
                            myPlot.lines->layer=1;

                            myPlot.addMarker(toolx,tooly,2);
                            myPlot.addMarker(toolx,tooly,1);
                            myPlot.addLabel(toolx,tooly);
                            myPlot.markers->layer=1;
                            myPlot.markers->next->layer=1;

                            myPlot.newObjectGroup();
                            myPlot.addCircle(toolx,tooly,0);

                            myPlot.circles->layer=1;
                        }
                        else if(tool_status == 1)
                        {
                            tool_status=0;
                            myPlot.lines->layer=0;
                            myPlot.markers->layer=0;
                            myPlot.markers->next->layer=0;
                            myPlot.circles->layer=0;
                        }
                    }
                    else if(tool_number==1)
                    {
                        if(tool_status == 0)
                        {
                            myPlot.newObjectGroup();
                            tool_status=1;
                            myPlot.addLine(toolx,tooly,toolx,tooly);
                        }
                        else if(tool_status == 1)
                        {
                            tool_status=2;
                           // double r = myPlot.Lines

                            myPlot.addLine(toolx,tooly,toolx,tooly);
                            myPlot.addAngle(toolx,tooly,0,0,0);
                        }
                        else if(tool_status == 2)
                        {
                            tool_status=0;
                        }
                    }
                    else if(tool_number==2)
                    {
                        if(tool_status == 0)
                        {
                            tool_status=1;
                            myPlot.newObjectGroup();
                            myPlot.addLine(toolx,tooly,toolx,tooly);
                            myPlot.addLabel(toolx,tooly);
                            myPlot.addMarker(toolx,tooly,2);
                            myPlot.addMarker(toolx,tooly,2);
                        }
                        else if(tool_status == 1)
                        {
                            tool_status=0;
                            myPlot.lines->layer=0;
                            //myPlot.markers->layer=0;
                            //myPlot.markers->next->layer=0;
                        }
                    }
                    else if(tool_number==4)
                    {
                        if(tool_status == 0)
                        {
                            tool_status=1;
                            myPlot.newObjectGroup();
                            //myPlot.addLine(toolx,tooly,toolx,tooly);

                           // double r = myPlot.Lines

                            myPlot.addLine(toolx,tooly,toolx,tooly);
                            myPlot.addAngle(toolx,tooly,0,0,0);
                            myPlot.addLabel(toolx,tooly);

                            myPlot.addMarker(toolx,tooly,1);
                            myPlot.markers->layer=1;
                        }
                        else if(tool_status == 1)
                        {
                            tool_status=0;
                            myPlot.lines->layer=0;
                            myPlot.markers->layer=0;

                            AngleObject* a = myPlot.angles;
                            myPlot.angles = a->next;
                            delete a;
                        }
                    }//else if
                }

/*
                //  ///DEBUGGING
                int wx = 0;
                int wy = 0;
                double theta0=-PI/2;
                double theta1=PI*3/2;
                double s = (scene.x1-scene.x0)/scene.width;

                //ArcPoly poly(agg::point_d( (wx-scene.x0)/(scene.x1-scene.x0)*512,(wy-scene.y0)/(scene.y1-scene.y0)*512),50/s,PI*2*50/s/5,PI,-PI);//50/s,PI*2*50/s/10

                //if(poly.clip(scene))
                //    cout<<"true\n";
                //else
                //    cout<<"false\n";
                //cout<<"scene: "<<" "<<scene.x0<<","<<scene.y0<<","<<scene.x1<<","<<scene.y1<<"\n";

                int r=50;
                double t,t2,x,x2,y,y2;
                double theta[11];    //last point = first point + 2 points for "angle limits"
                int i=0;
                if((wy+r)>scene.y0 && (wy-r)<scene.y0)
                {
                    t= asin((scene.y0-wy)/r);
                    t2= PI-t;
                    x=cos(t)*r;
                    x2=cos(t2)*r;
                    if(wx+x>scene.x0 && wx+x<scene.x1)
                    {
                        theta[i]=t;
                        i++;
                    }
                    if(wx+x2>scene.x0 && wx+x2<scene.x1)
                    {
                        theta[i]=t2;
                        i++;
                    }
                    //cout<<"y0: "<<t*180/PI<<" ("<<x<<"),"<<t2*180/PI<<" ("<<x2<<")"<<"\n";

                }
                if((wy+r)>scene.y1 && (wy-r)<scene.y1)
                {
                    t= asin((scene.y1-wy)/r);
                    t2=PI-t;
                    x=cos(t)*r;
                    x2=cos(t2)*r;
                    if(wx+x>scene.x0 && wx+x<scene.x1)
                    {
                        theta[i]=t;
                        i++;
                    }
                    if(wx+x2>scene.x0 && wx+x2<scene.x1)
                    {
                        theta[i]=t2;
                        i++;
                    }
                    //cout<<"y1: "<<t*180/PI<<" ("<<x<<"),"<<t2*180/PI<<" ("<<x2<<")"<<"\n";
                }
                 if((wx+r)>scene.x0 && (wx-r)<scene.x0)
                {
                    t= acos((scene.x0-wx)/r);
                    t2=PI*2-t;
                    y=sin(t)*r;
                    y2=sin(t2)*r;
                    if(wy+y>scene.y0 && wy+y<scene.y1)
                    {
                        theta[i]=t;
                        i++;
                    }
                    if(wy+y2>scene.y0 && wy+y2<scene.y1)
                    {
                        theta[i]=t2;
                        i++;
                    }
                    //cout<<"x0: "<<t*180/PI<<" ("<<y<<"),"<<t2*180/PI<<" ("<<y2<<")"<<"\n";
                }
                 if((wx+r)>scene.x1 && (wx-r)<scene.x1)
                {
                    t= -acos((scene.x1-wx)/r);
                    t2=-t;
                    y=sin(t)*r;
                    y2=sin(t2)*r;
                    if(wy+y>scene.y0 && wy+y<scene.y1)
                    {
                        theta[i]=t;
                        i++;
                    }
                    if(wy+y2>scene.y0 && wy+y2<scene.y1)
                    {
                        theta[i]=t2;
                        i++;
                    }
                    //cout<<"x1: "<<t*180/PI<<" ("<<y<<"),"<<t2*180/PI<<" ("<<y2<<")"<<"\n";
                }
                cout<<i<<" intersection points\n";
                //if(i>0)
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
                    //cout<<"\n";
                    for(int j=0;j<i;j++)
                        cout<<theta[j]*180/PI<<" ";
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

                    //cout<<c<<" switches: ";
                    //theta[i] = PI*2+theta[0];
                    cout<<"\n";
                    for(int j=0;j<i;j++)
                        cout<<theta[j]*180/PI<<" ";

                    bool ok=false;
                    double theta_test = theta[0]+(theta[1]-theta[0])/2;
                    if(wx+r*cos(theta_test) > scene.x0 &&
                       wx+r*cos(theta_test) < scene.x1 &&
                       wy+r*sin(theta_test) > scene.y0 &&
                       wy+r*sin(theta_test) < scene.y1 )
                        ok=true;
                        cout<<"\n"<<i-1<<"\n";
                    for(int j=0;j<i-1;j++)
                    {
                        if(ok)
                            cout<<theta[j]*180/PI<<" to "<<theta[j+1]*180/PI<<"\n";
                        ok=!ok;
                    }
                    cout<<"\n";

                }  ///DEBUGGING!
                //cout<<"to: "<<receiver.GetMouseState().Position.X<<", "<<receiver.GetMouseState().Position.Y<<"\n";
*/
            }//if mousedown = true
            else //mousedown = false
            {
                switch(tool_number)
                {
                case 0:
                    if(tool_status==1)
                        {
                        myPlot.lines->SetNewEndpoint(toolx,tooly);
                        myPlot.markers->setPosition(toolx,tooly);
                        myPlot.markers->setRotation(myPlot.lines->angle());
                        myPlot.labels->setPosition(myPlot.lines->x0+myPlot.lines->length()/2*cos(myPlot.lines->angle()),
                                                  myPlot.lines->y0+myPlot.lines->length()/2*sin(myPlot.lines->angle()));
                        myPlot.labels->setText(patch::to_string(myPlot.lines->length()));
                        myPlot.circles->setRadius(myPlot.lines->length());
                        }
                    break;
                case 1:
                    if (tool_status==1)
                        {
                            myPlot.lines->SetNewEndpoint(toolx,tooly);
                        }
                    else if(tool_status==2)
                        {
                            myPlot.lines->SetNewEndpoint(toolx,tooly);

                            double r = min(myPlot.lines->length()/2,myPlot.lines->next->length()/2);
                            double a0 = myPlot.lines->angle();
                            //double a1 = -PI+myPlot.lines->next->angle();
                            double a1 = myPlot.lines->next->iangle();

                            a1 = a0<a1-PI ? a1-PI*2 : a1;
                            double temp;
                            if(a1 < a0)
                            {
                                temp=a0;
                                a0=a1;
                                a1=temp;
                            }

                            if(a1>PI+a0)
                            {
                                a1-=PI*2;
                            }

                            if(a1 < a0)
                            {
                                temp=a0;
                                a0=a1;
                                a1=temp;
                            }
                           // cout<<a0*180/PI<<" "<<a1*180/PI <<"\n";

                            myPlot.angles->SetNewAngle(r,a0,a1);
                        }
                    break;
                case 2:
                    if(tool_status==1)
                    {
                        myPlot.lines->SetNewEndpoint(toolx,tooly);
                        myPlot.labels->setPosition(myPlot.lines->x0+myPlot.lines->length()*cos(myPlot.lines->angle()),
                                                  myPlot.lines->y0+myPlot.lines->length()*sin(myPlot.lines->angle()));
                        myPlot.labels->setText(patch::to_string(myPlot.lines->length()));
                        myPlot.markers->setPosition(toolx,tooly);
                    }
                    break;
                case 3:
                    {
                       // double tx0=toolx-((scene.x1-scene.x0)/(0.85*scene.width));
                       // double tx1=toolx+((scene.x1-scene.x0)/(0.85*scene.width));
                       // double ty0=tooly-((scene.x1-scene.x0)/(0.85*scene.height));
                       // double ty1=tooly+((scene.x1-scene.x0)/(0.85*scene.height));
                       //cout<<(scene.x1-scene.x0)/(0.2*scene.width)<<"\n";
                       double t = (scene.x1-scene.x0)/(0.1*scene.width);
                        for(CircleObject* c=myPlot.circles;c!=NULL;c=c->next)
                        {
                            if(c->touchRadius(toolx,tooly,t))
                               c->layer=1;
                            else
                                c->layer=0;
                        }
                        for(LineObject* n=myPlot.lines;n!=NULL;n=n->next)
                        {
                            if(n->touchLine(toolx,tooly,t))
                               n->layer=1;
                            else
                                n->layer=0;
                        }
                    }break;
                    case 4:
                    {
                        if (tool_status==1)
                        {
                            myPlot.lines->SetNewEndpoint(toolx,tooly);

                            double r = myPlot.lines->length();//(scene.x1-scene.x0)/4;
                            double a0 = myPlot.lines->angle();
                            //double a1 = -PI+myPlot.lines->next->angle();
                            double a1 = -PI/2;//myPlot.lines->next->iangle();

                            a1 = a0<a1-PI ? a1-PI*2 : a1;
                            double temp;
                            if(a1 < a0)
                            {
                                temp=a0;
                                a0=a1;
                                a1=temp;
                            }

                            if(a1>PI+a0)
                            {
                                a1-=PI*2;
                            }

                            if(a1 < a0)
                            {
                                temp=a0;
                                a0=a1;
                                a1=temp;
                            }
                           // cout<<a0*180/PI<<" "<<a1*180/PI <<"\n";

                            //myPlot.angles->SetPosition(toolx,tooly);
                            myPlot.angles->SetNewAngle(r,a0,a1);

                            myPlot.labels->setPosition(myPlot.lines->x0+myPlot.lines->length()*cos(myPlot.lines->angle()),
                                                  myPlot.lines->y0+myPlot.lines->length()*sin(myPlot.lines->angle()));
                            myPlot.labels->setText(patch::to_string(floor(((a1-a0)*180/PI)*100)/100));
                            myPlot.markers->setPosition(toolx,tooly);
                            myPlot.markers->setRotation(myPlot.lines->angle());
                        }
                    break;
                    }
                }//switch tool number
            }
            bMouseDown=false;
        }
        if(MouseWheelPos != receiver.GetMouseState().WheelPos)
        {
            if(MouseWheelPos < receiver.GetMouseState().WheelPos && scene.x1-scene.x0 > 1)
            {
                float s = (scene.x1-scene.x0)/scene.width;
                scene.x0+=s*75;
                scene.y0+=s*75;
                scene.x1-=s*75;
                scene.y1-=s*75;
                //cout<<"scene: "<<scene.x0<<","<<scene.y0<<","<<scene.x1<<","<<scene.y1<<"\n";
                //cout<<"vertexes: "<<n_vertexes<<"\n";
                //double d = (PI*2*50/s/5)+(PI*2*30/s/5)+(PI*2*15/s/5);
                //cout<<"lines: "<<d<<"\n";
                if((scene.x1-scene.x0)/myPlot.gridspacing < 3 && myPlot.gridspacing > 1)
                    myPlot.gridspacing/=2;
            }
            else if(MouseWheelPos > receiver.GetMouseState().WheelPos)
            {
                float s = (scene.x1-scene.x0)/scene.width;
                scene.x0-=s*75;
                scene.y0-=s*75;
                scene.x1+=s*75;
                scene.y1+=s*75;
                //cout<<"scene: "<<scene.x0<<","<<scene.y0<<","<<scene.x1<<","<<scene.y1<<"\n";
                //cout<<"vertexes: "<<n_vertexes<<"\n";
                if((scene.x1-scene.x0)/myPlot.gridspacing > 7)
                    myPlot.gridspacing*=2;
            }
            MouseWheelPos = receiver.GetMouseState().WheelPos;
            //cout<<MouseWheelPos<<"\n";
        }

        n_vertexes=0;

        if(receiver.IsKeyDown(KEY_KEY_F))
           {
               if(fDown==false)
                    renderImage(img,tex,myPlot,myFont,scene,true);
                fDown=true;
           }
        else
        {
            if(!paused)
                renderImage(img,tex,myPlot,myFont,scene,false);
            fDown = false;
        }

        if(receiver.IsKeyDown(KEY_KEY_P))
           {
               if(pDown==false)
                  paused=!paused;
                pDown=true;
           }
        else
        {
            pDown = false;
        }

        if(receiver.IsKeyDown(KEY_KEY_Z))
           {
               if(zDown==false)
                  {
                   if(tool_number==3)
                   {
                       if(myPlot.circles)
                       {
                           CircleObject* c;
                           CircleObject* cc;
                           for(c=myPlot.circles,cc = c->next;cc!=NULL;c=c->next,cc=cc->next)
                            {
                                if(cc->layer==1)
                                {
                                    c->next=cc->next;
                                    cc->next=NULL;
                                    delete cc;
                                }
                            }

                            if(myPlot.circles->layer==1)
                            {
                                c=myPlot.circles;
                                myPlot.circles=c->next;
                                c->next = NULL;
                                delete c;
                            }
                       }

                        if(myPlot.lines)
                        {
                           LineObject* c;
                           LineObject* cc;
                           for(c=myPlot.lines,cc = c->next;cc!=NULL;c=c->next,cc=cc->next)
                            {
                                if(cc->layer==1)
                                {
                                    c->next=cc->next;
                                    cc->next=NULL;
                                    myPlot.deleteMarkersLabels(cc->group);
                                    delete cc;
                                }
                            }

                            if(myPlot.lines->layer==1)
                            {
                                c=myPlot.lines;
                                myPlot.lines=c->next;
                                c->next = NULL;
                                myPlot.deleteMarkersLabels(c->group);
                                delete c;
                            }
                        }
                   }
                  }
                zDown=true;
           }
        else
        {
            zDown = false;
        }

        if(receiver.IsKeyDown(KEY_KEY_A))
           {
            if(aDown==false)
                {
                tool_number++;
                if(tool_number > 4)
                    tool_number=0;
                switch(tool_number)
                    {
                    case 0:
                        cout<<"Tool Selected: Circle\n";
                        break;
                    case 1:
                        cout<<"Tool Selected: Angle\n";
                        break;
                    case 2:
                        cout<<"Tool Selected: Line\n";
                        break;
                    case 3:
                        cout<<"Tool Selected: Eraser\n";
                        break;
                    case 4:
                        cout<<"Tool Selected: Directional Ray\n";
                        break;
                    }
                }
                aDown=true;
           }
        else
            {
            aDown = false;
            }

        time = device->getTimer()->getTime();


        videoDriver->beginScene();

        videoDriver->draw2DImage(tex, zeroVector, srcRect, 0, white, true);
        videoDriver->endScene();
    }
    //delete plotobjects;
    img->drop();

    device->drop();
    return 0;
}
