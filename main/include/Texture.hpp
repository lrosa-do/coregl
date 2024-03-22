#pragma once


#include "Config.hpp"
#include <string>
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include "Math.hpp"


enum PixelFormat
{
    GRAYSCALE = 1,     // 8 bit per pixel (no alpha)
    GRAY_ALPHA,        // 8*2 bpp (2 channels)
    R8G8B8,            // 24 bpp
    R8G8B8A8,          // 32 bpp    
};


class  Color 
{
public:
    Color() : r(255), g(255), b(255), a(255) {}
    Color(u8 r, u8 g, u8 b, u8 a) : r(r), g(g), b(b), a(a) {}
    Color(u8 r, u8 g, u8 b) : r(r), g(g), b(b), a(255) {}
    Color(float r, float g, float b, float a) : r((u8)(r * 255.0f)), g((u8)(g * 255.0f)), b((u8)(b * 255.0f)), a((u8)(a * 255.0f)) {}
    Color(u32 color) : r((color >> 24) & 0xFF), g((color >> 16) & 0xFF), b((color >> 8) & 0xFF), a(color & 0xFF) {}


    Color Lerp(const Color& rhs,float t) const
	{
		float invT = 1.0f - t;
		return Color(
			r * invT + rhs.r * t,
			g * invT + rhs.g * t,
			b * invT + rhs.b * t,
			a * invT + rhs.a * t
			);
	}

	std::string ToString() const
	{
		char tempBuffer[128];
		sprintf(tempBuffer,"(%i %i %i %i)",r,g,b,a);
		return std::string(tempBuffer);
	}

	u32 ToUInt() const
	{
		unsigned r = (unsigned) Clamp(((int)(this->r * 255.0f)),0,255);
		unsigned g = (unsigned) Clamp(((int)(this->g * 255.0f)),0,255);
		unsigned b = (unsigned) Clamp(((int)(this->b * 255.0f)),0,255);
		unsigned a = (unsigned) Clamp(((int)(this->a * 255.0f)),0,255);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}

    void Set(u8 r, u8 g, u8 b, u8 a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    static float getRed(u32 value){return (float)((value >> 16) & 0xFF) / 255.0f;}
    static	float getGreen(u32 value)    {    return (float)((value >> 8) & 0xFF) / 255.0f;    }
    static float getBlue(u32 value)    {    return (float)((value >> 0) & 0xFF) / 255.0f;    }
    static float getAlpha(u32 value)    {    return (float)((value >> 24) & 0xFF) / 255.0f;    }


    u8 r, g, b, a;


	
	static const Color WHITE;
	
	static const Color GRAY;
	
	static const Color BLACK;
	
	static const Color RED;
	
	static const Color GREEN;
	
	static const Color BLUE;
	
	static const Color CYAN;
	
	static const Color MAGENTA;
	
	static const Color YELLOW;
	
	static const Color TRANSPARENT;
};




enum WrapMode
{
	ClampToEdge       = 0x812F,
	ClampToBorder     = 0x812D,
	MirroredRepeat    = 0x8370,
	Repeat            = 0x2901 // Default
	
};



enum FilterMode
{
	Nearest           = 0x2600,
	Linear            = 0x2601,
	NearestMipNearest = 0x2700,
	LinearMipNearest  = 0x2701,
	NearestMipLinear  = 0x2702, // This is the default setting
	LinearMipLinear   = 0x2703
};

class  Pixmap
{
public:
    Pixmap();
    ~Pixmap();
    Pixmap(int w, int h, int components);
    Pixmap(int w, int h, int components, unsigned char *data);

    Pixmap(const Pixmap &image,const IntRect &crop);
    Pixmap(const Pixmap &other) = delete;
    Pixmap &operator=(const Pixmap &other) = delete;

    void SetPixel(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a);
    void SetPixel(u32 x, u32 y, u32 rgba);

    u32 GetPixel(u32 x, u32 y) const;
    Color GetPixelColor(u32 x, u32 y) const;

    void Fill(u8 r, u8 g, u8 b, u8 a);
    void Fill(u32 rgba);
    
    bool Save(const std::string &file_name);
    
    void Clear();
    

    bool Load(const std::string &file_name);
    bool LoadFromMemory(const unsigned char *buffer,   unsigned int bytesRead);


    void FlipVertical();
    void FlipHorizontal();




    unsigned char *pixels;
    int components;
    int width;
    int height;
};




class  Texture 
{
    public:
        Texture();
        virtual ~Texture();
           
        u32 GetID() {return id;}

	FilterMode GetMinFilter() const { return MinificationFilter; }
	FilterMode GetMagFilter() const { return MagnificationFilter; }
	WrapMode GetWrapS() const { return HorizontalWrap; }
	WrapMode GetWrapT() const { return VerticalWrap; }

    int GetWidth() {return width;}
    int GetHeight() {return height;}
	
	void SetMinFilter(FilterMode filter);
	void SetMagFilter(FilterMode filter);
	void SetWrapS(WrapMode mode);
	void SetWrapT(WrapMode mode);
	void SetAnisotropicFiltering(float level = -1.0f);

    void Use(u32 unit=0);
    void Update(const Pixmap &pixmap);
    void Update(const unsigned char *buffer, u16 components, int width, int height);

    virtual    void Release();
        
    protected:
        u32 id;   
        WrapMode       HorizontalWrap;
        WrapMode       VerticalWrap;
	    FilterMode      MinificationFilter;
	    FilterMode      MagnificationFilter;
	    float          MaxAnisotropic;
        int width;          
        int height;    

        void createTexture();

        Texture& operator=(const Texture& other) = delete;
        Texture(const Texture& other) = delete;




     
};


class  Texture2D  : public Texture
{
    public:
    Texture2D();

    
    Texture2D(int w, int h);
    Texture2D(const Pixmap &pixmap);
    Texture2D(const std::string &file_name);

    bool Load(const Pixmap &pixmap);
    bool Load(const std::string &file_name);
    bool LoadFromMemory(const unsigned char *buffer,u16 components, int width, int height);
    u32 getID() {return id;}

    private:
          
        s32 components{0};         
};


class  CubemapTexture : public Texture
{
public:

    CubemapTexture(const std::string& Directory,
                   const std::string& PosXFilename,
                   const std::string& NegXFilename,
                   const std::string& PosYFilename,
                   const std::string& NegYFilename,
                   const std::string& PosZFilename,
                   const std::string& NegZFilename);


    bool Load();




private:

    std::string m_fileNames[6];

};