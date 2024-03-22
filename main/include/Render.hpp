#pragma once
#include <string>
#include "Config.hpp"
#include "Array.hpp"
#include "Math.hpp"
#include "Utils.hpp"
#include "Texture.hpp"
#include "Shader.hpp"


class Driver;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

};

class Material
{
public:
    Material() { Release();}
    virtual ~Material() {}
    Material(const std::string &name)    {        m_name = name;    }
    const std::string &GetName() const { return m_name; }
    virtual void Release(){};
    virtual void Bind(Driver *driver,Shader *shader) ;
private:
    std::string m_name;
};

class  TextureMaterial : public Material
{
public:
  

    TextureMaterial( Texture2D * texture);
    void SetTexture(Texture2D * texture);

    private:
    u32 layer;
    Texture2D *m_texture{nullptr};
    void Bind(Driver *driver, Shader *shader) ;

};


class  VertexFormat
{
public:

   
    enum Usage
    {
        POSITION = 1,
        NORMAL = 2,
        COLOR = 3,
        TANGENT = 4,
        BINORMAL = 5,
        BLENDWEIGHTS = 6,
        BLENDINDICES = 7,
        TEXCOORD0 = 8,
        TEXCOORD1 = 9,
        TEXCOORD2 = 10,
        TEXCOORD3 = 11,
        TEXCOORD4 = 12,
        TEXCOORD5 = 13,
        TEXCOORD6 = 14,
        TEXCOORD7 = 15
    };

    
    class Element
    {
    public:
        Usage usage;
        unsigned int size;
        Element();
        Element(Usage usage, unsigned int size);
        bool operator == (const Element& e) const;
        bool operator != (const Element& e) const;
    };


    VertexFormat(){_vertexSize=0;};


    VertexFormat(const Element* elements, unsigned int elementCount);
    ~VertexFormat();

    const Element& getElement(unsigned int index) const;
    unsigned int getElementCount() const;
    unsigned int getVertexSize() const;
    bool operator == (const VertexFormat& f) const;
    bool operator != (const VertexFormat& f) const;

private:

    Array<Element> _elements;
    unsigned int _vertexSize;
};



enum PrimitiveType
    {
        TRIANGLES = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        LINES = GL_LINES,
        LINE_STRIP = GL_LINE_STRIP,
        POINTS = GL_POINTS
    };

class  MeshBuffer 
{
public:
    MeshBuffer( Driver *driver, bool dynamic=false);
     ~MeshBuffer();

   
    void CreateVertexBuffer(const VertexFormat& vertexFormat, unsigned int vertexCount);
    void CreateIndexBuffer(unsigned int indexCount);

    void SetVertexData(const void* vertexData);
    void SetIndexData(const void* indexData);

    void Render(int mode,int count);
    void Release();


    void* Lock();
    bool  Unlock();

    private:
    friend class Mesh;

    Driver *m_driver;


      
    int m_numVertices = 0;
    int m_numIndices  = 0;

    bool m_useIndices = false;
    bool m_dynamic = false;
    unsigned int m_ibo = 0;
    unsigned int m_vbo = 0;
    unsigned int m_vao = 0;
    VertexFormat m_vertexFormat;
};


const int VBO_POSITION  = 0x00000001;
const int VBO_NORMAL  = 0x00000002;
const int VBO_COLOR  = 0x00000004;
const int VBO_TANGENT  = 0x00000008;
const int VBO_BINORMAL  = 0x00000010;
const int VBO_BLENDWEIGHTS  = 0x00000020;
const int VBO_BLENDINDICES  = 0x00000040;
const int VBO_TEXCOORD0  = 0x00000080;
const int VBO_TEXCOORD1  = 0x00000100;
const int VBO_INDICES = 0x00000200; 


class  Surface
{public:
    Surface(Driver *driver,const VertexFormat& vertexFormat,int material =0, bool dynamic = false); 
    virtual ~Surface()  ;

    int GetMaterial() const { return m_material; }  
    void SetMaterial(int material) { m_material = material; }   

    void Render(int mode,int count);
    void Render(int mode);
    void Render();
    
    

    int AddVertex(const glm::vec3 &position);
    int AddVertex(float x, float y, float z);

    const void* GetVertices() const;
    void* GetVertices();

    void* GetIndices();
    const void* GetIndices() const;


