#define _GLIBCXX_USE_CXX11_ABI 0
#include <agg_basics.h>
#include <agg_rendering_buffer.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_scanline_p.h>
#include <agg_renderer_scanline.h>
#include "agg_font_win32_tt.h"
#include <agg_path_storage.h>
#include "agg_conv_marker.h"
#include "agg_vcgen_markers_term.h"

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
#include "agg_scanline_u.h"
#include "agg_span_allocator.h"
#include "agg_span_gradient.h"
#include "agg_span_interpolator_linear.h"
#include "agg_conv_dash.h"

//
#include <irrlicht.h>

//
#include <math.h>

// basic file operations
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "PolyTypes.h"
#include "PlotObjects.h"
#include "NoiseMap.h"
#include "Outline.h"
#include "Plot.h"

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


namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

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


class MyEventReceiver : public IEventReceiver
{
public:
    // We'll create a struct to record info on the mouse state
    struct SMouseState
    {
        core::position2di Position;
        bool LeftButtonDown;
        bool RightButtonDown;
        int WheelPos;
        SMouseState() : LeftButtonDown(false),RightButtonDown(false),WheelPos(0) { }

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

            case EMIE_RMOUSE_LEFT_UP:
                MouseState.RightButtonDown = false;
                break;

            case EMIE_RMOUSE_PRESSED_DOWN:
                MouseState.RightButtonDown = true;
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



inline double SceneX(double x, Scene scene)
{
return CLIP_BORDER+(x-scene.x0)/(scene.x1-scene.x0)*scene.width;
}

inline double SceneY(double y, Scene scene)
{
return CLIP_BORDER+(y-scene.y0)/(scene.y1-scene.y0)*scene.height;
}


double time;

int n_vertexes=0;
bool paused=false;




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
    double m_weight=2;
    double m_height=30;
    double m_width=20;
    agg::rgba m_color= agg::rgba(0.6*0.8,0.8,0.6,1);



public:

    void size(double height, double width, double weight)
    {
    m_weight=weight;
    m_height=height;
    m_width=width;
    }

    void color(agg::rgba color)
    {
     m_color=color;
    }

    FontDraw(HDC dc) : m_feng(dc),
        m_fman(m_feng),
        m_curves(m_fman.path_adaptor()),
        m_contour(m_curves)
        {}


