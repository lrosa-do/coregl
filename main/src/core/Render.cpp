#include "Render.hpp"
#include "Assets.hpp"
#include "stb_image.h"
#include "DriverGL.hpp"
#include "Device.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
 extern double GetTime();







extern glm::vec3 GetAngleWeight(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);

//***********************************************************************************************************


VertexFormat::VertexFormat(const Element* elements, unsigned int elementCount)
    : _vertexSize(0)
{
    for (unsigned int i = 0; i < elementCount; ++i)
    {
        Element element;
        memcpy(&element, &elements[i], sizeof(Element));
        _elements.push_back(element);
        _vertexSize += element.size * sizeof(float);
    }
}

VertexFormat::~VertexFormat(){}

const VertexFormat::Element& VertexFormat::getElement(unsigned int index) const{return _elements[index];}

unsigned int VertexFormat::getElementCount() const{    return (unsigned int)_elements.size();}
unsigned int VertexFormat::getVertexSize() const{    return _vertexSize;}

bool VertexFormat::operator == (const VertexFormat& f) const
{
    if (_elements.size() != f._elements.size())
        return false;
    for (size_t i = 0, count = _elements.size(); i < count; ++i)
    {
        if (_elements[i] != f._elements[i])
            return false;
    }

    return true;
}
bool VertexFormat::operator != (const VertexFormat& f) const{    return !(*this == f);}
VertexFormat::Element::Element() :    usage(POSITION), size(0){}
VertexFormat::Element::Element(Usage usage, unsigned int size) :    usage(usage), size(size){}
bool VertexFormat::Element::operator == (const VertexFormat::Element& e) const{    return (size == e.size && usage == e.usage);}
bool VertexFormat::Element::operator != (const VertexFormat::Element& e) const{    return !(*this == e);}

//***********************************************************************************************************
 void Material::Bind(Driver *driver,Shader *shader) 
 {
    (void)driver;
    (void)shader;
 }
TextureMaterial::TextureMaterial(Texture2D * texture): Material("TextureMaterial")
{
    if (texture)
    {
            m_texture = texture;
    } 
    layer=0;

}

void TextureMaterial::Bind(Driver *driver, Shader *shader) 
{
    (void)shader;
    if (m_texture)
    {
        driver->SetTexture(m_texture,layer);
    }
}
void TextureMaterial::SetTexture(Texture2D * texture)
{
    m_texture = texture;

}



MeshBuffer::MeshBuffer(Driver *driver, bool dynamic)
{
    m_vbo = 0;
    m_ibo = 0;
    m_vao = 0;
    m_dynamic = dynamic;
    m_useIndices = false;
    m_driver = driver;

}

MeshBuffer::~MeshBuffer()
{
    m_driver = nullptr;
    Release();
}

void MeshBuffer::CreateVertexBuffer(const VertexFormat& vertexFormat, unsigned int vertexCount)
{
    m_vertexFormat = vertexFormat; 
    m_numVertices = vertexCount;
    glGenBuffers(1, &this->m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexFormat.getVertexSize() * vertexCount, NULL, m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) ;

    if (!m_vao)
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
    } else 
    {
        glBindVertexArray(m_vao);
    }
    glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);
    unsigned int offset = 0;
    for (unsigned int i = 0; i < vertexFormat.getElementCount(); ++i)
    {
        const VertexFormat::Element& element = vertexFormat.getElement(i);
     
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, (GLint)element.size, GL_FLOAT, GL_FALSE,(GLsizei)vertexFormat.getVertexSize(), (void*)(offset * sizeof(float)));

     
        offset += element.size;
    }

    glBindVertexArray(0);
}

void MeshBuffer::CreateIndexBuffer(unsigned int indexCount)
{
        if (!m_vao)
        {
            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);
        } else 
        {
            glBindVertexArray(m_vao);
        }
        m_useIndices = true;
        m_numIndices = indexCount;
        glGenBuffers(1, &this->m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), NULL, m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) ;
        glBindVertexArray(0);
}

 void MeshBuffer::SetIndexData(const void* indexData)
 {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_numIndices * sizeof(unsigned int), indexData);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
 }

