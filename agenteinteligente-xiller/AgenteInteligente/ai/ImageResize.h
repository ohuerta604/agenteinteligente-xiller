

#ifndef ImageResize_h
#define ImageResize_h


#include "vec2Di.h"
#include "basefwt.h"
#include "haar.h"


public class ImageResize
{
public:
        ImageResize();
        //ImageResize(const ImageResize& imresize);
        ~ImageResize();

// Operators
        //const ImageResize& operator=(const ImageResize& imresize);

// Operations
        void init(unsigned int w, unsigned int h, float zoom = 0.125f);         //default zoom x8
        int resize(const unsigned char* pBGR);                         //resize 24bpp [BGR] data
        int resize(const unsigned char* pR,                            //resize separate channels
                   const unsigned char* pG,
                   const unsigned char* pB) const;

// Access
        inline const vec2Di* gety() const;                  //gray converted [rgb] resized data
        inline char** getr() const;                                //2D pointer to r spec
        inline char** getg() const;                                //2D pointer to g spec
        inline char** getb() const;                                //2D pointer to b spec

// Inquiry
        int status() const;

protected:
private:
        ImageResize(const ImageResize& imresize);
        const ImageResize& operator=(const ImageResize& imresize);

        int m_status;              //1-initialized
        unsigned int m_width;      //width of initialized data
        unsigned int m_height;     //height
        float m_zoom;              //zoom coeff
        unsigned char* m_rdata;    //temp r-channel
        unsigned char* m_gdata;    //temp g-channel
        unsigned char* m_bdata;    //temp b-channel

        vec2Di* m_y;               //gray image 0...255.0f
        Haar* m_r;                 //[r,g,b]
        Haar* m_g;
        Haar* m_b;

        inline float rgb2y(int r, int g, int b) const;
        void rgb2y() const;
        float log2(float x) const;


};

/*
      store downscaled RGB for SkinFilter
                        Y  for FaceDetector

      ImageResize r;
      r.init(width, height, zoom);
      r.resize(pBGR);

*/

// Inlines
inline int ImageResize::status() const
{
        return m_status;
}

inline const vec2Di* ImageResize::gety() const
{
        return m_y;
}

char** ImageResize::getr() const
{
        return m_r->getspec2d();
}

char** ImageResize::getg() const
{
        return m_g->getspec2d();
}

char** ImageResize::getb() const
{
        return m_b->getspec2d();
}

inline float ImageResize::log2(float x) const
{
        return log(x) / log(2.0f);
}

inline float ImageResize::rgb2y(int r, int g, int b) const
{
        __declspec(align(16)) const float Y[4] = { 0.2989f, 0.5870f, 0.1140f, 0.0f };
        __declspec(align(16)) float rgb[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        rgb[0] = (float)r;
        rgb[1] = (float)g;
        rgb[2] = (float)b;

        float out[1] = { 0.0f };

        __m128 n = _mm_load_ps(Y);
        __m128 m = _mm_load_ps(rgb);

        __m128 res = _mm_mul_ps(m, n);

        //res = a,b,c,d
        m = _mm_movelh_ps(res, res);     //a,b,a,b
        n = _mm_movehl_ps(res, res);     //c,d,c,d
        m = _mm_add_ps(m, n);            //r0,r1,r0,r1
        n = _mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 3, 0, 1)); //r1,r0,r1,r0
        res = _mm_add_ss(m, n);               //r1+r0

        _mm_store_ss(out, res);
        return out[0];
}



#endif ImageResize_h