 template<class Rasterizer, class Scanline, class RenSolid, class RenBin>
    unsigned draw_text(agg::pod_array<bLabelObject> labels,Scene scene,Rasterizer& ras, Scanline& sl,
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

        //m_contour.width(-m_weight * m_height * 0.05);

        //m_feng.hinting(m_hinting.status());


        // Font width in Windows is strange. MSDN says,
        // "specifies the average width", but there's no clue what
        // this "average width" means. It'd be logical to specify
        // the width with regard to the font height, like it's done in
        // FreeType. That is, width == height should mean the "natural",
        // not distorted glyphs. In Windows you have to specify
        // the absolute width, which is very stupid and hard to use
        // in practice.
        //-------------------------
        //m_feng.width(((m_width) == m_height) ? 0.0 : m_width / 2.4);
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

/*
         if(labels && labels->type==1)
            {
            m_height=14;
            m_color = agg::rgba(0.9,0.9,1,1);
            }
        else
            {
            m_height=18;
            m_color = agg::rgba(0.6*0.8,0.8,0.6,1);
            }
*/

        m_height=14;
        m_color = agg::rgba(0.9,0.9,1,1);

        m_feng.height(m_height);
      //  m_feng.
        m_contour.width(-4 * m_height * 0.7);

        if(m_feng.create_font("Ariel", gren))
        {
            m_fman.precache(' ', 127);
          //  for(LabelObject* label = labels; label != NULL; label = label->next)
          for(int w=0; w<labels.size(); w++)
            {
                double x = SceneX(labels[w].x0,scene)+labels[w].x_offset;//10.0+i*100;
                double y = SceneY(labels[w].y0,scene)+labels[w].y_offset;//10.0+i*100;//60/*height()*/ - m_height - 10.0;

                double x1=x;
                //double y1=y-14;

                //if(x<CLIP_BORDER||x>1024+CLIP_BORDER||y<CLIP_BORDER||y>640+CLIP_BORDER)
                if(x<0||x>1024||y<0||y>640)
                    continue;

                //double y = y0;
                    const char* p = labels[w].text;
                    const agg::glyph_cache* glyph;

                    glyph = m_fman.glyph(*p);
                   // if(glyph)
                    //y += 16;

                    while(*p)
                    {
                        glyph = m_fman.glyph(*p);
                        if(glyph)
                        {
                            x1 += glyph->advance_x;
                            //y1 += glyph->advance_y;
                        }
                        ++p;
                    }
                    if(x1-0.0001>x)
                    {
                        agg::path_storage ps;
                        agg::conv_stroke<agg::path_storage> pg(ps);

                        ps.move_to(x-2,y-14);
                        ps.line_to(x-2,y+2);
                        ps.line_to(x1+2,y+2);
                        ps.line_to(x1+2,y-14);
                        ps.close_polygon();
                        ras.reset();
                        ras.add_path(ps);

                        ren_bin.color(agg::rgba(0,0,0,0.6));

                        agg::render_scanlines(ras, sl, ren_bin);
                    }
                    ras.reset();

                    ren_bin.color(m_color);

                    p = labels[w].text;
                    while(*p)
                    {
                        glyph = m_fman.glyph(*p);
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
            }//if font
            else cout<<"no font\n";
        return num_glyphs;
    }

};

// A simple function to form the gradient color array
// consisting of 3 colors, "begin", "middle", "end"
//---------------------------------------------------
template<class Array>
void fill_color_array(Array& array,
                      agg::rgba8 begin,
                      agg::rgba8 middle,
                      agg::rgba8 end)
{
    unsigned i;
    unsigned half_size = array.size() / 2;
    for(i = 0; i < half_size; ++i)
    {
        array[i] = begin.gradient(middle, i / double(half_size));
    }
    for(; i < array.size(); ++i)
    {
        array[i] = middle.gradient(end, (i - half_size) / double(half_size));
    }
}

#define FLAG_MAKE_BMP 1
#define FLAG_TOGGLE_1 2

bool renderImage(irr::video::IImage* pImage,irr::video::ITexture* imgtex,Plot& plot,FontDraw& myFont,Scene scene, char flags)
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

    agg::pixfmt_bgra32 pixelFormat(renderingBuffer);
    agg::renderer_base<agg::pixfmt_bgra32> rendererBase(pixelFormat);
    agg::scanline_p8 scanLine;
    agg::rasterizer_scanline_aa<> ras;
    agg::renderer_primitives<agg::pixfmt_bgra32> prim(pixelFormat);

    typedef agg::renderer_scanline_aa_solid<agg::renderer_base<agg::pixfmt_bgra32> > renderer_solid;
    typedef agg::renderer_scanline_bin_solid<agg::renderer_base<agg::pixfmt_bgra32> > renderer_bin;

    renderer_solid ren_solid(rendererBase);
    renderer_bin ren_bin(rendererBase);

    typedef agg::pod_auto_array<agg::rgba8, 256> color_array_type;


    // Gradient shape function (linear, radial, custom, etc)
    //-----------------
    typedef agg::gradient_circle gradient_func_type;


    // Span interpolator. This object is used in all span generators
    // that operate with transformations during iterating of the spans,
    // for example, image transformers use the interpolator too.
    //-----------------
    typedef agg::span_interpolator_linear<> interpolator_type;


    // Span allocator is an object that allocates memory for
    // the array of colors that will be used to render the
    // color spans. One object can be shared between different
    // span generators.
    //-----------------
    typedef agg::span_allocator<agg::rgba8> span_allocator_type;


    // Finally, the gradient span generator working with the agg::rgba8
    // color type.
    // The 4-th argument is the color function that should have
    // the [] operator returning the color in range of [0...255].
    // In our case it will be a simple look-up table of 256 colors.
    //-----------------
    typedef agg::span_gradient<agg::rgba8,
                               interpolator_type,
                               gradient_func_type,
                               color_array_type> span_gradient_type;


    // The gradient scanline renderer type
    //-----------------
    typedef agg::renderer_scanline_aa<agg::renderer_base<agg::pixfmt_bgra32>,
                                      span_allocator_type,
                                      span_gradient_type> renderer_gradient_type;


    // The gradient objects declarations
    //----------------
    gradient_func_type  gradient_func;                   // The gradient function
    agg::trans_affine   gradient_mtx;                    // Affine transformer
    interpolator_type   span_interpolator(gradient_mtx); // Span interpolator
    span_allocator_type span_allocator;                  // Span Allocator
    color_array_type    color_array;                     // Gradient colors

    // Declare the gradient span itself.
    // The last two arguments are so called "d1" and "d2"
    // defining two distances in pixels, where the gradient starts
    // and where it ends. The actual meaning of "d1" and "d2" depands
    // on the gradient function.
    //----------------
    span_gradient_type span_gradient(
                                     span_interpolator,
                                     gradient_func,
                                     color_array,
                                     60, 500);

    // The gradient renderer
    //----------------
    renderer_gradient_type ren_gradient(rendererBase, span_allocator, span_gradient);

    fill_color_array(color_array,
                     agg::rgba8(250,40,40,200),
                    // agg::rgba8(80, 80, 240,200),
                     agg::rgba8(250, 250, 40),
                     agg::rgba8(40, 40, 250,150));

    gradient_mtx *= agg::trans_affine_scaling(1.3, 1.3);
    gradient_mtx *= agg::trans_affine_rotation(agg::pi/5);
    gradient_mtx *= agg::trans_affine_translation(-scene.x0+250,-scene.y0+150);
    gradient_mtx.invert();

    //cout<<renderingBuffer.height()<<"   "<<renderingBuffer.width()<<"\n";
    renderingBuffer.clear(0);

    agg::path_storage ps;
    agg::conv_stroke<agg::path_storage> stroke1(ps);


    agg::path_storage ps2;
    agg::conv_stroke<agg::path_storage> stroke2(ps2);

    //rendererBase.clear(agg::rgba8(255, 255, 255));


    agg::path_storage ps3;
    agg::conv_dash<agg::path_storage, agg::vcgen_markers_term> dash(ps3);
     // dash.add_dash(20.0, 5.0);
    dash.add_dash(10.0, 3.0);


    agg::conv_stroke<agg::conv_dash<agg::path_storage,agg::vcgen_markers_term> >  stroke3(dash);

    //terrain
    agg::path_storage ps4;
    agg::conv_stroke<agg::path_storage> stroke4(ps4);

    agg::path_storage ps5;
    agg::conv_stroke<agg::path_storage> stroke5(ps5);

    agg::path_storage ps6;
    agg::conv_stroke<agg::path_storage> stroke6(ps6);

    agg::path_storage ps7;
    agg::conv_stroke<agg::path_storage> stroke7(ps7);


    LinePoly gpoly;
    //if(plot.gridspacing > 0)
    if(false)
    {
        for(int g=plot.gridspacing*(int)(scene.x0/plot.gridspacing);g<scene.x1;g+=plot.gridspacing)
        {
            //cout<<g<<"\n";pg.width(2);
            gpoly.set0(SceneX(g,scene),CLIP_BORDER);
            gpoly.set1(SceneX(g,scene),CLIP_BORDER+512);
            ps.concat_path(gpoly);
        }
        for(int g=plot.gridspacing*(int)(scene.y0/plot.gridspacing);g<scene.y1;g+=plot.gridspacing)
        {
            //cout<<g<<"\n";
            gpoly.set0(CLIP_BORDER,SceneY(g,scene));
            gpoly.set1(CLIP_BORDER+512,SceneY(g,scene));
            ps.concat_path(gpoly);
        }
       // ras.add_path(stroke1);
       //stroke1.width(0.5);
        agg::render_scanlines_bin_solid(ras, scanLine, rendererBase, agg::rgba(0.3,0.3,0.7,0.5));

       // ps.remove_all();
    }
    ras.reset();
    ArbPoly_solid arbpoly;

    for(ArbObject* isl=plot.islands;isl !=NULL;isl=isl->next)
        {
        if(isl->getPoly(arbpoly,scene))
            {
                //ras.add_path(arbpoly);
                ps.concat_path(arbpoly);
                //ps.close_polygon();
            }
        }
    ps.remove_all();

    ArcPoly_solid apoly_s;
    ArcPoly apoly;
    for(CircleObject* circle=plot.circles;circle !=NULL;circle=circle->next)
        {
        if(circle->getPoly(apoly,scene))
            {
            switch(circle->layer)
                {
                case 0:
                    ps2.concat_path(apoly);
                    break;
                default:
                    ps.concat_path(apoly);
                    break;
                }
            }
        }
    for(AngleObject* Angle=plot.angles;Angle !=NULL;Angle=Angle->next)
        {
        if(Angle->getPoly_solid(apoly_s,scene))
            {
           ras.add_path(apoly_s);
            }
        }

    BezierPoly bpoly(125,-150,300,400,100,200,400,300,0.0,1.0,9);

    CurvePoly cpoly;
    CurvePoly_solid cpoly_solid;
    Marker_small_arrow_poly marker_sa_poly;

    int filt=0;
   // int zc=0;
    //if(drawTerrain)
    for(CurveObject* curve=plot.curves;curve !=NULL;curve=curve->next)
        {
    //if(filt<20)
    if(flags & FLAG_TOGGLE_1)
    {
        //if(curve->getPoly(cpoly_solid,scene)  && curve->terr_group==0)

        if(curve->getPoly(cpoly_solid,scene))
        {
             ras.add_path(cpoly_solid);
            agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.35,0.55,0.7,0.15));
        }

        //    {
/*
            for(int i=0;i<curve->n_bez;i++)
                if(curve->m_bez[i].getPoly(bpoly,scene))
            {
                marker_sa_poly.setPosition(bpoly.dx,bpoly.dy);
                marker_sa_poly.setRotation(curve->m_bez[i].angle()+PI);
                if(i%2)
                {
                    ps2.concat_path(bpoly);
                   // ps2.concat_path(marker_sa_poly);
                }
                else
                {
                    ps4.concat_path(bpoly);

                    ps.concat_path(marker_sa_poly);
                }
            }
*/
            // ps2.concat_path(cpoly_solid);

         //   }
          //  else
        //    {

         // agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.35,0.45,0.8,0.3));
         //   }
            //ps2.concat_path(cpoly);

    }
       if(curve->terr_group == 0 )
        {
            if(curve->getPoly(cpoly,scene))
            {
                //if(curve->perimeter < 250*((scene.x1-scene.x0)/scene.width) && curve->terr_group > 0)
               // {
                    //ps.concat_path(cpoly);
               // }
               // else
                {
                   // agg::conv_stroke<agg::path_storage> stroke1(ps);


                    //dash.dash_start((cpoly.interval[0]);
                   // double d = sqrt( pow( cpoly.m_bez[0].ax - cpoly.m_bez[cpoly.range[0]].test_x(cpoly.interval[0]-cpoly.range[0]),2) +
                    //                 pow( cpoly.m_bez[0].ay - cpoly.m_bez[cpoly.range[0]].test_y(cpoly.interval[1]-cpoly.range[1]),2) );
                    dash.dash_start(0);
                    ps4.concat_path(cpoly);

                   // agg::conv_dash<agg::path_storage, agg::vcgen_markers_term> dash(ps);




                   // zc++;

                }
            }
        }



