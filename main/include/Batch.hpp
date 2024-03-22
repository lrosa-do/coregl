#pragma once
#include <string>
#include "Config.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Math.hpp"
#include "Array.hpp"


#define PI 3.14159265358979323846f
#define EPSILON 0.000001f
#define DEG2RAD (PI/180.0f)
#define RAD2DEG (180.0f/PI)

class Driver;

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
struct VertexBuffer 
{
    int elementCount;          
    Array<float>  vertices;        
    Array<float>  texcoords;           
    Array<unsigned char> colors;     
    Array<unsigned int>  indices;    
    unsigned int vaoId;         
    unsigned int vboId[4];
} ;

struct DrawCall 
{
    int mode;                   
    int vertexCount;          
    int vertexAlignment;       
   unsigned int textureId;
};


class  RenderBatch : public IDrop
{
public:


    void Init(int numBuffers, int bufferElements);
 

    void SetColor(const Color &color);
    void SetColor(float r , float g , float b);
    void SetColor(u8 r, u8 g, u8 b, u8 a);

    void Line2D(int startPosX, int startPosY, int endPosX, int endPosY);
    void Line2D(const Vector2 &start,const Vector2 &end);

    void Line3D(const Vector3 &start, const Vector3 &end);
    void Line3D(float startX, float startY, float startZ, float endX, float endY, float endZ);

    void Box(const BoundingBox &box);
    void Box(const BoundingBox &box,const glm::mat4 &transform);

    void Cube(const Vector3 &position, float width, float height, float depth,bool wire=true);
    void Sphere(const Vector3 &position, float radius, int rings, int slices,bool wire=true);
    void Cone(const Vector3& position, float radius, float height, int segments, bool wire);
    void Cylinder(const Vector3& position, float radius, float height, int segments, bool wire);
    void Capsule(const Vector3& position, float radius, float height, int segments, bool wire);


    void Grid(int slices, float spacing,bool axes=true);
    
    void Quad(const Vector2 *coords, const Vector2 *texcoords);
    void Quad(Texture2D *texture, float x, float y,float width, float height);
    void Quad(Texture2D *texture, const FloatRect &src,float x, float y,float width, float height);
    void Quad(u32 texture, float x, float y,float width, float height);

    void BeginTransform(const glm::mat4 &transform);
    void EndTransform();

   
    void Render();

    void SetMode(int mode);                        
       

          
    void Vertex2f(float x, float y);          
    void Vertex3f(float x, float y, float z);     
    void TexCoord2f(float x, float y);          

    void SetTexture(unsigned int id);


    private:
        bool CheckRenderBatchLimit(int vCount);


    int bufferCount;            
    int currentBuffer;         
    int drawCounter;           
    float currentDepth;         
    int vertexCounter;
    GLuint defaultTextureId;
    bool use_matrix;
    glm::mat4 modelMatrix;


    Driver *m_driver;

    Array<DrawCall*> draws;
    Array<VertexBuffer*> vertexBuffer;

    float texcoordx, texcoordy;         
    u8 colorr, colorg, colorb, colora;

private:
    friend class Driver;
    friend class Device;


     void Release();
        
    RenderBatch(  Driver *driver);
    virtual ~RenderBatch();
    

};



struct  Glyph
{
    int value;    
    int offsetX;  
    int offsetY;  
    int advanceX;     
} ;

class  Font : public IDrop
{
    public:


        bool Load(const std::string& filePath);

        void SetClip(int x, int y, int w, int h);
        void EnableClip(bool enable);


        Vector2 GetTextSize(const char *text);
        float     GetTextWidth(const char *text);

         void    Print (const char *text, float x, float y);
         void    Print (float x, float y, const char *text, ...);
        
        void SetTexture(Texture2D *texture) {this->texture = texture;}
        void SetBatch(RenderBatch *batch) {this->batch = batch;}

    private:
        friend class RenderBatch;
        friend class Assets;


        Driver *m_driver;

        RenderBatch *batch;
        float fontSize;
        float spacing;
        Vector2 coords[4];
        Vector2 texcoords[4];
        Color color;
        Texture2D *texture;
        bool enableClip;
        IntRect clip;
        int m_baseSize;          
        int m_glyphCount;        
        int m_glyphPadding;      
   
        Array<IntRect> m_recs;  
        Array<Glyph> m_glyphs;   
        int textLineSpacing{15};

        void drawTextCodepoint(int codepoint,float x, float y);
        int  getGlyphIndex( int codepoint);
        void drawTexture(const FloatRect &src,float x, float y,float w, float h);

        Font();
        ~Font();
};