void MeshBuffer::SetVertexData(const void *vertexData)
{
    glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_numVertices * m_vertexFormat.getVertexSize(), vertexData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void MeshBuffer::Render(int mode,int count)
{
   

    glBindVertexArray(m_vao);
    if (m_useIndices) 
    {
  
        m_driver->DrawElements(mode, count, GL_UNSIGNED_INT, 0);
    }
    else 
    {
       
        m_driver->DrawArrays(mode, 0, count);
    }
}

void MeshBuffer::Release()
{

    if (m_vao != 0) 
    {
        glDeleteVertexArrays(1, &m_vao);
    }

    if (m_useIndices && m_ibo != 0) 
    {
        glDeleteBuffers(1, &m_ibo);
    }

    if (m_vbo != 0) 
    {
        glDeleteBuffers(1, &m_vbo);
    }

   m_vbo = 0;
   m_ibo = 0;


}

void *MeshBuffer::Lock()
{
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    
}

bool MeshBuffer::Unlock()
{
    
    bool ret = glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return ret;
}

//***********************************************************************************************************

Mesh::Mesh(Driver *driver,const VertexFormat& vertexFormat,bool dynamic):m_driver(driver), m_vertexFormat(vertexFormat), m_dynamic(dynamic)
{
    
}

Mesh::~Mesh()
{
    m_driver=nullptr;
    Release();
}

Surface *Mesh::AddSurface(int material)
{
    Surface *buffer = new Surface(m_driver,m_vertexFormat,material, this->m_dynamic);
    m_surfaces.push_back(buffer);
    return buffer;
}

Surface *Mesh::GetSurface(int index)
{
    if (index < 0 || index >= (int)m_surfaces.size()) 
    {
        Log(2,"Invalid surface index");
        return nullptr;
    }
    return m_surfaces[index];
}

int Mesh::AddMaterial(Material *material)
{
    m_materials.push_back(material);
    return (int)m_materials.size() - 1;
}


Material *Mesh::GetMaterial(int index)
{
    if (GetMaterialCount() == 0) 
    {
        return nullptr;
    }
    if (index < 0 || index >= (int)m_materials.size()) 
    {
       // Log(2,"Invalid material index: %d total: %d",index, (int)m_materials.size());
        return nullptr;
    }
    return m_materials[index];
}
void Mesh::CalculateBoundingBox()
{
    m_boundingBox.Reset();
    for (u32 i = 0; i < m_surfaces.size(); ++i) 
    {
        m_boundingBox.Add(m_surfaces[i]->GetBoundingBox());
    }
}

void Mesh::Release()
{
    for (u32 i = 0; i < m_surfaces.size(); ++i) 
    {
        delete m_surfaces[i];
    }


    for (u32 i = 0; i < m_materials.size(); ++i) 
    {
        delete m_materials[i];
    }


    m_surfaces.clear();
    m_materials.clear();

   
}

void Mesh::Transform(const glm::mat4 &transform)
{
    for (u32 i = 0; i < m_surfaces.size(); ++i) 
    {
        m_surfaces[i]->Transform(transform);
    }
    CalculateBoundingBox();
}

static s32 CompareSurfaces(Surface *a, Surface *b)
{
    return a->GetMaterial() - b->GetMaterial();
 
}

void Mesh::OrderByMaterial()
{
    m_surfaces.QuickSort(CompareSurfaces);
}


void Mesh::Render(Shader *shader)
{
    
    for ( u32 i = 0; i < m_surfaces.size(); ++i) 
    {
        if (!m_driver->IsIgnoreMaterial() ) 
        {
            Material *material = GetMaterial(m_surfaces[i]->GetMaterial());
            if (material!=nullptr) 
            {
               material->Bind(m_driver,shader);
            }
        }
        m_surfaces[i]->Render();
    }

   
}
    

void Mesh::Render(Shader *shader,int mode)
{
  
    for ( u32 i = 0; i < m_surfaces.size(); ++i) 
    {
        if (!m_driver->IsIgnoreMaterial() ) 
        {
            Material *material = GetMaterial(m_surfaces[i]->GetMaterial());
            if (material!=nullptr) 
            {
               material->Bind(m_driver,shader);
            }
        }
        m_surfaces[i]->Render(mode);
    }
  

}

void Mesh::Render(Shader *shader,int mode, int count)
{
   
    
    for ( u32 i = 0; i < m_surfaces.size(); ++i) 
    {
        if (!m_driver->IsIgnoreMaterial() ) 
        {
            Material *material = GetMaterial(m_surfaces[i]->GetMaterial());
            if (material!=nullptr) 
            {
               material->Bind(m_driver,shader);
            }
        }
        m_surfaces[i]->Render(mode,count);
    }
  
  
}

Surface::Surface(Driver *driver,const VertexFormat &vertexFormat, int material, bool dynamic)
{
    m_vertexFormat = vertexFormat;
    m_material = material;
    m_dynamic = dynamic;
    flags = VBO_POSITION | VBO_INDICES;
    m_stride = vertexFormat.getVertexSize();
    isDirty = true;
    IBO=0;
    VAO=0;
    m_name = "Surface";
    m_driver = driver;
    Init();
   
    
}

Surface::~Surface()
{

    m_driver = nullptr;
    Release();
}

void Surface::Transform(const glm::mat4 &transform)
{

    bool hasNormals = normals.size() == positions.size();
    for (u32 i = 0; i < positions.size(); ++i) 
    {
        positions[i] = glm::vec3(transform * glm::vec4(positions[i], 1.0f));
        if (hasNormals) 
        {
            normals[i] = glm::vec3(transform * glm::vec4(normals[i], 0.0f));
        }
        
    }
    flags |= VBO_POSITION;
    if (hasNormals) 
    {
        flags |= VBO_NORMAL;
    }
    CalculateBoundingBox();

    isDirty = true;
}

void Surface::Init()
{

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    isDirty = true;


     for (GLuint j = 0; j <(GLuint) m_vertexFormat.getElementCount(); ++j)
    {
            const VertexFormat::Element& e = m_vertexFormat.getElement(j);           
            if (e.usage == VertexFormat::POSITION) 
            {
                flags |= VBO_POSITION;
                VertexBuffer * buffer = new VertexBuffer();
                glGenBuffers(1, &buffer->id);
                glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
                glEnableVertexAttribArray(j);
                glVertexAttribPointer(j, 3, GL_FLOAT, GL_FALSE, (GLint)sizeof(glm::vec3), 0);

                buffer->size  = e.size;
                buffer->usage = e.usage;
                buffer->name = "POSITION";
            //    Log(1,"POSITION");
                
                AddBuffer(buffer);
            }
            else
            if (e.usage == VertexFormat::TEXCOORD0) 
            {
                flags |= VBO_TEXCOORD0;

                VertexBuffer * buffer = new VertexBuffer();
                glGenBuffers(1, &buffer->id);
                glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
                glEnableVertexAttribArray(j);
                glVertexAttribPointer(j, 2, GL_FLOAT, GL_FALSE, (GLint)sizeof(glm::vec2), 0);
                buffer->size  = e.size;
                buffer->usage = e.usage;
                buffer->name = "TEXCOORD0";
              //  Log(1,"TEXCOORD0");
                AddBuffer(buffer);

                

            }
            else if (e.usage == VertexFormat::NORMAL) 
            {
                flags |= VBO_NORMAL;
                VertexBuffer * buffer = new VertexBuffer();
                glGenBuffers(1, &buffer->id);
                glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
                glEnableVertexAttribArray(j);
                glVertexAttribPointer(j, 3, GL_FLOAT, GL_FALSE, (GLint)sizeof(glm::vec3), 0);
                buffer->size  = e.size;
                buffer->usage = e.usage;
                buffer->name = "NORMAL";
            //   Log(1,"NORMAL");
                AddBuffer(buffer);
            } else
                
            if (e.usage == VertexFormat::TANGENT) 
            {
                flags |= VBO_TANGENT;
               
                VertexBuffer * buffer = new VertexBuffer();
                glGenBuffers(1, &buffer->id);
                glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    
                glEnableVertexAttribArray(j);
                glVertexAttribPointer(j, 3, GL_FLOAT, GL_FALSE, (GLint)sizeof(glm::vec3), 0);

                buffer->size  = e.size;
                buffer->usage = e.usage;
                buffer->name = "TANGENT";
                Log(1,"TANGENT");
                AddBuffer(buffer);
            } else          
            if (e.usage == VertexFormat::COLOR)
            {
                flags |= VBO_COLOR;
                VertexBuffer * buffer = new VertexBuffer();
                glGenBuffers(1, &buffer->id);
                glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
           
                glEnableVertexAttribArray(j);
                glVertexAttribPointer(j, (GLint)e.size, GL_FLOAT, GL_FALSE, (GLint)e.size * sizeof(float), (void*)(0));
                buffer->size  = e.size;
                buffer->usage = e.usage;
                buffer->name = "COLOR";
                Log(1,"COLOR");
                AddBuffer(buffer);
            } 
        }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Surface::FlipFaces()
{
    const u32 idxcnt = (u32)indices.size();
    if (idxcnt == 0) return;
    
     unsigned int *idx = indices.Data();

    for (u32 i = 0; i < idxcnt; i += 3) 
    {
       const unsigned int tmp = idx[i+1];
        idx[i+1] = idx[i + 2];
        idx[i + 2] = tmp;
    }
    isDirty = true;
    flags |= VBO_INDICES;

}

void Surface::TexturePlanarMapping(float resolution)
{

     const u32 idxcnt = (u32)indices.size();
     const u32 texcnt = (u32)texCoords.size();
    if (idxcnt == 0 || texcnt==0) return;

    


    for (u32 i = 0; i < idxcnt; i += 3) 
    {
        Plane3D plane(positions[indices[i]], positions[indices[i + 1]], positions[indices[i + 2]]);
        glm::vec3 normal = plane.Normal();
        normal.x = fabs(normal.x);
        normal.y = fabs(normal.y);
        normal.z = fabs(normal.z);

        if (normal.x > normal.y && normal.x > normal.z) 
        {
            for (u32 j = 0; j < 3; ++j) 
            {
                texCoords[indices[i + j]].x = positions[indices[i + j]].y * resolution;
                texCoords[indices[i + j]].y = positions[indices[i + j]].z * resolution;
            }
        } else 
        if (normal.y > normal.x && normal.y > normal.z) 
        {
            for (u32 j = 0; j < 3; ++j) 
            {
                texCoords[indices[i + j]].x = positions[indices[i + j]].x * resolution;
                texCoords[indices[i + j]].y = positions[indices[i + j]].z * resolution;
            }
        } else 
        {
            for (u32 j = 0; j < 3; ++j) 
            {
                texCoords[indices[i + j]].x = positions[indices[i + j]].x * resolution;
                texCoords[indices[i + j]].y = positions[indices[i + j]].y * resolution;
            }
        }
           
    }



    flags |= VBO_TEXCOORD0;
    isDirty = true;
}
void Surface::TexturePlanarMapping(float resolutionS, float resolutionT, u8 axis, const glm::vec3& offset)
{
     const u32 idxcnt = (u32)indices.size();
     const u32 texcnt = (u32)texCoords.size();
     if (idxcnt == 0 || texcnt==0) return;


    for (u32 i = 0; i < idxcnt; i += 3)
    {

        if (axis==0)
        {
                for (u32 j = 0; j < 3; ++j) 
                {
                    texCoords[indices[i + j]].x =0.5f +(positions[indices[i + j]].z + offset.z) * resolutionS;
                    texCoords[indices[i + j]].y =0.5f -(positions[indices[i + j]].y + offset.y) * resolutionT;
                

                }
        } else if (axis==1)
        {
                for (u32 j = 0; j < 3; ++j) 
                {
                    texCoords[indices[i + j]].x =0.5f +(positions[indices[i + j]].x + offset.x) * resolutionS;
                    texCoords[indices[i + j]].y =1.0f -(positions[indices[i + j]].z + offset.z) * resolutionT;
                }
        } else if (axis==2)
        {
                for (u32 j = 0; j < 3; ++j) 
                {
                    texCoords[indices[i + j]].x =0.5f +(positions[indices[i + j]].x + offset.x) * resolutionS;
                    texCoords[indices[i + j]].y =0.5f -(positions[indices[i + j]].y + offset.y) * resolutionT;
                }
        }
    
    }



    flags |= VBO_TEXCOORD0;
    isDirty = true;
}

void Surface::Upload()
{

    for (u32 i = 0; i < buffers.size(); ++i) 
    {
        VertexBuffer *buffer = buffers[i];

      //  Log(1,"Upload usage %d id %d size %d name %s",buffer->usage, buffer->id ,buffer->size, buffer->name.c_str());
        if (buffer->usage == VertexFormat::POSITION) 
        {
            if (flags & VBO_POSITION) 
            {
                glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
                glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
             //  Log(1,"Upload POSITION  ");
            }
        }
        else if (buffer->usage == VertexFormat::TEXCOORD0) 
        {
            if (flags & VBO_TEXCOORD0) 
            {
                if (texCoords.size() != positions.size()) 
                {
                    for (size_t i = 0; i < positions.size(); ++i) 
                    {
                        texCoords.push_back(glm::vec2(0.0f, 0.0f));
                    }
                }
             //   Log(1,"Upload TEXCOORD0  ");
                glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
                glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            }

        }
        else if (buffer->usage == VertexFormat::NORMAL) 
        {
            if (flags & VBO_NORMAL) 
            {

                if (normals.size() != positions.size()) 
                {
                    for (size_t i = 0; i < positions.size(); ++i) 
                    {
                        normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                    }
                }

            //    Log(1,"Upload NORMAL  ");
                glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
                glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            }
        }
        else if (buffer->usage == VertexFormat::TANGENT) 
        {
            // if (flags & VBO_TANGENT) 
            // {
            //     if (tangents.size() != positions.size()) 
            //     {
            //         for (size_t i = 0; i < positions.size(); ++i) 
            //         {
            //             tangents.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            //         }
            //     }

            //     glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
            //     glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            // }

            // glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
            // glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        }
        else if (buffer->usage == VertexFormat::COLOR) 
        {

            if (colors.size() != positions.size()) 
            {
                for (size_t i = 0; i < positions.size(); ++i) 
                {
                    colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), &colors[0], m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        }
    }

    if (flags & VBO_INDICES) 
    {
       // Log(1,"Upload INDICES  ");
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    flags = 0;
    isDirty = false;

}

void Surface::Release()
{

    if (VAO != 0) 
    {
        glDeleteVertexArrays(1, &VAO);
    }
    if (IBO != 0) 
    {
        glDeleteBuffers(1, &IBO);
    }
    for (u32 i = 0; i < buffers.size(); ++i) 
    {
        VertexBuffer *buffer = buffers[i];
    
        glDeleteBuffers(1, &buffer->id);
        buffer->id = 0;
        delete buffer;
    }

    buffers.clear();

}

void Surface::Render(int mode,int count)
{
    if (isDirty) 
    {
        Upload();
    }

  

    glBindVertexArray(VAO);
    m_driver->DrawElements(mode, count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Surface::Render(int mode)
{
   
    this->Render(mode, this->GetIndexCount());
}
void Surface::Render()
{
    this->Render(GL_TRIANGLES, this->GetIndexCount());
}

const void* Surface::GetVertices() const
{
	return positions.const_pointer();
}
void* Surface::GetVertices()
{
	return positions.pointer();
}

void* Surface::GetIndices()
{
    	return indices.pointer();

}
const void* Surface::GetIndices() const
{
		return indices.const_pointer();
}


int Surface::AddVertex(const glm::vec3 &position)
{
    positions.push_back(position);
    flags |= VBO_POSITION;
    texCoords.push_back(glm::vec2(1.0f));
    normals.push_back(glm::vec3(1.0f));

    m_boundingBox.AddPoint(position);
    isDirty = true;
    return (int)positions.size() - 1;
}

int Surface::AddVertex(float x, float y, float z)
{
    glm::vec3 position(x, y, z);
    positions.push_back(position);
    texCoords.push_back(glm::vec2(1.0f));
    normals.push_back(glm::vec3(1.0f));
    flags |= VBO_POSITION;
    isDirty = true;
    m_boundingBox.AddPoint(position);
    return (int)positions.size() - 1;
}

int Surface::AddVertex(const glm::vec3 &position,const glm::vec2 &texCoord)
{
    positions.push_back(position);
    texCoords.push_back(texCoord);
    m_boundingBox.AddPoint(position);
    flags |= VBO_POSITION | VBO_TEXCOORD0;
     isDirty = true;
    return (int)positions.size() - 1;
}

int Surface::AddVertex(float x, float y, float z, float u, float v)
{
    glm::vec3 position(x, y, z);
    positions.push_back(position);
    m_boundingBox.AddPoint(position);
    texCoords.push_back(glm::vec2(u, v));
    flags |= VBO_POSITION | VBO_TEXCOORD0;
     isDirty = true;
    return (int)positions.size() - 1;
}

int Surface::AddVertex(const glm::vec3 &position,const glm::vec2 &texCoord,const glm::vec3 &normal)
{

    positions.push_back(position);
    m_boundingBox.AddPoint(position);
    texCoords.push_back(texCoord);
    normals.push_back(normal);
    flags |= VBO_POSITION | VBO_TEXCOORD0 | VBO_NORMAL;
     isDirty = true;
    return (int)positions.size() - 1;
}

int Surface::AddVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz)
{
     glm::vec3 position(x, y, z);
    positions.push_back(position);
    m_boundingBox.AddPoint(position);

    texCoords.push_back(glm::vec2(u, v));
    normals.push_back(glm::vec3(nx, ny, nz));
    flags |= VBO_POSITION | VBO_TEXCOORD0 | VBO_NORMAL;
     isDirty = true;
    return (int)positions.size() - 1;
}



void Surface::VertexPosition(int index, const glm::vec3 &position)
{
    m_boundingBox.AddPoint(position);
    positions[index] = position;
    flags |= VBO_POSITION;
     isDirty = true;
}

void Surface::VertexPosition(int index, float x, float y, float z)
{
    positions[index] = glm::vec3(x, y, z);
     m_boundingBox.AddPoint(positions[index]);
    flags |= VBO_POSITION;
     isDirty = true;
}

void Surface::VertexNormal(int index, const glm::vec3 &normal)
{
    normals[index] = normal;
    flags |= VBO_NORMAL;
     isDirty = true;
}

void Surface::VertexNormal(int index, float x, float y, float z)
{
    normals[index] = glm::vec3(x, y, z);
    flags |= VBO_NORMAL;
     isDirty = true;
}

void Surface::VertexTexCoord(int index, const glm::vec2 &texCoord)
{
    texCoords[index] = texCoord;
    flags |= VBO_TEXCOORD0;
     isDirty = true;
}

void Surface::VertexTexCoord(int index, float u, float v)
{
    texCoords[index] = glm::vec2(u, v);
    flags |= VBO_TEXCOORD0;
     isDirty = true;
}

int Surface::AddFace(int v0, int v1, int v2)
{
    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(v2);
    flags |= VBO_INDICES;
     isDirty = true;
    return (int)indices.size() - 3;
}



void Surface::CalculateNormals()
{

    if (indices.size() > 0) 
    {

        if (normals.size() != positions.size()) 
        {
            for (size_t i = 0; i < positions.size(); ++i) 
            {
                normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            }
        }
        for (u32 i = 0; i < GetIndexCount(); i += 3)
        {
            Plane3D plane = Plane3D(positions[indices[i]], positions[indices[i + 1]], positions[indices[i + 2]]);
            plane.Normalise();
            glm::vec3 normal = plane.Normal();
            normals[indices[i]] = normal;
            normals[indices[i + 1]] = normal;
            normals[indices[i + 2]] = normal;

        }


         flags |= VBO_NORMAL;
         isDirty = true;
    }

       
}

void Surface::Clear()
{
    positions.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();
    colors.clear();
    m_boundingBox.Reset();
}

void Surface::CalculateSmothNormals(bool angleWeighted)
{

    if (indices.size() > 0) 
    {
        if (normals.size() != positions.size()) 
        {
            for (size_t i = 0; i < positions.size(); ++i) 
            {
                normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
            }
        }
        for (size_t i = 0; i < indices.size(); i += 3) 
        {
            glm::vec3 v0 = positions[indices[i + 0]];
            glm::vec3 v1 = positions[indices[i + 1]];
            glm::vec3 v2 = positions[indices[i + 2]];

            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            glm::vec3 weight(1.f, 1.f, 1.f);
            if (angleWeighted)
            {
                weight = GetAngleWeight(v0, v1, v2);
			    
            }



            normals[indices[i + 0]] += weight.x*normal;
            normals[indices[i + 1]] += weight.y*normal;
            normals[indices[i + 2]] += weight.z*normal;
        }

        for (size_t i = 0; i < normals.size(); ++i) 
        {
            normals[i] = glm::normalize(normals[i]);
        }
          flags |= VBO_NORMAL;
           isDirty = true;
    }
  
}

void Surface::CalculateBoundingBox()
{
    m_boundingBox.Reset();
    for (size_t i = 0; i < positions.size(); ++i) 
    {
        m_boundingBox.AddPoint(positions[i]);
    }
}

 void Surface::VertexUpload(MeshBuffer *buffer ,const VertexFormat& vertexFormat)
 {
     Array<float> vertexData;

   
     u32 count = vertexFormat.getElementCount();
     

    for (u32 i = 0; i <(u32) this->GetVertexCount(); ++i)
    {
        vertexData.push_back(this->positions[i].x);
        vertexData.push_back(this->positions[i].y);
        vertexData.push_back(this->positions[i].z);
   
        for (u32 j = 0; j < count; ++j)
        {
            const VertexFormat::Element& e = vertexFormat.getElement(j);           
           
            if (e.usage == VertexFormat::TEXCOORD0) 
            {
                vertexData.push_back(this->texCoords[i].x);
                vertexData.push_back(this->texCoords[i].y);
            }
            else if (e.usage == VertexFormat::NORMAL) 
            {
                vertexData.push_back(this->normals[i].x);
                vertexData.push_back(this->normals[i].y);
                vertexData.push_back(this->normals[i].z);
            } else
            if (e.usage == VertexFormat::COLOR)
            {
                if (e.size==3)
                {
                        vertexData.push_back(1.0f);
                        vertexData.push_back(1.0f);
                        vertexData.push_back(1.0f);
                } else
                {
                    vertexData.push_back(1.0f);
                    vertexData.push_back(1.0f);
                    vertexData.push_back(1.0f);
                    vertexData.push_back(1.0f);
                }
            } 
        }
    }
    buffer->SetVertexData(vertexData.const_pointer());

 }

//***********************************************************************************************************

MeshBuilder* MeshBuilder::instance = nullptr; 

MeshBuilder::~MeshBuilder()
{
    m_driver = nullptr;
    delete m_h3dLoader;
    delete m_objLoader;
    instance = nullptr;
    Log(0,"Removing MeshBuilder");
}

MeshBuilder::MeshBuilder(Driver *driver)
{
    if (instance != nullptr) 
    {
        Log(0,"MeshBuilder already created");
        return;
    }

    m_driver = driver;

    m_texturePath = "assets/textures/";
    m_loadPath = "assets/meshes/";

    m_h3dLoader = new MeshStaticH3D(m_driver);
    m_objLoader = new MeshImporterOBJ(m_driver);

    if (instance == nullptr) 
    {
        instance = this;
    }
}

MeshBuilder& MeshBuilder::Instance()
{
    return *instance;
}

void MeshBuilder::SetLoadPath(const std::string &path)
{
    m_loadPath = path;
}

std::string MeshBuilder::GetLoadPath() const
{
    return m_loadPath;
}

void MeshBuilder::SetTexturePath(const std::string &path)
{
    m_texturePath = path;
}

std::string MeshBuilder::GetTexturePath() const
{
    return m_texturePath;
}

Mesh* MeshBuilder::LoadMesh(const VertexFormat& vertexFormat,const std::string &fileName)
{
    std::string ext = ExtractFileExtension(fileName);
    if (ext == "h3d") 
    {
        return m_h3dLoader->Load(vertexFormat,m_loadPath + fileName);
    } else if (ext == "obj") 
    {
        return m_objLoader->Load(vertexFormat,m_loadPath + fileName);
    } else 
    {
        Log(2,"Unknown file format: %s",fileName.c_str());
    }
    return CreateCube(vertexFormat,1.0f,1.0f,1.0f);
}



Mesh* MeshBuilder::CreateCube(const VertexFormat& vertexFormat,float width, float height, float length)
{
    Mesh *mesh = new Mesh(m_driver,vertexFormat);

   
     width /= 2;
	 height /= 2;
	 length /= 2;

    
    Surface *surf = mesh->AddSurface(0);
    surf->AddVertex(-width,-height,-length);
	surf->AddVertex(-width, height,-length);
	surf->AddVertex(width, height,-length);
	surf->AddVertex(width,-height,-length);

	surf->AddVertex(-width,-height, length);
	surf->AddVertex(-width, height, length);
	surf->AddVertex(width, height, length);
	surf->AddVertex(width,-height, length);

	surf->AddVertex(-width,-height, length);
	surf->AddVertex(-width, height, length);
	surf->AddVertex(width, height, length);
	surf->AddVertex(width,-height, length);

	surf->AddVertex(-width,-height,-length);
	surf->AddVertex(-width, height,-length);
	surf->AddVertex(width, height,-length);
	surf->AddVertex(width,-height,-length);

	surf->AddVertex(-width,-height, length);
	surf->AddVertex(-width, height, length);
	surf->AddVertex(width, height, length);
	surf->AddVertex(width,-height, length);

	surf->AddVertex(-width,-height,-length);
	surf->AddVertex(-width, height,-length);
	surf->AddVertex(width, height,-length);
	surf->AddVertex(width,-height,-length);

	surf->VertexNormal(0,0.0,0.0,-1.0);
	surf->VertexNormal(1,0.0,0.0,-1.0);
	surf->VertexNormal(2,0.0,0.0,-1.0);
	surf->VertexNormal(3,0.0,0.0,-1.0);

	surf->VertexNormal(4,0.0,0.0,1.0);
	surf->VertexNormal(5,0.0,0.0,1.0);
	surf->VertexNormal(6,0.0,0.0,1.0);
	surf->VertexNormal(7,0.0,0.0,1.0);

	surf->VertexNormal(8,0.0,-1.0,0.0);
	surf->VertexNormal(9,0.0,1.0,0.0);
	surf->VertexNormal(10,0.0,1.0,0.0);
	surf->VertexNormal(11,0.0,-1.0,0.0);

	surf->VertexNormal(12,0.0,-1.0,0.0);
	surf->VertexNormal(13,0.0,1.0,0.0);
	surf->VertexNormal(14,0.0,1.0,0.0);
	surf->VertexNormal(15,0.0,-1.0,0.0);

	surf->VertexNormal(16,-1.0,0.0,0.0);
	surf->VertexNormal(17,-1.0,0.0,0.0);
	surf->VertexNormal(18,1.0,0.0,0.0);
	surf->VertexNormal(19,1.0,0.0,0.0);

	surf->VertexNormal(20,-1.0,0.0,0.0);
	surf->VertexNormal(21,-1.0,0.0,0.0);
	surf->VertexNormal(22,1.0,0.0,0.0);
	surf->VertexNormal(23,1.0,0.0,0.0);

	surf->VertexTexCoord(0,0.0,1.0);
	surf->VertexTexCoord(1,0.0,0.0);
	surf->VertexTexCoord(2,1.0,0.0);
	surf->VertexTexCoord(3,1.0,1.0);

	surf->VertexTexCoord(4,1.0,1.0);
	surf->VertexTexCoord(5,1.0,0.0);
	surf->VertexTexCoord(6,0.0,0.0);
	surf->VertexTexCoord(7,0.0,1.0);

	surf->VertexTexCoord(8,0.0,1.0);
	surf->VertexTexCoord(9,0.0,0.0);
	surf->VertexTexCoord(10,1.0,0.0);
	surf->VertexTexCoord(11,1.0,1.0);

	surf->VertexTexCoord(12,0.0,0.0);
	surf->VertexTexCoord(13,0.0,1.0);
	surf->VertexTexCoord(14,1.0,1.0);
	surf->VertexTexCoord(15,1.0,0.0);

	surf->VertexTexCoord(16,0.0,1.0);
	surf->VertexTexCoord(17,0.0,0.0);
	surf->VertexTexCoord(18,1.0,0.0);
	surf->VertexTexCoord(19,1.0,1.0);

	surf->VertexTexCoord(20,1.0,1.0);
	surf->VertexTexCoord(21,1.0,0.0);
	surf->VertexTexCoord(22,0.0,0.0);
	surf->VertexTexCoord(23,0.0,1.0);

	surf->VertexTexCoord(0,0.0,1.0);
	surf->VertexTexCoord(1,0.0,0.0);
	surf->VertexTexCoord(2,1.0,0.0);
	surf->VertexTexCoord(3,1.0,1.0);

	surf->VertexTexCoord(4,1.0,1.0);
	surf->VertexTexCoord(5,1.0,0.0);
	surf->VertexTexCoord(6,0.0,0.0);
	surf->VertexTexCoord(7,0.0,1.0);

	surf->VertexTexCoord(8,0.0,1.0);
	surf->VertexTexCoord(9,0.0,0.0);
	surf->VertexTexCoord(10,1.0,0.0);
	surf->VertexTexCoord(11,1.0,1.0);

	surf->VertexTexCoord(12,0.0,0.0);
	surf->VertexTexCoord(13,0.0,1.0);
	surf->VertexTexCoord(14,1.0,1.0);
	surf->VertexTexCoord(15,1.0,0.0);

	surf->VertexTexCoord(16,0.0,1.0);
	surf->VertexTexCoord(17,0.0,0.0);
	surf->VertexTexCoord(18,1.0,0.0);
	surf->VertexTexCoord(19,1.0,1.0);

	surf->VertexTexCoord(20,1.0,1.0);
	surf->VertexTexCoord(21,1.0,0.0);
	surf->VertexTexCoord(22,0.0,0.0);
	surf->VertexTexCoord(23,0.0,1.0);

	surf->AddFace(0,1,2); // front
	surf->AddFace(0,2,3);
	surf->AddFace(6,5,4); // back
	surf->AddFace(7,6,4);
	surf->AddFace(6+8,5+8,1+8); // top
	surf->AddFace(2+8,6+8,1+8);
	surf->AddFace(0+8,4+8,7+8); // bottom
	surf->AddFace(0+8,7+8,3+8);
	surf->AddFace(6+16,2+16,3+16); // right
	surf->AddFace(7+16,6+16,3+16);
	surf->AddFace(0+16,1+16,5+16); // left
	surf->AddFace(0+16,5+16,4+16);

    mesh->m_boundingBox.Add(surf->GetBoundingBox());


    

  


    return mesh;
}

Mesh * MeshBuilder::CreatePlane(const VertexFormat& vertexFormat,int stacks, int slices, int tilesX, int tilesY)
{

     Mesh *mesh = new Mesh(m_driver,vertexFormat);
    
    Surface *surface = mesh->AddSurface(0);
  
    glm::vec3 center(-9999999.0f, 0.0f, -9999999.0f);


  
  for (int i = 0; i <= stacks; ++i) 
  {
        float y = static_cast<float>(i) / static_cast<float>(stacks) * tilesY;
        for (int j = 0; j <= slices; ++j) 
        {
            float x = static_cast<float>(j) / static_cast<float>(slices) * tilesX;
            
                if (x>center.x) center.x = x;
                if (y>center.z) center.z = y;
                if (x<center.x) center.x = x;
                if (y<center.z) center.z = y;

            surface->AddVertex(x, 0.0f, y, x ,y, 0.0f, 1.0f, 0.0f);
            
        }
    }

    for (u32 i =0; i < surface->GetVertexCount(); ++i)
    {
       glm::vec3 &v = surface->positions[i];
       v.x -= center.x * 0.5f;
       v.z -= center.z * 0.5f;
    } 
  

    for (int i = 0; i < stacks; ++i) 
    {
        for (int j = 0; j < slices; ++j) 
        {
            u16 index = (slices + 1) * i + j;
            surface->AddFace(index, index + slices + 1, index + slices + 2);
            surface->AddFace(index, index + slices + 2, index + 1);
        }
    }
  
  
    if (surface->normals.size() == surface->positions.size())
    {

        for (u32 i = 0; i < surface->GetIndexCount(); i += 3)
        {
            Plane3D plane = Plane3D(surface->positions[surface->indices[i]], surface->positions[surface->indices[i + 1]], surface->positions[surface->indices[i + 2]]);
            plane.Normalise();
            glm::vec3 normal = plane.Normal();
            surface->normals[surface->indices[i]] = normal;
            surface->normals[surface->indices[i + 1]] = normal;
            surface->normals[surface->indices[i + 2]] = normal;

        }
    }
    return mesh;
}

Mesh *MeshBuilder::CreateCylinder(const VertexFormat& vertexFormat,int stacks, int slices)
{

  
     Mesh *mesh = new Mesh(m_driver,vertexFormat);
    
    Surface *surface = mesh->AddSurface(0);

   
    const float pi = 3.14159265359f;
    const float stackHeight = 1.0f / static_cast<float>(stacks);
    const float sliceAngle = 2.0f * pi / static_cast<float>(slices);



    // vértices para os lados do cilindro
    for (int i = 0; i <= stacks; ++i) {
        float y = -0.5f + static_cast<float>(i) * stackHeight;
        for (int j = 0; j <= slices; ++j) {
            float x = cos(static_cast<float>(j) * sliceAngle);
            float z = sin(static_cast<float>(j) * sliceAngle);

             surface->AddVertex(x, y, z, static_cast<float>(j) / slices, static_cast<float>(i) / stacks);
        }
    }

    for (int i = 0; i < stacks; ++i) 
    {
        for (int j = 0; j < slices; ++j) 
        {
            int index = (slices + 1) * i + j;
   
                surface->AddFace(index, index + slices + 1, index + slices + 2);
                surface->AddFace(index, index + slices + 2, index + 1);
        }
    }

    

    int topSurfaceStartIndex= surface->GetVertexCount();
    surface->AddVertex(0.0f, 0.5f, 0.0f, 0.5f, 0.5f); // Vértice do topo

    for (int i = 0; i <= slices; ++i) 
    {
        float x = cos(static_cast<float>(i) * sliceAngle);
        float z = sin(static_cast<float>(i) * sliceAngle);
        surface->AddVertex(x, 0.5f, z, 0.5f * (x + 1.0f), 0.5f * (z + 1.0f));
    }

    for (int i = 0; i < slices; ++i) 
    {
        surface->AddFace(topSurfaceStartIndex, topSurfaceStartIndex + (i + 1) % slices + 1, topSurfaceStartIndex + i + 1);
    }


      
       


        int bottomSurfaceStartIndex = surface->GetVertexCount();

        surface->AddVertex(0.0f, -0.5f, 0.0f, 0.5f, 0.5f); // Vértice da base

        for (int i = 0; i <= slices; ++i) {
            float x = cos(static_cast<float>(i) * sliceAngle);
            float z = sin(static_cast<float>(i) * sliceAngle);

            surface->AddVertex(x, -0.5f, z, 0.5f * (x + 1.0f), 0.5f * (z + 1.0f));
        }

        for (int i = 0; i < slices; ++i) 
        {
           surface->AddFace(bottomSurfaceStartIndex, bottomSurfaceStartIndex + i + 1, bottomSurfaceStartIndex + (i + 1) % slices + 1);
        }


        surface->CalculateSmothNormals();


    return mesh;
}

Mesh *MeshBuilder::CreateSphere(const VertexFormat& vertexFormat,int stacks, int slices)
{
     
     Mesh *mesh = new Mesh(m_driver,vertexFormat);
    
    Surface *surface = mesh->AddSurface(0);
          const float pi = 3.14159265359f;

    for (int i = 0; i <= stacks; ++i) 
    {
        float phi = static_cast<float>(i) * pi / static_cast<float>(stacks); // Ângulo phi
        for (int j = 0; j <= slices; ++j) 
        {
            float theta = static_cast<float>(j) * 2.0f * pi / static_cast<float>(slices); // Ângulo theta

            
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            float u = static_cast<float>(j) / static_cast<float>(slices);
            float v = static_cast<float>(i) / static_cast<float>(stacks);

            
             surface->AddVertex(x, y, z, u,v); 
        }
    }

    
    for (int i = 0; i < stacks; ++i) 
    {
        for (int j = 0; j < slices; ++j) 
        {
            int index = (slices + 1) * i + j;
             
              

             surface->AddFace(index, index + 1, index + slices + 2);
            surface->AddFace(index, index + slices + 2, index + slices + 1);


        }
    }
    
    surface->CalculateSmothNormals();

    return mesh;
}



Mesh *MeshBuilder::CreateCone(const VertexFormat& vertexFormat,int stacks, int slices)
{

  
     Mesh *mesh = new Mesh(m_driver,vertexFormat);
    
    Surface *surface = mesh->AddSurface(0);
    const float pi = 3.14159265359f;

    const float stackHeight = 1.0f / static_cast<float>(stacks) ;
    const float sliceAngle = 2.0f * pi / static_cast<float>(slices);
    
  
   


    for (int i = 0; i <= stacks; ++i) 
    {
        float y = -0.5f + static_cast<float>(i) * stackHeight;
        float radius = 0.5f - y; // Reduzir o raio à medida que subimos o cone
        for (int j = 0; j <= slices; ++j) 
        {
            float x = radius * cos(static_cast<float>(j) * sliceAngle);
            float z = radius * sin(static_cast<float>(j) * sliceAngle);


            surface->AddVertex(x, y, z, static_cast<float>(j) / slices, static_cast<float>(i) / stacks);
        }
    }
    

        
        for (int i = 0; i < stacks; ++i) 
        {
            for (int j = 0; j < slices; ++j) 
            {
                int index = (slices + 1) * i + j;
                 surface->AddFace(index,
                index + slices + 1,
                index + slices + 2);

                surface->AddFace(index,
                index + slices + 2,
                index + 1);
            }
        }

    //floor 


    


    float yBottom = -0.5f;
    
    int baseStartIndex = surface->GetVertexCount();

     surface->AddVertex(0.0f, yBottom, 0.0f, 0.5f, 0.5f); // Vértice central da base

    for (int i = 0; i <= slices; ++i) 
    {
        float x = cos(static_cast<float>(i) * sliceAngle);
        float z = sin(static_cast<float>(i) * sliceAngle);
          surface->AddVertex(x, yBottom, z, 0.5f * (x + 1.0f), 0.5f * (z + 1.0f));
    }




    for (int i = 0; i < slices; ++i) 
    {
            surface->AddFace(baseStartIndex,
            baseStartIndex + i + 1,
            baseStartIndex + (i + 1) % slices + 1);
    }
     
    surface->CalculateSmothNormals();
 
    return mesh;
}



Mesh *MeshBuilder::CreateTorus(const VertexFormat& vertexFormat,int stacks, int slices,float innerRadius, float outerRadius)
{

   
     Mesh *mesh = new Mesh(m_driver,vertexFormat);
    
    Surface *surface = mesh->AddSurface(0);

    const float pi = 3.14159265359f;
    const float stackAngle = 2.0f * pi / static_cast<float>(stacks);
    const float sliceAngle = 2.0f * pi / static_cast<float>(slices);

    for (int i = 0; i <= stacks; ++i) 
    {
        float u = static_cast<float>(i) * stackAngle;

        for (int j = 0; j <= slices; ++j) 
        {
            float v = static_cast<float>(j) * sliceAngle;

            float x = (outerRadius + innerRadius * cos(v)) * cos(u);
            float y = (outerRadius + innerRadius * cos(v)) * sin(u);
            float z = innerRadius * sin(v);

            float textureU = static_cast<float>(i) / stacks;
            float textureV = static_cast<float>(j) / slices;

            surface->AddVertex(x, y, z, textureU, textureV);
        }
    }

    for (int i = 0; i < stacks; ++i) 
    {
        for (int j = 0; j < slices; ++j) 
        {
            int index = (slices + 1) * i + j;

            surface->AddFace(index,
            index + slices + 1,
            index + slices + 2);

            surface->AddFace(index,
            index + slices + 2,
            index + 1);
        }
   
    }
     
    surface->CalculateSmothNormals(true);
    mesh->m_boundingBox.Add(surface->GetBoundingBox());
    surface->Upload();
  
    return mesh;
}

Mesh* MeshBuilder::CreateHillsPlane(const VertexFormat& vertexFormat,
    const FloatSize& tileSize,
    const IntSize& tc,
    float hillHeight,
    const FloatSize& ch,
    const FloatSize& textureRepeatCount)
{
    Mesh* mesh = new Mesh(m_driver,vertexFormat);

    Surface* surface = mesh->AddSurface(0);

    IntSize          tileCount = tc;
    FloatSize       countHills = ch;




    if (countHills.width < 0.01f)
        countHills.width = 1.f;
    if (countHills.height < 0.01f)
        countHills.height = 1.f;

    // center
    const glm::vec2 center((tileSize.width * tileCount.width) * 0.5f, (tileSize.height * tileCount.height) * 0.5f);


    const FloatSize tx(
        textureRepeatCount.width / tileCount.width,
        textureRepeatCount.height / tileCount.height);

    ++tileCount.height;
    ++tileCount.width;

    u32 x;

    f32 sx = 0.f, tsx = 0.f;
    for (x = 0; x <(u32) tileCount.width; ++x)
    {
        f32 sy = 0.f, tsy = 0.f;
        for (u32 y = 0; y <(u32) tileCount.height; ++y)
        {
            float vx = sx - center.x;
            float vz = sy - center.y;
            float vy = hillHeight * sinf(vx * countHills.width * 2.0f * 3.14159f / center.x) *
                cosf(vz * countHills.height * 2.0f * 3.14159f / center.y);

            //   hillHeight * (sin((x / ch.x) * countHills.x * 2.0f * 3.14159f) * cos((z / ch.y) * countHills.y * 2.0f * 3.14159f));
            surface->AddVertex(vx, vy, vz, tsx, 1.0f - tsy, 0.0f, 1.0f, 0.0f);
            sy += tileSize.height;
            tsy += tx.height;
        }
        sx += tileSize.width;
        tsx += tx.width;
    }

    for (x = 0; x < (u32)tileCount.width - 1; ++x)
    {
        for (u32 y = 0; y < (u32)tileCount.height - 1; ++y)
        {
            const s32 index = x * tileCount.height + y;
            surface->AddFace(index, index + 1, index + tileCount.height);
            surface->AddFace(index + 1, index + 1 + tileCount.height, index + tileCount.height);
        }
    }

    if (surface->normals.size() == surface->positions.size())
    {
    
        for (u32 i = 0; i < surface->GetIndexCount(); i += 3)
        {
            Plane3D plane = Plane3D(surface->positions[surface->indices[i]], surface->positions[surface->indices[i + 1]], surface->positions[surface->indices[i + 2]]);
            plane.Normalise();
            glm::vec3 normal = plane.Normal();
            surface->normals[surface->indices[i]] = normal;
            surface->normals[surface->indices[i + 1]] = normal;
            surface->normals[surface->indices[i + 2]] = normal;

        }
    }

  
  
    return mesh;
}


Mesh *MeshBuilder::CreateTerrain(const VertexFormat& vertexFormat, Pixmap &heightmap,
 const Pixmap &colors,const FloatSize& stretchSize, f32 maxHeight, const IntSize &maxVtxBlockSize)

{
   Mesh *mesh = new Mesh(m_driver,vertexFormat);

   	const s32 borderSkip =1;
    
   

	const double tm = GetTime();
    const double    startTime = (double)tm * 100;
	const IntSize hMapSize= IntSize(heightmap.width, heightmap.height);
	const IntSize tMapSize= IntSize(colors.width, colors.height);

	const glm::vec2 thRel(static_cast<f32>(tMapSize.width) / hMapSize.width, static_cast<f32>(tMapSize.height) / hMapSize.height);

//	maxHeight /= 255.0f; // height step per color value

	glm::ivec2 processed(0,0);
    int index =0;
    while (processed.y<hMapSize.height)
	{
		while(processed.x<hMapSize.width)
		{
			IntSize blockSize = maxVtxBlockSize;
	
			if (processed.x + blockSize.width > hMapSize.width)				blockSize.width = hMapSize.width - processed.x;
			
            if (processed.y + blockSize.height > hMapSize.height)				blockSize.height = hMapSize.height - processed.y;

		    Surface * surface = mesh->AddSurface(index);
            index++;

			int y;
			glm::vec2 pos(0.f, processed.y*stretchSize.height);
			const glm::vec2 bs(1.f/blockSize.width, 1.f/blockSize.height);
			glm::vec2 tc(0.f, 0.5f*bs.y);
			for (y=0; y<blockSize.height; ++y)
			{
				pos.x=processed.x*stretchSize.width;
				tc.x=0.5f*bs.x;
				for (int x=0; x<blockSize.width; ++x)
				{
                    Color color = heightmap.GetPixelColor(x+processed.x, y+processed.y);
					const f32 height = (color.r/255.0f) * maxHeight;

                    surface->AddVertex(pos.x, height, pos.y, tc.x,tc.y, 0.0f, 1.0f, 0.0f);

					pos.x += stretchSize.width;
					tc.x += bs.x;
				}
				pos.y += stretchSize.height;
				tc.y += bs.y;
			}
            mesh->m_boundingBox.Add(surface->m_boundingBox);

	
			s32 c1 = 0;
			for (y=0; y< blockSize.height-1; ++y)
			{
				for (u32 x=0; x<(u32)blockSize.width-1; ++x)
				{
					const s32 c = c1 + x;

                    surface->AddFace(c,	c + blockSize.width,c + 1);

					surface->AddFace(c + 1,c + blockSize.width,c + 1 + blockSize.width);
				}
				c1 += blockSize.width;
			}

			// recalculate normals
		  	surface->CalculateNormals();

			if (surface->GetVertexCount() > 0)
            {
          
				c8 textureName[64];
       
                IntRect clip = IntRect(
                    (int)floor(processed.x*thRel.x), 
                    (int)floor(processed.y*thRel.y),
                    (int)floor(blockSize.width*thRel.x), 
                    (int)floor(blockSize.height*thRel.y));

                    sprintf(textureName, "terrain_%f_%u.png",startTime, mesh->GetSurfaceCount());
                    
                    Pixmap paint(colors,clip);
            


                    Texture2D *texture =  Assets::Instance().CreateTexture(paint);

                    mesh->AddMaterial(new TextureMaterial( texture));

                     Assets::Instance().AddTexture(std::string(textureName),texture);


		
				

         
				Log(0, "Generated terrain texture (%dx%d): %s",
				 		paint.width,				 		paint.height,				 		textureName);


		
			}
            surface->Upload();

	
			// keep on processing
			processed.x += maxVtxBlockSize.width - borderSkip;
		}

		// keep on processing
		processed.x = 0;
		processed.y += maxVtxBlockSize.height - borderSkip;
	}

    const double tend = GetTime();
    double timeTotal = (tend-tm);

    Log(1,"Terrain  (%d) created in %f ms",mesh->GetSurfaceCount(),timeTotal);
 

    return mesh;
}


//***********************************************************************************************************
//***********************************************************************************************************

MeshImporter::MeshImporter(Driver *driver)
{
    m_driver = driver;
}

MeshImporter::~MeshImporter()
{
  
}



void MeshImporterOBJ::Clear()
{
   
    m_materials.clear();
    m_mtlMap.clear();
    temp_vertices.clear();
    temp_uvs.clear();
    temp_normals.clear();
    vertices.clear();
    indices.clear();
 
}



Mesh* MeshImporterOBJ::Load(const VertexFormat& vertexFormat,const std::string& fileName)
{

    std::string meshPath = MeshBuilder::Instance().GetLoadPath() +   ExtractFilename(fileName);
    std::string filenameS(meshPath);
    std::string dir = filenameS.substr(0, filenameS.find_last_of("\\/"));

    

        FileStream file;
        if (!file.Open(meshPath.c_str(), "rb"))
        {
            Log(2, "Cannot open %s", meshPath.c_str());
            return nullptr;
        }

        u32 size = file.Size();
        char* buffer = new char[size];
        file.Read(buffer, size);
        file.Close();

        std::istringstream in(buffer);

        delete [] buffer;


      bool firstMesh = true;
      std::string currentMtl;
      std::string currentObj;

      std::string line;

        Mesh *mesh = new Mesh(m_driver,vertexFormat);
        Surface *surface = nullptr;

        bool gerenateUVS = false;
        bool gerenateNormals = false;


      while (getline(in, line))
      {
        if (line.substr(0,2) == "v ")
        {
          std::istringstream s(line.substr(2));
          glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;

          v.y = -v.y;
   
          temp_vertices.push_back(v);
        }
        else if (line.substr(0,3) == "vt ")
        {
          std::istringstream s(line.substr(3));
          glm::vec2 v; s >> v.x; s >> v.y;
          temp_uvs.push_back(v);
        }
        else if (line.substr(0,3) == "vn ")
        {
          std::istringstream s(line.substr(3));
          glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
          temp_normals.push_back(v);
        }
        else if (line.substr(0,2) == "f ")
        {
          unsigned int v1, v2, v3, v4;
          unsigned int vt1, vt2, vt3, vt4;
          unsigned int vn1, vn2, vn3, vn4;

          bool hasUV = true;
          bool hasNormal = true;
          bool done = false;

          // v1/v2/v3/v4
          int matches = sscanf(line.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3, &v4, &vt4, &vn4);
          if (matches == 12)
          {
            done = true;
            gerenateNormals = false;
            gerenateUVS = false;
            pushVertex(v1, vt1, vn1, hasUV,hasNormal);
            pushVertex(v2, vt2, vn2, hasUV,hasNormal);
            pushVertex(v3, vt3, vn3, hasUV,hasNormal);
            pushVertex(v1, vt1, vn1, hasUV,hasNormal);
            pushVertex(v3, vt3, vn3, hasUV,hasNormal);
            pushVertex(v4, vt4, vn4, hasUV,hasNormal);
          }

          // v1/v2/v3
          matches = sscanf(line.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
          if (matches == 9 && !done)
          {
            done = true;
            gerenateNormals = false;
            gerenateUVS = false;
            pushVertex(v1, vt1, vn1, hasUV,hasNormal);
            pushVertex(v2, vt2, vn2, hasUV,hasNormal);
            pushVertex(v3, vt3, vn3, hasUV,hasNormal);
          }

          // v1//vn1
          matches = sscanf(line.c_str(), "f %u//%u %u//%u %u//%u", &v1, &vn1, &v2, &vn2, &v3, &vn3);
          if (matches == 6 && !done)
          {
            done = true;
            hasUV = false;
            gerenateNormals = false;
            gerenateUVS = true;
            pushVertex(v1, vt1, vn1, hasUV,hasNormal);
            pushVertex(v2, vt2, vn2, hasUV,hasNormal);
            pushVertex(v3, vt3, vn3, hasUV,hasNormal);
          }

          //v1//
            matches = sscanf(line.c_str(), "f %u// %u// %u//", &v1, &v2, &v3);
            if (matches == 3 && !done)
            {
              done = true;
              gerenateNormals = true;
              gerenateUVS = true;
              pushVertex(v1, vt1, vn1, hasUV,hasNormal);
              pushVertex(v2, vt2, vn2, hasUV,hasNormal);
              pushVertex(v3, vt3, vn3, hasUV,hasNormal);
            }    

          if (!done)
          {
            Log(2, "Unsupported file!");
            return nullptr;
          }
        }
        else if (line.substr(0,7) == "mtllib ")
        {
                std::istringstream s(line.substr(7));
                std::string mtlPath; 
                s >> mtlPath;
                std::string fullPath = dir + "/" + mtlPath;
                LoadMaterial(fullPath);
                
            for (u32 i = 0; i < m_materials.size(); i++) 
            {
             
                auto m = m_materials[i];

                  if (m.texPath != "")
                {
                    std::string texPath = MeshBuilder::Instance().GetTexturePath()  + "/" +  ExtractFilename(m.texPath);
                    std::string fileName = ExtractFilenameNoExtension(texPath);
               //     Log(0, "Loading texture %s name %s ", texPath.c_str(),fileName.c_str());

                   Texture2D* texture =  Assets::Instance().LoadTexture(fileName, texPath);
                    if (texture)
                    {
                     
						mesh->AddMaterial(new TextureMaterial(texture));
					}
                    else
                    {
                        mesh->AddMaterial(new TextureMaterial( Assets::Instance().GetDefaultTexture()));
						Log(2, "Cannot load texture %s", texPath.c_str());
					}   
                }
            }

        }
        else if (line.substr(0,7) == "usemtl ")
        {
          if (!firstMesh) 
          {
              int matIndex = GetIndex(currentMtl) ;


         //   Log(0, "Creating mesh %s material %d %s", currentObj.c_str(), matIndex, currentMtl.c_str());

            
                surface = mesh->AddSurface(matIndex);
                surface->SetName(currentObj);

      
                for (u32 i = 0; i < indices.size() / 3; i++)
                {
                    surface->AddFace(indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2]);
                }

                for (u32 i = 0; i < vertices.size(); ++i)
                {
                    surface->AddVertex(vertices[i].position, vertices[i].texCoords, vertices[i].normal);
                }
                if (gerenateUVS)
                {
                
                    surface->TexturePlanarMapping(0.1f);
                }
                if (gerenateNormals)
                {
                    
                    surface->CalculateSmothNormals();
                }
            vertices.clear();
            indices.clear();
          }

          firstMesh = false;

          std::istringstream s(line.substr(7));
          s >> currentMtl;
        }
        else if (line.substr(0,2) == "o " || line.substr(0,2) == "g ")
        {
          //currentMtl = "";
          std::istringstream s(line.substr(2));
          s >> currentObj;
        }
        else if (line[0] == '#')
        {
        }
        else
        {
        }
      }

 

    surface = mesh->AddSurface(0);
    surface->SetName(currentObj);
    for (u32 i = 0; i < indices.size() / 3; i++)
    {
	  surface->AddFace(indices[i*3], indices[i*3+1], indices[i*3+2]);
	}   

    for (u32 i = 0; i < vertices.size(); ++i)
    {
	  surface->AddVertex(vertices[i].position,vertices[i].texCoords,vertices[i].normal);
	}   

    if (gerenateUVS)
    {

        surface->TexturePlanarMapping(0.1f);
    }
    if (gerenateNormals)
    {

        surface->CalculateSmothNormals();
    }

    mesh->OrderByMaterial();
    Clear();
    return mesh;



}

int MeshImporterOBJ::GetIndex(const std::string& name)
{
    for (u32 i = 0; i < m_materials.size(); i++)
    {
	    if (m_materials[i].name == name)
	      return i;
    }
   Log(2, "Material %s not found", name.c_str());

    return 0;


}

bool MeshImporterOBJ::LoadMaterial(const std::string& fileName)
{


        FileStream file;
        if (!file.Open(fileName.c_str(), "rb"))
        {
            Log(2, "Cannot open %s", fileName.c_str());
            return false;
        }

        u32 size = file.Size();
        char* buffer = new char[size];
        file.Read(buffer, size);
        file.Close();

        std::istringstream in(buffer);

        delete [] buffer;



      std::string line;
      MeshMaterial currentMtl = MeshMaterial();
      bool first = true;
      while (getline(in, line))
      {
        if (line.substr(0,7) == "newmtl ")
        {
          if (!first) 
          {
            m_mtlMap[currentMtl.name] = currentMtl;
            m_materials.push_back(currentMtl);
          //  Log(0, "Create Material %s total:%d", currentMtl.name.c_str(), (int)m_materials.size()-1);
            currentMtl = MeshMaterial();
        

           }

          first = false;

          std::istringstream s(line.substr(7));
          s >> currentMtl.name;
        }
        else if (line.substr(0, 7) == "map_Kd ")
        {
          std::istringstream s(line.substr(7));
          s >> currentMtl.texPath;
         // Log(0, "Texture path: %s", currentMtl.texPath.c_str());
        }
        else if (line.substr(0, 7) == "map_Ks ")
        {
          std::istringstream s(line.substr(7));
          s >> currentMtl.specularPath;
       //   Log(0, "Specular path: %s", currentMtl.specularPath.c_str());
        }
        else if (line.substr(0, 9) == "map_bump ")
        {
          std::istringstream s(line.substr(9));
          s >> currentMtl.normalPath;
         // Log(0, "Normal path: %s", currentMtl.normalPath.c_str());
        }
        else if (line.substr(0, 6) == "map_d ")
        {
          std::istringstream s(line.substr(6));
          s >> currentMtl.maskPath;
        }
        else if (line.substr(0, 3) == "Ka ")
        {
          float r, g, b;
          int matches = sscanf(line.c_str(), "Ka %f/%f/%f", &r, &g, &b);
          if (matches == 3)
            currentMtl.ambient = glm::vec3(r, g, b);
        }
        else if (line.substr(0, 3) == "Kd ")
        {
          float r, g, b;
          int matches = sscanf(line.c_str(), "Kd %f/%f/%f", &r, &g, &b);
          if (matches == 3)
            currentMtl.diffuse = glm::vec3(r, g, b);
        }
        else if (line.substr(0, 3) == "Ks ")
        {
          float r, g, b;
          int matches = sscanf(line.c_str(), "Ks %f/%f/%f", &r, &g, &b);
          if (matches == 3)
            currentMtl.specular = glm::vec3(r, g, b);
        }
        else if (line[0] == '#')
        {
        }
      }
   //    Log(0, "Create Material %s total:%d", currentMtl.name.c_str(),(int)m_materials.size());
       m_mtlMap[currentMtl.name] = currentMtl;
       m_materials.push_back(currentMtl);
     
       return true;
    }
  

 void MeshImporterOBJ::pushVertex(unsigned int v, unsigned int vt, unsigned int vn, bool hasUV, bool hasNormal)
    {
      v--;
      vt--;
      vn--;

      Vertex vertex; 
      vertex.position = temp_vertices[v];
      vertex.texCoords = hasUV ? temp_uvs[vt] : glm::vec2(0.0f);
      vertex.normal = hasNormal ? temp_normals[vn] : glm::vec3(0.0f);
     vertex.position.y = 1 - vertex.position.y;

      std::string k = std::to_string(v) + "/" + std::to_string(vt) + "/" + std::to_string(vn); 

      if (verticesMap.find(k) == 0)
      {
        vertices.push_back(vertex);
        verticesMap[k] = vertices.size()-1; 
      }

       indices.push_back(verticesMap[k]);
    }


    Mesh*  MeshStaticH3D::Load(const VertexFormat& vertexFormat,const std::string& fileName) 
    {

            

    std::string meshPath = MeshBuilder::Instance().GetLoadPath() +   ExtractFilename(fileName);





    //    Log(0,"Loading H3D file %s", meshPath.c_str());
      //  SDL_Log("Loading H3D file %s", fileName.c_str());
        FileStream stream(meshPath, "rb");
        if (!stream.IsOpen())
        {
            return nullptr;
        }

        Mesh *mesh = new Mesh(m_driver,vertexFormat);

       char a = stream.ReadByte(); // H
       char b = stream.ReadByte(); // 3
       char c = stream.ReadByte(); // D

        if (a != 'H' || b != '3' || c != 'D')
        {
            Log(2, "Invalid H3D file");
            return nullptr;
        } else 
        {
              int version =0;
        version=stream.ReadShort(); // version
        version+=stream.ReadShort(); // 
        version+=stream.ReadShort(); // 
        Log(0, "H3D file version %d (%c%c%c) ", version,a,b,c);

        }

      
        int numMaterial = stream.ReadInt(); // num materials

     //   Log(0, "Num materials %d", numMaterial);
            // std::string name = stream.ReadUTFString();
            // std::string base = stream.ReadUTFString();
            // int numTextures = stream.ReadInt();
            // Log(0, "Material %s base %s numTextures %d", name.c_str(), base.c_str(), numTextures);
        for (int i = 0; i < numMaterial; i++)
        {
      
            std::string name=stream.ReadUTFString();
            std::string base=stream.ReadUTFString();
            int numTextures = stream.ReadInt();

           Log(0, "Material %s base %s numTextures %d", name.c_str(), base.c_str(), numTextures);
     
            for (int j = 0; j < numTextures; j++)
            {
                //std::string tex=
                stream.ReadUTFString();
             //  Log(0, "Texture %s", texture.c_str());
            } 

                    if (base.size() < 3 && numTextures==0)   
                    {
                         mesh->AddMaterial(new TextureMaterial( Assets::Instance().GetDefaultTexture()));
                         continue; 
                    }



                    std::string texPath = MeshBuilder::Instance().GetTexturePath() + ExtractFilename(base);
                    std::string fileName = ExtractFilenameNoExtension(texPath);

  Log(0, "Material %s texture %s final %s", fileName.c_str(), base.c_str(), texPath.c_str());

                   //     Log(0, "Material %s base %s file %s  name %s", name.c_str(), base.c_str(), texPath.c_str(),  ExtractFilename(base).c_str());

             //     Log(0, " file %s  - name %s  -  %s",texPath.c_str(),  ExtractFilename(base).c_str(),MeshBuilder::Instance().GetTexturePath().c_str());

                    if (FileExists(texPath))
                    {
                           
                            Texture2D* texture =  Assets::Instance().LoadTexture(fileName, texPath);
                            if (texture)
                            {
                                
                                mesh->AddMaterial(new TextureMaterial(texture));
                            }
                            else
                            {
                                Log(2, "Cannot load texture %s", texPath.c_str());
                                mesh->AddMaterial(new TextureMaterial( Assets::Instance().GetDefaultTexture()));
                            }   

                    } else 
                    {
                        Log(2, "Texture %s dont exits ", texPath.c_str());
                        mesh->AddMaterial(new TextureMaterial( Assets::Instance().GetDefaultTexture()));
                    }

                   

                    
        }

        u32 numSurfaces = stream.ReadInt(); // num surfaces
      //  Log(0, "Num surfaces %d", numSurfaces);
        for (u32 i=0; i<numSurfaces; i++)
        {
            std::string name=stream.ReadUTFString();
            int material = stream.ReadInt();
            int numVertices = stream.ReadInt();
            int numIndices = stream.ReadInt();
            int hasNormals = stream.ReadShort();
            int hasTexcoords = stream.ReadShort();
         //   Log(0, "Surface %s material %d numVertices %d numIndices %d hasNormals %d hasTexcoords %d", name.c_str(), material, numVertices, numIndices, hasNormals, hasTexcoords);
            Surface *surface = mesh->AddSurface(material);
            surface->SetName(name);
    
            for (int j = 0; j < numVertices; j++)
            {
                float x = stream.ReadFloat();
                float y = stream.ReadFloat();
                float z = stream.ReadFloat();
                float nx =0;
                float ny =0;
                float nz =0;
                float u=0;
                float v=0;

                if (hasTexcoords)
                {
                    u = stream.ReadFloat();
                    v = stream.ReadFloat();
                }

                if (hasNormals)
                {
                    nx = stream.ReadFloat();
                    ny = stream.ReadFloat();
                    nz = stream.ReadFloat();
   
                }
          
               
                surface->AddVertex(x, y, z, u, v, nx, ny, nz);
            }
            for (int j = 0; j < numIndices; j++)
            {
                int a = stream.ReadInt();
                int b = stream.ReadInt();
                int c = stream.ReadInt();
                surface->AddFace(a, b, c);
            }
            if (!hasNormals)
            {
                surface->CalculateSmothNormals();
            }
            if (!hasTexcoords)
            {
                surface->TexturePlanarMapping(0.1f);
            }
            surface->Upload();
        }
        mesh->OrderByMaterial();
        Clear();
        return mesh;

    }