      /*  if(curve->getPoly(cpoly_solid,scene))
            {
          ras.add_path(cpoly_solid);
          agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.35,0.45,0.8,0.3));
            }
*/
        filt++;
    }


    LinePoly lpoly;


    for(ArrowObject* aro=plot.arrows;aro !=NULL;aro=aro->next)
        {
        if(aro->getPoly(arbpoly,scene))
            {
                //cout<<"shit\n";
                ras.add_path(arbpoly);
                //ps.concat_path(arbpoly);
                //ras.
            }
        }
 agg::render_scanlines_bin_solid(ras, scanLine, rendererBase, agg::rgba(1,1,1.0,1));
    ras.reset();


   // agg::render_scanlines_aa(ras, scanLine, rendererBase, span_allocator, span_gradient);
    //agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.2,0.4,0.7,0.4));
   // agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.5,0.2,0.85,0.4+sin(time/1000)*0.08));
    //ras.reset();
    //ras.add_path(stroke1);

    Marker_small_circle_poly marker_sc_poly;
    Marker_dash_poly marker_dash_poly;
    //Marker_small_arrow_poly marker_sa_poly;
    Marker_dot_poly marker_d_poly;
    //prim.l

    //std::cout<<"A\n";
    Path_Markers* pm=NULL;
    Path_Lines* plines=NULL;
    Path_Labels* pl=NULL;
    Path_Circle* pc =NULL;
    Marker_cross_poly marker_c_poly;
    Marker_square_poly marker_sq_poly;
    agg::path_storage* ps_current=NULL;
    static Scene scene_screen = {CLIP_BORDER,CLIP_BORDER,scene.width+CLIP_BORDER,scene.height+CLIP_BORDER,scene.width,scene.height};
    for(PathObject* path = plot.paths; path!=NULL; path=path->next)
    {
       for(Path_Draw* pDraw=path->draw;pDraw !=NULL;pDraw=pDraw->next)
       {
          // std::cout<<"hghg\n";
        switch(pDraw->graph_type)
        {
/*            case GRAPH_SQUARE:
            pc = dynamic_cast<Path_Square*>(pDraw);
            if(pc)
            for(ArbObject* sq=pc->square;sq !=NULL;sq=sq->next)
            {
                if(square->getPoly(arbpoly,scene))
                    {
                   //     ras.add_path(arbpoly);

                    switch(arbpoly->layer)
                        {
                        case 0:
                            ps.concat_path(arbpoly);
                            break;
                        case 1:
                            ps2.concat_path(arbpoly);
                            break;
                        }

                    }
                }
            }
            break;*/
            case GRAPH_CIRCLES:
            pc = dynamic_cast<Path_Circle*>(pDraw);
            if(pc)
            for(CircleObject* circle=pc->circles;circle !=NULL;circle=circle->next)
            {
            if(pc->flags & GRAPH_FLAGS_SCREENCOORDS)
                {
                    //ircle->ScreenTransform(scene);
                    if(circle->getPoly_Screen(apoly,scene))
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
                }
            else
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
                }
            }
            break;
            //ras.add_path(apoly_s);
        //if(circle->getPoly(apoly,scene))
        //    ps.concat_path(apoly);
            case GRAPH_LINES:
                 plines = dynamic_cast<Path_Lines*>(pDraw);
                if(plines)
                for(LineObject* line=plines->lines;line !=NULL;line=line->next)
                {
                    if(line->getPoly(lpoly,scene))
                    {
                    switch(line->layer)
                        {
                        case 0:
                            ps3.concat_path(lpoly);
                            break;
                        case 1:
                            ps7.concat_path(lpoly);
                            break;
                        case 2:
                            ps.concat_path(lpoly);
                            break;
                        case 3:
                            ps4.concat_path(lpoly);
                            break;
                        case 4:
                            ps5.concat_path(lpoly);
                            break;
                        case 5:
                            ps6.concat_path(lpoly);
                            break;
                        }
                    }
                }
            break; //GRAPH_LINES
            case GRAPH_MARKERS:

                pm = dynamic_cast<Path_Markers*>(pDraw);
               // bMarkerObject* m;
                if(pm)
                {
                 //  cout<<pm->bMarkers.size()<<"--\n";
                  switch(pm->bMarkers[0].layer)
                  {
                      case 0:
                            ps_current = &ps;
                            break;
                        case 1:
                             ps_current = &ps7;
                            break;
                        case 2:
                             ps_current = &ps;
                            break;
                        case 3:
                            ps_current = &ps4;
                            break;
                        case 4:
                             ps_current = &ps5;
                            break;
                        case 5:
                             ps_current = &ps6;
                            break;
                        default:
                             ps_current = &ps;
                            break;

                  }
                    for(int i=0;i<pm->bMarkers.size();i++)
                    {

                        if(pm->bMarkers[i].clip(scene) || pDraw->flags & GRAPH_FLAGS_SCREENCOORDS)
                        {
                            switch( pm->bMarkers[i].type)
                            {
                                case MARKER_DASH:

                                if(pDraw->flags & GRAPH_FLAGS_SCREENCOORDS)
                                {
                                    switch(pm->type)
                                    {
                                    case GRAPH_MARKERS_COMPASS:
                                            marker_dash_poly.setPosition(
                                            SceneX(pm->data->m_points[(pm->index)*2],scene)+pm->bMarkers[i].x0,
                                            SceneY(pm->data->m_points[(pm->index)*2+1],scene)+pm->bMarkers[i].y0 );
                                            break;
                                    case GRAPH_MARKERS_PROTRACTOR:
                                            marker_dash_poly.setPosition(
                                            SceneX(pm->data->m_points[(pm->index)*2],scene)+pm->bMarkers[i].x0,
                                            SceneY(pm->data->m_points[(pm->index)*2+1],scene)+pm->bMarkers[i].y0 );
                                            break;
                                    }
                                }
                                else
                                    marker_dash_poly.setPosition(SceneX(pm->bMarkers[i].x0,scene)+pm->bMarkers[i].x_offset,SceneY(pm->bMarkers[i].y0,scene)+pm->bMarkers[i].y_offset);
                                    marker_dash_poly.setRotation(pm->bMarkers[i].rot);
                                    marker_dash_poly.setLength(pm->bMarkers[i].len);

                                    ps_current->concat_path(marker_dash_poly);
                                    break;
                                case MARKER_SQUARE:
                                    marker_sq_poly.setPosition(SceneX(pm->bMarkers[i].x0,scene),SceneY(pm->bMarkers[i].y0,scene));
                                    ps2.concat_path(marker_sq_poly);

                                    break;

                                case MARKER_SMALL_CIRCLE:
                                    marker_sc_poly.setPosition(SceneX(pm->bMarkers[i].x0,scene),SceneY(pm->bMarkers[i].y0,scene));
                                    ps_current->concat_path(marker_sc_poly);

                                    break;
                                case MARKER_SMALL_ARROW:
                                    marker_sa_poly.setPosition(SceneX(pm->bMarkers[i].x0,scene),SceneY(pm->bMarkers[i].y0,scene));
                                    marker_sa_poly.setRotation(pm->bMarkers[i].rot);
                                    ps_current->concat_path(marker_sa_poly);

                                    break;

                                case MARKER_DOT:
                                    marker_d_poly.setPosition(SceneX(pm->bMarkers[i].x0,scene),SceneY(pm->bMarkers[i].y0,scene));
                                    marker_sc_poly.setPosition(SceneX(pm->bMarkers[i].x0,scene),SceneY(pm->bMarkers[i].y0,scene));
                                    //ps_current->concat_path(marker_sc_poly);
                                   // marker_sc_poly
                                     ras.add_path(marker_sc_poly);
                                     ras.add_path(marker_d_poly);
                                     /*
                                    switch(pm->bMarkers[i].layer)
                                        {
                                        case 0:
                                            ras.add_path(marker_d_poly);
                                            break;
                                        case 1:
                                            ps2.concat_path(marker_d_poly);
                                            break;
                                        }*/
                                    break;
                                case MARKER_CROSS:
                                    marker_c_poly.setPosition(SceneX(pm->bMarkers[i].x0,scene),SceneY(pm->bMarkers[i].y0,scene));
                                    ps_current->concat_path(marker_c_poly);

                                    break;
                            }//switch
                        }//if clip
                    }//for marker
                }//if pm
                break; //GRAPH_MARKERS
                case GRAPH_LABELS:
                   // agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba8(110, 60, 165,255));
                  //  ras.reset();


                break;//GRAPH_LABELS
        }//switch graph type
       }//for draw
    }//for path
    //std::cout<<"B\n";
    //agg::render_s
    agg::render_scanlines_bin_solid(ras, scanLine, rendererBase, agg::rgba(0.5,0.5,1.0,0.4));
    ras.reset();

    stroke1.width(1.5);
    stroke2.width(0.01);
    stroke3.width(1.5);
    stroke4.width(1.5);
    stroke5.width(2.5);
    stroke6.width(1.5);
    stroke7.width(2.5);

    ras.add_path(stroke4);
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.6,0.3,0.6,0.8));
    ras.reset();

    ras.add_path(stroke2);
    //agg::render_s
    agg::render_scanlines_bin_solid(ras, scanLine, rendererBase, agg::rgba(0.8,0.8,0.8,1.0));
    ras.reset();

    ras.add_path(stroke3);
    ras.add_path(stroke1);
    //agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba8(110, 60, 165,255));
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba8(255, 255, 255,255));
    ras.reset();

    ras.add_path(stroke5);
    //agg::render_s
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.2,0.8,0.5,1.0));
    ras.reset();

    ras.add_path(stroke6);
    //agg::render_s
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.8,0.3,0.1,1.0));
    ras.reset();

    ras.add_path(stroke7);
    //agg::render_s
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.8,0.3,0.1,0.7));
    ras.reset();


    //ras.add_path(stroke3);
    //agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.8,0.8,0.8,1.0));
    //ras.reset();

    for(PathObject* path = plot.paths; path!=NULL; path=path->next)
    {
       for(Path_Draw* pDraw=path->draw;pDraw !=NULL;pDraw=pDraw->next)
       {
        if(pDraw->graph_type==GRAPH_LABELS)
        {
            pl = dynamic_cast<Path_Labels*>(pDraw);
            if(pl)
//                for(;pl!=NULL;pl=pl->next)
                    {
                    myFont.draw_text(pl->bLabels,scene,ras, scanLine, ren_solid, ren_bin);
                    }
        }
       }
    }

    if(flags & FLAG_MAKE_BMP) generateBitmapImage(imgDataPtr,imgSize.Height,imgSize.Width,"bmpTest.bmp");

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