    void TexturePlanarMapping(float resolution=0.001f);
    void TexturePlanarMapping(float resolutionS, float resolutionT, u8 axis, const glm::vec3& offset);
    //Changes backfacing triangles to frontfacing
    void FlipFaces();
    void FlipNormals();





    int AddVertex(const glm::vec3 &position,const glm::vec2 &texCoord);
    int AddVertex(float x, float y, float z, float u, float v);

    int AddVertex(const glm::vec3 &position,const glm::vec2 &texCoord,const glm::vec3 &normal);
    int AddVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz);



    void VertexPosition(int index, const glm::vec3 &position);
    void VertexPosition(int index, float x, float y, float z);

    void VertexNormal(int index, const glm::vec3 &normal);
    void VertexNormal(int index, float x, float y, float z);

    void VertexTexCoord(int index, const glm::vec2 &texCoord);
    void VertexTexCoord(int index, float u, float v);

    int AddFace(int v0, int v1, int v2);

    void SetName(const std::string &name) { m_name = name; }
    const std::string &GetName() const { return m_name; }

    void Transform(const glm::mat4 &transform);
   

    void Init();
    void Upload();
    void Release();

    void CalculateNormals();

    void CalculateSmothNormals(bool angleWeighted=false);

    void CalculateBoundingBox();

    BoundingBox GetBoundingBox() const { return m_boundingBox; }
    


    void VertexUpload(MeshBuffer *buffer ,const VertexFormat& vertexFormat);


    void Clear();

    u32 GetVertexCount() const { return (u32) positions.size(); }
    u32 GetIndexCount() const { return (u32)indices.size(); }

    

private:
    struct VertexBuffer
    {   
        std::string name;
        s32         size;
        s32         usage;
        unsigned int         id;
        VertexBuffer()
        {
            size = 0;
			usage = 0;
			id = 0;
            name = "";
        }
    };
    void AddBuffer(VertexBuffer *buffer)
    {
        this->buffers.push_back(buffer);
        m_vbo[buffer->name] = buffer;
    }
private:
    friend class Mesh;  
    friend class MeshBuffer;
    friend class MeshBuilder;
    u32 m_material;
    u32 m_stride ;
    bool m_dynamic;
    u32 flags;
    bool isDirty;
    std::string m_name;
    BoundingBox m_boundingBox;
    Driver *m_driver;


    
    VertexFormat  m_vertexFormat;
    Array<glm::vec3> positions;
    Array<glm::vec3> normals;
    Array<glm::vec2> texCoords;
    Array<glm::vec4> colors;
    Array<unsigned int> indices;
    Array<VertexBuffer*> buffers;
    Map<std::string, VertexBuffer*> m_vbo;
    u32                         IBO;
    u32                         VAO;

   
};

class  Mesh : public IDrop
{
    public:
    Mesh(Driver *driver,const VertexFormat& vertexFormat,bool dynamic = false);


    Surface *AddSurface(int material=0);
    Surface *GetSurface(int index);

    const Surface *GetSurface(int index) const;

    int      AddMaterial(Material *material);
    Material *GetMaterial(int index);

    int GetSurfaceCount() const { return (int) m_surfaces.size(); }
    int GetMaterialCount() const { return (int)m_materials.size(); }
    
    void Release();

    void Transform(const glm::mat4 &transform);
 
    void Render(Shader *shader);
    void Render(Shader *shader,int mode);
    void Render(Shader *shader,int mode,int count);

    BoundingBox GetBoundingBox() const { return m_boundingBox; }
    void CalculateBoundingBox();

    void OrderByMaterial();

    private:
    friend class MeshBuilder;
    friend class Surface;

    ~Mesh();

    Driver *m_driver;
    
    Array<Surface*> m_surfaces;
    Array<Material*> m_materials;
    VertexFormat  m_vertexFormat;
    bool m_dynamic = false;
    BoundingBox m_boundingBox;




};


class  MeshImporter
{
public:


        struct MeshMaterial 
        {
        std::string name;
        std::string texPath;
        std::string normalPath;
        std::string specularPath;
        std::string maskPath;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        MeshMaterial()
            {
                name = "";
                texPath = "";
                normalPath = "";
                specularPath = "";
                maskPath = "";
                ambient = glm::vec3(0.0f);
                diffuse = glm::vec3(0.0f);
                specular = glm::vec3(0.0f);
            }
        //with no trivial copy-assignment; use copy-assignment or copy-initialization instead 
        MeshMaterial(const MeshMaterial& other)
        {
            name = other.name;
            texPath = other.texPath;
            normalPath = other.normalPath;
            specularPath = other.specularPath;
            maskPath = other.maskPath;
            ambient = other.ambient;
            diffuse = other.diffuse;
            specular = other.specular;
        }
        MeshMaterial& operator=(const MeshMaterial& other) 
        {
            if (this == &other)
                return *this;
            name = other.name;
            texPath = other.texPath;
            normalPath = other.normalPath;
            specularPath = other.specularPath;
            maskPath = other.maskPath;
            ambient = other.ambient;
            diffuse = other.diffuse;
            specular = other.specular;
            return *this;
        }
        
        };





    virtual Mesh* Load(const VertexFormat& vertexFormat,void* data, u64 size) { (void)vertexFormat;(void) data; (void)size; return nullptr; };
    virtual Mesh* Load(const VertexFormat& vertexFormat,const std::string& fileName) { (void)vertexFormat;(void)fileName; return nullptr; };
    virtual void Clear() {};



protected:
    friend class Driver;
    Driver *m_driver;

    MeshImporter(Driver*driver);
    virtual ~MeshImporter();

    Array<MeshMaterial> m_materials;
    Map<std::string, MeshMaterial> m_mtlMap;

};

class  MeshImporterOBJ : public MeshImporter
{
public:
    MeshImporterOBJ(Driver *driver) : MeshImporter(driver) {};

    Mesh* Load(const VertexFormat& vertexFormat,const std::string& fileName) override;
    void Clear() override;




private:



    bool LoadMaterial(const std::string& fileName);
    void pushVertex(unsigned int v, unsigned int vt, unsigned int vn, bool hasUV, bool hasNormal);

    int GetIndex(const std::string& name);



    Array<glm::vec3> temp_vertices;
    Array<glm::vec2> temp_uvs;
    Array<glm::vec3> temp_normals;

    Array<Vertex> vertices;
    Array<unsigned int> indices;

    Map<std::string, unsigned int> verticesMap;
  
};

class  MeshStaticH3D : public MeshImporter
{
    private:
        
public:
    MeshStaticH3D(Driver *driver) : MeshImporter(driver) {};
    Mesh* Load(const VertexFormat& vertexFormat,const std::string& fileName) override;


};


class  MeshBuilder : public IDrop
{
    public:

        static MeshBuilder& Instance();


    void SetLoadPath(const std::string &path);
    std::string GetLoadPath() const;

    void SetTexturePath(const std::string &path);
    std::string GetTexturePath() const;


    Mesh*  CreateCube(const VertexFormat& vertexFormat,float width, float height, float length);
    Mesh * CreatePlane(const VertexFormat& vertexFormat,int stacks, int slices, int tilesX, int tilesY);
    Mesh *CreateSphere(const VertexFormat& vertexFormat,int stacks, int slices);
    Mesh *CreateCylinder(const VertexFormat& vertexFormat,int stacks, int slices);
    Mesh *CreateCone(const VertexFormat& vertexFormat,int stacks, int slices);
    Mesh *CreateTorus(const VertexFormat& vertexFormat,int stacks, int slices, float innerRadius, float outerRadius);
    Mesh* CreateHillsPlane(const VertexFormat& vertexFormat,
    const FloatSize& tileSize,
    const IntSize& tileCount,
    float hillHeight,
    const FloatSize& countHills,
    const FloatSize& textureRepeatCount);

 Mesh* CreateTerrain(const VertexFormat& vertexFormat, Pixmap& heightmap, const Pixmap& colors, const FloatSize& stretchSize, f32 maxHeight, const IntSize& maxVtxBlockSize);

Mesh * LoadMesh(const VertexFormat& vertexFormat,const std::string &fileName);

private:
    friend class Driver;
    MeshBuilder(Driver *driver);
    ~MeshBuilder();

    static MeshBuilder* instance;
    std::string m_loadPath;
    std::string m_texturePath;
    Driver *m_driver;
    MeshStaticH3D   *m_h3dLoader;
    MeshImporterOBJ *m_objLoader;


};