bool specialRenderImage(irr::video::IImage* pImage,irr::video::ITexture* imgtex,Plot& plot,FontDraw& myFont,Scene scene,bool makeBMP = false)
{
    if (!pImage)
        return false;

    // Create AGG stuff, setting the Irrlicht image->getData() as the buffer

    // Yes, it's technically u32 data, but Anti-Grain treats it as pixels composed of 1 byte/8 bit colors.
    irr::u8* imgDataPtr = (irr::u8*) pImage->lock();
    irr::core::dimension2du imgSize = pImage->getDimension();

    agg::rendering_buffer renderingBuffer;
    renderingBuffer.attach(imgDataPtr, imgSize.Width, imgSize.Height, pImage->getPitch());

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
    agg::conv_stroke<agg::path_storage> stroke1(ps);


    agg::path_storage ps2;
    agg::conv_stroke<agg::path_storage> stroke2(ps2);




    BezierPoly bpoly(125,-150,300,400,100,200,400,300,0.0,1.0,9);
    //BezObject bezOb(125,-150,300,400,100,200,400,300);
    //cout<<bezOb.len<<"\n";

    for(CurveObject* curve=plot.curves;curve !=NULL;curve=curve->next)
        {
        for(int i=0;i<curve->n_bez;i++)
            {
            if(curve->m_bez[i].getSpecialPoly(bpoly,scene))
                {

                /*ps2.concat_path(bpoly);switch(curve->m_bez[i].layer)
                    {
                    case 0:
                        ps.concat_path(bpoly);
                        break;
                    case 1:
                        ps2.concat_path(bpoly);
                        break;
                    }*/
                    if(i%2)
                    ps.concat_path(bpoly);
                    else
                    ps2.concat_path(bpoly);
                }
            }
        }

    stroke1.width(3);
    stroke2.width(3);

    //agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.5,0.6,0.9));
   // agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.5,0.2,0.85,0.4+sin(time/1000)*0.08));
    ras.reset();


    ras.add_path(stroke1);
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(0.6,0.6,0.85,0.8));
    ras.reset();

    ras.add_path(stroke2);
    agg::render_scanlines_aa_solid(ras, scanLine, rendererBase, agg::rgba(1,1,0.8,1));

//    myFont.draw_text(plot.labels,scene,ras, scanLine, ren_solid, ren_bin);

   // if(makeBMP) generateBitmapImage(imgDataPtr,imgSize.Width,imgSize.Height,"bmpTest.bmp");

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



extern unsigned N_STEPS=10;
//void InitOutline(Outline& outline)
void InitMap(Plot& myPlot)
{

    //int minc =75437;

    //int minc =833624;
    //int minc=94534;//<---------
    //int minc = 628;
    //int minc = 17789; //
    int minc = 10019; //

    //int minc = 80870;
    NoiseMap* Map = new NoiseMap(1024,8);
    //Map.init(373);
    Map->init(minc);
    NoiseMap* Map2 = new NoiseMap(1024,16);
    Map2->init(530);

    NoiseMap* Map3 = new NoiseMap(1024,32);
    Map3->init(1863);

    Map->addNoise(*Map2,0.1);
    Map->addNoise(*Map3,0.05);
    //Map.postprocess();
    //Outline outline(512);

    Outline outline(1024);

    int jj=0;
    int pcount=0;
    for(int z=0;z<6;z++)
    {
        outline.init(*Map,155+(z*10),155);
       // if(z==0)
       //     outline.finalize_map(Map,155);
         for(int i=0;i<outline.MapPixels;i++)
            for(int j =0;j<outline.MapPixels;j++)
            {
                if(outline.m_map[j*outline.MapPixels+i]==1)
                {

                    if(jj >=0 )  //22 //1
                    //if(false)
                    {
                         std::cout<<jj<<"  \n";
                        outline.trace(i,j);
                        if(outline.trace_area < 15)
                        {
                         jj++;
                         std::cout<<"trace area ="<<outline.trace_area<<", deleting\n";
                         continue;
                        }
                        /*
                        myPlot.addPath();
                        myPlot.paths->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_MARK1,1);
                        for(int ii=0;ii<49 && ii<outline.n_trace;ii++)
                        {
                            double x_,y_;
                        if(outline.m_trace[ii].flags & X_ALIGNED && !(outline.m_trace[ii].flags & NO_Y_ROOT))
                            {
                                x_=outline.m_trace[ii].x0;
                                y_=(double)outline.m_trace[ii].y0+outline.m_trace[ii].y_offset;

                                myPlot.paths->addPoint(x_,y_);
                            }
                            else if(!(outline.m_trace[ii].flags & X_ALIGNED) && !(outline.m_trace[ii].flags & NO_X_ROOT))
                            {
                                x_=(double)outline.m_trace[ii].x0+outline.m_trace[ii].x_offset;
                                y_=outline.m_trace[ii].y0;

                                myPlot.paths->addPoint(x_,y_);
                            }

                        }
                        */
                    }
                    else
                    {
                        jj++;
                        outline.fill_polygon(i,j);
                        continue;
                    }
                    jj++;
                    //cout<<pcount<<"\n";
                   // bez_obj.n_bez=0;
                    bezier_object bez_obj;
                    bez_obj.n_bez=0;
                   // if(i==229 && j==426)
                        bez_obj = outline.MakeBezierObject();
                    //cout<<"n_bez="<<bez_obj.n_bez<<"\n";

                    if(bez_obj.n_bez > 0 )
                    {
                        myPlot.addCurve(bez_obj);
                        myPlot.curves->terr_group=z;
                        /*
                        myPlot.addPointCloud(outline.n_trace);
                        for(int q=0;q<outline.n_trace;q++)
                        {
                         myPlot.pointclouds->points[q*2]=outline.m_trace[q].x0;
                         myPlot.pointclouds->points[(q*2)+1]=outline.m_trace[q].y0;
                        }
                        cout<<"added points...\n";
                        */
                        pcount++;
                    }
                }
            }
    }
        cout<<"Added "<<pcount<<" bezier polygons\n";



    delete Map;
    delete Map2;
    delete Map3;
}


//int tool_status=0;
int tool_number=0;

void LeftButtonClick(double x,double y)
{
    switch(tool_number)
    {
    case TOOL_LINE:
        break;
    case TOOL_COMPASS:
        break;
    case TOOL_WAYPOINT:
        break;
    case TOOL_PROTRACTOR:
        break;
    case TOOL_ERASER:
        break;
    }
}

void MoveMouse(double x,double y)
{
    switch(tool_number)
    {
    case TOOL_LINE:
        break;
    case TOOL_COMPASS:
        break;
    case TOOL_WAYPOINT:
        break;
    case TOOL_PROTRACTOR:
        break;
    case TOOL_ERASER:
        break;
    }
}

class RasterImageLevel
{
public:
    int n;
    int img_dim;
    irr::video::IImage** img;
    irr::video::ITexture** tex;

    RasterImageLevel():n(0),img_dim(0),img(NULL),tex(NULL)
    {
    }

    init(int nn, int dim)
    {

        img = new irr::video::IImage* [2^(nn-1)];
        tex = new irr::video::ITexture* [2^(nn-1)];
        n=nn;
        img_dim=dim;
    }
};

class RasterImageMgr
{
public:
    RasterImageLevel level1;
    RasterImageLevel level0;
    irr::video::IVideoDriver* videoDriver = NULL;

    void init(irr::video::IVideoDriver* vd,Plot& myPlot,FontDraw& myFont)
    {
        videoDriver=vd;

        irr::core::dimension2du screenSize(512,512);
        level0.init(1,512);
        level0.img[0] = videoDriver->createImage(irr::video::ECF_A8R8G8B8, screenSize);
        level0.tex[0] = videoDriver->addTexture(irr::io::path("image name"), level0.img[0]);

        Scene scene = {0,0,512,512,512,512};

        specialRenderImage(level0.img[0],level0.tex[0],myPlot,myFont,scene,false);

        level1.init(2,512);
        for(int i=0;i<2;i++)
            for(int j=0;j<2;j++)
        {
            level1.img[i*2+j] = videoDriver->createImage(irr::video::ECF_A8R8G8B8, screenSize);
            level1.tex[i*2+j] = videoDriver->addTexture(irr::io::path("image name"), level1.img[i*2+j]);
            Scene scene = {0+i*256,0+j*256,256+i*256,256+j*256,512,512};

            specialRenderImage(level1.img[i*2+j],level1.tex[i*2+j],myPlot,myFont,scene,false);
        }
    }

    void render(Scene scene)
    {
        irr::core::recti srcRect(0,0,512,512);
        irr::video::SColor white(0xffffffff);
        int level;

        if(scene.x1-scene.x0 == 512) level = 0;
        else if(scene.x1-scene.x0== 256) level = 1;
        if(videoDriver && level >=0 && level <= 1)
        {
            if(level == 0)
            {
            videoDriver->draw2DImage(level0.tex[0], irr::core::vector2di(SceneX(0,scene),SceneY(0,scene)), srcRect, 0, white, true);
            }
            else if (level == 1)
            {

            videoDriver->draw2DImage(level1.tex[0], irr::core::vector2di(SceneX(0,scene),SceneY(0,scene)), srcRect, 0, white, true);
            videoDriver->draw2DImage(level1.tex[1], irr::core::vector2di(SceneX(0,scene),SceneY(256,scene)), srcRect, 0, white, true);
            videoDriver->draw2DImage(level1.tex[2], irr::core::vector2di(SceneX(256,scene),SceneY(0,scene)), srcRect, 0, white, true);
            videoDriver->draw2DImage(level1.tex[3], irr::core::vector2di(SceneX(256,scene),SceneY(256,scene)), srcRect, 0, white, true);
            }
        }
    }
};

int main() {
    //irr::core::dimension2du screenSize(640,640);
    irr::core::dimension2du screenSize(1024,600);
    irr::video::SColor white(0xffffffff);
    irr::core::vector2di zeroVector(0);

    MyEventReceiver receiver;

    irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_BURNINGSVIDEO,screenSize,16,false,false,false,&receiver);

    if ( !device ) return 1;

    irr::video::IVideoDriver* videoDriver = device->getVideoDriver();

    //
    /*
    scene::ISceneManager* smgr = device->getSceneManager();

    // add camera
	scene::ICameraSceneNode* camera =
		//smgr->addCameraSceneNodeFPS(0,100.0f,1.2f);
		smgr->addCameraSceneNode();

	camera->setPosition(core::vector3df(2700*2,255*2,2600*2));
	camera->setTarget(core::vector3df(2397*2,343*2,2700*2));
	camera->setFarValue(42000.0f);
	smgr->addMeshSceneNode();
	*/
	//

    videoDriver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
    videoDriver->setTextureCreationFlag(irr::video::ETCF_ALLOW_NON_POWER_2, true);

    irr::core::recti srcRect(0,0,(irr::s32)screenSize.Width,(irr::s32)screenSize.Height);

    irr::video::IImage* img = videoDriver->createImage(irr::video::ECF_A8R8G8B8, screenSize);
    irr::video::IImage* img2 = videoDriver->createImage(irr::video::ECF_A8R8G8B8, screenSize);

    double d = 3421.244;
    std::string astring = patch::to_string(d);
//    Draw(img);
    irr::video::ITexture* tex = videoDriver->addTexture(irr::io::path("image name"), img);
    irr::video::ITexture* tex2 = videoDriver->addTexture(irr::io::path("image name2"), img2);
    cout<<"a z "<<(int)'a'<<" "<<(int)'z'<<"\n";
    bool bBackground=false;
    bool bMouseDown=false;
    bool rMouseDown=false;
    bool letterDown[26];
    bool numberDown[10];

    bool bDrag=false;

    char draw_flags=0;
    Scene scene = {0,0,0,0,screenSize.Width-CLIP_BORDER*2,screenSize.Height-CLIP_BORDER*2};
    scene.x1=scene.x0+scene.width;
    scene.y1=scene.y0+scene.height;
    set_screen_corners(scene,CLIP_BORDER);
    int MouseWheelPos = receiver.GetMouseState().WheelPos;
    double mousex;
    double mousey;
    double toolx;
    double tooly;
    srand(43242);
    Plot myPlot;
/*
    myPlot.addPath();
    PathObject* legend_X = myPlot.paths;

    myPlot.addPath();
    PathObject* legend_Y = myPlot.paths;

    //legend_X->addGraph(GRAPH_LINES,1,1);
    legend_X->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_DIV4,4);
    legend_X->addGraph(GRAPH_LABELS,GRAPH_LABEL_X4,4);

    //legend_Y->addGraph(GRAPH_LINES,1,1);
    legend_Y->addGraph(GRAPH_MARKERS,GRAPH_MARKERS_DIV4,4);
    legend_Y->addGraph(GRAPH_LABELS,GRAPH_LABEL_Y4,4);

    legend_X->addPoint(0,0);
    legend_X->addPoint(100,0);

    legend_Y->addPoint(0,0);
    legend_Y->addPoint(0,100);
*/
    //for(int i=0;i<5;i++)
    //{
    //myPlot.addCircle(rand()%1000-500,rand()%1000-500,rand()%200);
    //myPlot.addLine(rand()%1000-500,rand()%1000-500,rand()%1000-500,rand()%1000-500);
    //}

    myPlot.addArrow();
    myPlot.arrows->setPosition(-50,25,50,-25);

    //myPlot.addCircle(0,0,50);
    //myPlot.addCircle(20,35,75);
    //double t0=PI*3/4;
    //double t1=PI*5/4;
    //double ra=50;
    double    z=0;
   InitMap(myPlot);


    ArbObject* ap = NULL;
    ArbObject* pp = NULL;
    int vcount=0;

    myPlot.islands = ap;

    myPlot.gridspacing=128;

    HDC dc = ::GetDC(0);
    FontDraw myFont(dc);

    SetToolPlot(&myPlot);

    double oldtoolx;
    double oldtooly;
    double clickx;
    double clicky;
    double myangle=0;
    double lasttime=0;
    int tool_status=0;
    tool_number=TOOL_WAYPOINT2;

    RasterImageMgr RasterImgMgr;
    RasterImgMgr.init(videoDriver,myPlot,myFont);

    //myPlot.addArrow();
    //myPlot.arrows->setLengthRotationPosition(10,1000,0.1,0,0);

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




         if(MouseWheelPos != receiver.GetMouseState().WheelPos)
        {
            if(MouseWheelPos < receiver.GetMouseState().WheelPos && scene.x1-scene.x0 > 1)
            {/*
                float s = (scene.x1-scene.x0)/scene.width;
                scene.x0+=s*75;
                scene.y0+=s*75;
                scene.x1-=s*75;
                scene.y1-=s*75;*/
                float s = (scene.x1-scene.x0)*0.25;
                scene.x0+=s;
                scene.x1-=s;
                s = (scene.y1-scene.y0)*0.25;
                scene.y0+=s;
                scene.y1-=s;
                //cout<<"scene: "<<scene.x0<<","<<scene.y0<<","<<scene.x1<<","<<scene.y1<<"\n";
                 cout<<"scene: "<<scene.x1-scene.x0<<"\n";
                //cout<<"vertexes: "<<n_vertexes<<"\n";
                //double d = (PI*2*50/s/5)+(PI*2*30/s/5)+(PI*2*15/s/5);
                //cout<<"lines: "<<d<<"\n";
                if((scene.x1-scene.x0)/myPlot.gridspacing < 3 && myPlot.gridspacing > 1)
                    myPlot.gridspacing/=2;
            }
            else if(MouseWheelPos > receiver.GetMouseState().WheelPos)
            {
               /* float s = (scene.x1-scene.x0)/scene.width;
                scene.x0-=s*108;
                scene.y0-=s*108;
                scene.x1+=s*108;
                scene.y1+=s*108;*/
                 float s = (scene.x1-scene.x0)*0.5;
                scene.x0-=s;
                scene.x1+=s;
                s = (scene.y1-scene.y0)*0.5;
                scene.y0-=s;
                scene.y1+=s;
                //cout<<"scene: "<<scene.x0<<","<<scene.y0<<","<<scene.x1<<","<<scene.y1<<"\n";
                cout<<"scene: "<<scene.x1-scene.x0<<"\n";
                //cout<<"vertexes: "<<n_vertexes<<"\n";
                if((scene.x1-scene.x0)/myPlot.gridspacing > 7)
                    myPlot.gridspacing*=2;
            }
            MouseWheelPos = receiver.GetMouseState().WheelPos;
            //cout<<MouseWheelPos<<"\n";
        }


        toolx = ((((double)receiver.GetMouseState().Position.X-CLIP_BORDER)/scene.width)*(scene.x1-scene.x0))+scene.x0;
        tooly = ((((double)receiver.GetMouseState().Position.Y-CLIP_BORDER)/scene.height)*(scene.y1-scene.y0))+scene.y0;

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
           // toolx = ((((double)receiver.GetMouseState().Position.X-CLIP_BORDER)/scene.width)*(scene.x1-scene.x0))+scene.x0;
           // tooly = ((((double)receiver.GetMouseState().Position.Y-CLIP_BORDER)/scene.width)*(scene.y1-scene.y0))+scene.y0;

            //cout<<"click: "<<toolx<<", "<<tooly<<"\n";
            double inverse_screen= (scene.y1-scene.y0)/scene.width;
            switch(tool_number)
                {
                    case TOOL_WAYPOINT:
                        bDrag = PlotTool_Waypoint::StartDrag(toolx,tooly,inverse_screen);
                    break;
                     case TOOL_WAYPOINT2:
                        bDrag = PlotTool_Waypoint2::StartDrag(toolx,tooly,inverse_screen);
                    break;
                    case TOOL_COMPASS:
                        //bDrag = PlotTool_Compass::StartDrag(toolx,tooly,inverse_screen);
                    break;
                }
            }
            else if(bDrag)
                {
                    switch(tool_number)
                    {
                        case TOOL_WAYPOINT:
                            PlotTool_Waypoint::Drag(toolx,tooly);
                        break;
                        case TOOL_WAYPOINT2:
                            PlotTool_Waypoint2::Drag(toolx,tooly);
                        break;
                        case TOOL_COMPASS:
//                            PlotTool_Compass::Drag(toolx,tooly);
                        break;
                    }
                }
            else
            {

                if(fabs(mousex-receiver.GetMouseState().Position.X) > 0.001 || fabs(mousey-receiver.GetMouseState().Position.Y) > 0.001)
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
                mousex=receiver.GetMouseState().Position.X;
                mousey=receiver.GetMouseState().Position.Y;

                if(bDrag)
                {
                    bDrag = false;
                    switch(tool_number)
                    {
                        case TOOL_WAYPOINT:
                            PlotTool_Waypoint::EndDrag(toolx,tooly);
                        break;
                        case TOOL_WAYPOINT2:
                            PlotTool_Waypoint2::EndDrag(toolx,tooly);
                        break;
                        case TOOL_COMPASS:
//                            PlotTool_Compass::EndDrag(toolx,tooly);
                        break;
                    }
                }
                else if(mousex-clickx == 0 &&
                    mousey-clicky == 0)
                {
                    switch(tool_number)
                    {
                    case TOOL_COMPASS:
                  //     PlotTool_Compass::LeftClick(toolx,tooly);
                    break;
                    case 666:/*
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
                        }*/
                    break;
                    case TOOL_LINE:
           //             PlotTool_Line::LeftClick(toolx,tooly);
                    break;
                    case TOOL_PROTRACTOR:
          //              PlotTool_Protractor::LeftClick(toolx,tooly);
                    break;
                    case TOOL_WAYPOINT:
                        PlotTool_Waypoint::LeftClick(toolx,tooly);
                    break;
                    case TOOL_WAYPOINT2:
                        PlotTool_Waypoint2::LeftClick(toolx,tooly);
                    break;
                    }
                }

            }//if mousedown = true
            else //mousedown = false
            {
                double inverse_screen= (scene.y1-scene.y0)/scene.width;
                switch(tool_number)
                {
                case TOOL_COMPASS:
//                   PlotTool_Compass::MouseMove(mousex,mousey);
                    break;
                case 666:/*
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
                        }*/
                    break;
                case TOOL_LINE:
//                    PlotTool_Line::MouseMove(toolx,tooly);
                    break;
                case TOOL_ERASER:
//                    PlotTool_Eraser::MouseMove(toolx,tooly,(scene.x1-scene.x0)/(0.1*scene.width));
                    break;
                case TOOL_PROTRACTOR:
//                    PlotTool_Protractor::MouseMove(toolx,tooly);
                    break;
                case TOOL_WAYPOINT:
                    PlotTool_Waypoint::MouseMove(toolx,tooly);
                    break;
                case TOOL_WAYPOINT2:
                    PlotTool_Waypoint2::MouseMove(toolx,tooly,inverse_screen);
                    break;

                }//switch tool number
            }

            bMouseDown=false;
        }

        if(receiver.GetMouseState().RightButtonDown == true)
        {
            if(rMouseDown==false)
                rMouseDown=true;
        }
        else if(receiver.GetMouseState().RightButtonDown == false)
        {
             if(rMouseDown==true)
             {
                switch(tool_number)
                    {
                    case TOOL_COMPASS:
//                        PlotTool_Compass::RightClick(toolx,tooly);
                        break;
                    case TOOL_LINE:
                       // PlotTool_Line::MouseMove(toolx,tooly);
                        break;
                    case TOOL_ERASER:
                        //PlotTool_Eraser::MouseMove(toolx,tooly,(scene.x1-scene.x0)/(0.1*scene.width));
                        break;
                    case TOOL_PROTRACTOR:
                       // PlotTool_Protractor::MouseMove(toolx,tooly);
                        break;
                    case TOOL_WAYPOINT:
                        //PlotTool_Waypoint::MouseMove(toolx,tooly);
                        PlotTool_Waypoint::RightClick(toolx,tooly);
                        break;
                    case TOOL_WAYPOINT2:
                        //PlotTool_Waypoint::MouseMove(toolx,tooly);
                        PlotTool_Waypoint2::RightClick(toolx,tooly);
                        break;
                    }//switch tool number
                rMouseDown=false;
             }
        }


/*
        legend_X->m_points[0]=scene.x0;
        legend_X->m_points[1]=scene.y0;
        legend_X->m_points[2]=scene.x1;
        legend_X->m_points[3]=scene.y0;

        legend_Y->m_points[0]=scene.x1;
        legend_Y->m_points[1]=scene.y0;
        legend_Y->m_points[2]=scene.x1;
        legend_Y->m_points[3]=scene.y1;
*/

        if(n_vertexes>3000)
        {
            N_STEPS=N_STEPS>3?N_STEPS-1:N_STEPS;
           // cout<<N_STEPS<<"\n";
            //cout<<"vertexes: "<<n_vertexes<<"\n";
        }

        if(n_vertexes<1000)
        {
            N_STEPS=N_STEPS<8?N_STEPS+1:N_STEPS;
            //cout<<N_STEPS<<"\n";
           // cout<<"vertexes: "<<n_vertexes<<"\n";
        }

        n_vertexes=0;

         if(receiver.IsKeyDown(KEY_KEY_X))
           {
               if(letterDown[(int)'x'-(int)'a'] == false)
               {
                    draw_flags = draw_flags ^ (draw_flags | FLAG_TOGGLE_1);

               }
               letterDown[(int)'x'-(int)'a']=true;
           }
           else
           {
               letterDown[(int)'x'-(int)'a']=false;
           }

//         legend_Y->changed(1);
 //       legend_X->changed(1);


        if(receiver.IsKeyDown(KEY_KEY_F))
           {
               if(letterDown[(int)'f'-(int)'a']==false)
               {
                    renderImage(img,tex,myPlot,myFont,scene,draw_flags | FLAG_MAKE_BMP);
                    cout<<"posa!\n";
                  //  renderImage(img,tex2,myPlot,myFont,scene,false);
                   // bBackground=true;
               }
                letterDown[(int)'f'-(int)'a']=true;
           }
        else
        {
            if(!paused)
            {
                if(bBackground && (scene.x1-scene.x0 == 256 || scene.x1-scene.x0 == 512))
                {
                    renderImage(img,tex,myPlot,myFont,scene,draw_flags);
                }
                else
                    renderImage(img,tex,myPlot,myFont,scene,draw_flags);
            }
            letterDown[(int)'f'-(int)'a'] = false;
        }

         if(receiver.IsKeyDown(KEY_KEY_N))
           {
               if(letterDown[(int)'n'-(int)'a']==false)
               {
                N_STEPS+=1;
                cout<<N_STEPS<<"\n";
                cout<<"vertexes: "<<n_vertexes<<"\n";
                letterDown[(int)'n'-(int)'a']=true;
               }
           }
        else
        {
            letterDown[(int)'n'-(int)'a'] = false;
        }

        if(receiver.IsKeyDown(KEY_KEY_K))
           {
               if(letterDown[(int)'k'-(int)'a']==false)
               {
                PlotTool_Waypoint2::ChangeStyle(-1,toolx,tooly);
                letterDown[(int)'k'-(int)'a']=true;
               }
           }
        else
        {
            letterDown[(int)'k'-(int)'a'] = false;
        }

        if(receiver.IsKeyDown(KEY_KEY_L))
           {
               if(letterDown[(int)'l'-(int)'a']==false)
               {
                PlotTool_Waypoint2::ChangeStyle(-2,toolx,tooly);
                letterDown[(int)'l'-(int)'a']=true;
               }
           }
        else
        {
            letterDown[(int)'l'-(int)'a'] = false;
        }

         if(receiver.IsKeyDown(KEY_KEY_3))
         {
             if(numberDown[3]==false)
             {
              PlotTool_Waypoint2::ChangeStyle(3,toolx,tooly);
              numberDown[3]=true;
             }
         }
         else
            numberDown[3]=false;

         if(receiver.IsKeyDown(KEY_KEY_4))
         {
             if(numberDown[4]==false)
             {
              PlotTool_Waypoint2::ChangeStyle(4,toolx,tooly);
              numberDown[4]=true;
             }
         }
         else
            numberDown[4]=false;

        if(receiver.IsKeyDown(KEY_KEY_5))
         {
             if(numberDown[5]==false)
             {
              PlotTool_Waypoint2::ChangeStyle(5,toolx,tooly);
              numberDown[5]=true;
             }
         }
         else
            numberDown[5]=false;

        if(receiver.IsKeyDown(KEY_KEY_1))
         {
             if(numberDown[1]==false)
             {
              PlotTool_Waypoint2::ChangeStyle(1,toolx,tooly);
              numberDown[1]=true;
             }
         }
         else
            numberDown[1]=false;

        if(receiver.IsKeyDown(KEY_KEY_2))
         {
             if(numberDown[2]==false)
             {
              PlotTool_Waypoint2::ChangeStyle(2,toolx,tooly);
              numberDown[2]=true;
             }
         }
         else
            numberDown[2]=false;

         if(receiver.IsKeyDown(KEY_KEY_M))
           {
               if(letterDown[(int)'m'-(int)'a']==false)
               {
                if(N_STEPS>1)
                    N_STEPS-=1;
                cout<<N_STEPS<<"\n";
                cout<<"vertexes: "<<n_vertexes<<"\n";
                letterDown[(int)'m'-(int)'a']=true;
               }
           }
        else
        {
            letterDown[(int)'m'-(int)'a'] = false;
        }

        if(receiver.IsKeyDown(KEY_KEY_P))
           {
               if(letterDown[(int)'p'-(int)'a']==false)
                  paused=!paused;
                letterDown[(int)'p'-(int)'a']=true;
           }
        else
        {
            letterDown[(int)'p'-(int)'a'] = false;
        }

         if(receiver.IsKeyDown(KEY_KEY_S))
           {
               if(letterDown[(int)'s'-(int)'a']==false)
               {
                   switch(tool_number)
                    {

                    case TOOL_WAYPOINT:
                        PlotTool_Waypoint::Push_S(toolx,tooly);
                        break;
                    case TOOL_WAYPOINT2:
                        PlotTool_Waypoint2::Push_S(toolx,tooly);
                        break;
                    }
               }
                letterDown[(int)'s'-(int)'a']=true;
           }
        else
        {
            letterDown[(int)'s'-(int)'a'] = false;
        }

        if(receiver.IsKeyDown(KEY_KEY_Z))
           {
               if(letterDown[(int)'z'-(int)'a']==false)
                  {
                   switch(tool_number)
                    {
                    case TOOL_COMPASS:

                        break;
                    case 666:

                        break;
                    case TOOL_LINE:

                        break;
                    case TOOL_ERASER:
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
                        break;
                    case TOOL_PROTRACTOR:

                        break;
                    case TOOL_WAYPOINT:
                        PlotTool_Waypoint::Push_Z(toolx,tooly);
                        break;
                    case TOOL_WAYPOINT2:
                        PlotTool_Waypoint2::Push_Z(toolx,tooly);
                        break;
                    }

                  }
                letterDown[(int)'z'-(int)'a']=true;
           }
        else
        {
            letterDown[(int)'z'-(int)'a'] = false;
        }

        if(receiver.IsKeyDown(KEY_KEY_A))
           {
            if(letterDown[(int)'a'-(int)'a']==false)
                {
                    switch(tool_number)
                    {
                    case TOOL_COMPASS:

                        break;
                    case 666:

                        break;
                    case TOOL_LINE:

                        break;
                    case TOOL_ERASER:

                        break;
                    case TOOL_PROTRACTOR:

                        break;
                    case TOOL_WAYPOINT:
                        //PlotTool_Waypoint::ToolChange();
                        break;
                    case TOOL_WAYPOINT2:
                        PlotTool_Waypoint2::ToolChange(toolx,tooly);
                        break;
                    }
                    /*
                tool_number++;
                if(tool_number > 5)
                    tool_number=0;
                switch(tool_number)
                    {
                    case TOOL_COMPASS:
                        cout<<"Tool Selected: Circle\n";
                        break;
                    case 666:
                        cout<<"Tool Selected: Angle\n";
                        break;
                    case TOOL_LINE:
                        cout<<"Tool Selected: Line\n";
                        break;
                    case TOOL_ERASER:
                        cout<<"Tool Selected: Eraser\n";
                        break;
                    case TOOL_PROTRACTOR:
                        cout<<"Tool Selected: Directional Ray\n";
                        break;
                    case TOOL_WAYPOINT:
                        cout<<"Tool Selected: Arrow\n";
                        break;
                    case TOOL_WAYPOINT2:
                        cout<<"Tool Selected: Points\n";
                        break;
                    }
                    */
                }
                letterDown[(int)'a'-(int)'a']=true;
           }
        else
            {
            letterDown[(int)'a'-(int)'a'] = false;
            }

        time = device->getTimer()->getTime();


        videoDriver->beginScene();

        //smgr->drawAll();
        if(bBackground)
        {
            //videoDriver->draw2DImage(tex2, zeroVector, srcRect, 0, white, true);
            //videoDriver->draw2DImage(RasterImgMgr.level0.tex[0], irr::core::vector2di(00), srcRect, 0, white, true);
            RasterImgMgr.render(scene);

        }
        videoDriver->draw2DImage(tex, irr::core::vector2di(00), srcRect, 0, white, true);
        videoDriver->endScene();
    }
    //delete plotobjects;
    img->drop();
   // img2->drop();

    device->drop();
    return 0;
}
