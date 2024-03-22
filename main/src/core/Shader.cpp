#include "Shader.hpp"
#include "Utils.hpp"


Shader::Shader()
{
    m_program=0;
    m_numAttributes=0;
    m_numUniforms=0;

}

Shader::~Shader()
{
    Release();
}

void Shader::Use() const
{
    glUseProgram(m_program);
}

void Shader::Release()
{
    if (m_program>0)
    {
        Log(0, "SHADER: [ID %i] Release shader program.", m_program);
        glDeleteProgram(m_program);
    }
    m_program=0;
}

bool Shader::Create(const char* vShaderCode, const char* fShaderCode, const char* gShaderCode)
{
    // 2. compile shaders
    unsigned int vertex, fragment, geometry;
    
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    
    // geometry shader
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");
    
    // shader Program
    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glAttachShader(m_program, geometry);
    glLinkProgram(m_program);
    checkCompileErrors(m_program, "PROGRAM");
    
    if (m_program>0)
        Log(0, "SHADER: [ID %i] Create shader program.", m_program);
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
    glUseProgram(m_program);
    
    return true;


}


bool Shader::Create(const char *vShaderCode, const char *fShaderCode)
{
     // 2. compile shaders
    unsigned int vertex, fragment;
    
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    
    // shader Program
    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);
    checkCompileErrors(m_program, "PROGRAM");

    if (m_program>0)
        Log(0, "SHADER: [ID %i] Create shader program.", m_program);
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glUseProgram(m_program); 
    
    return true;
}

bool Shader::Load(const char *vShaderCode, const char *fShaderCode)
{
    bool result=false;
    char *shaderCode = LoadFileText(vShaderCode);
    char *fragCode = LoadFileText(fShaderCode);
    if (shaderCode && fragCode)
    {
        result = Create(shaderCode, fragCode);
        free(shaderCode);
        free(fragCode);
    }
    
    return result;
}

bool Shader::Load(const char *vShaderCode, const char *fShaderCode, const char *gShaderCode)
{
    bool result=false;
    char *shaderCode = LoadFileText(vShaderCode);
    char *fragCode = LoadFileText(fShaderCode);
    char *geomCode = LoadFileText(gShaderCode);
    if (shaderCode && fragCode && geomCode)
    {
        result = Create(shaderCode, fragCode, geomCode);
        free(shaderCode);
        free(fragCode);
        free(geomCode);
    }
    
    return result;
}

void Shader::checkCompileErrors(unsigned int shader, const std::string &type)
{
      int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            Log(2,"SHADER: Error Compile %s Msg: %s", type.c_str(), infoLog );
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            Log(2,"SHADER: Error Link %s Msg: %s", type.c_str(), infoLog );
        }
    }
}


bool Shader::findUniform(const std::string &name)const
{
    return m_uniforms.find(name) ;
}
int  Shader::getUniform(const std::string &name)
{
    return m_uniforms[name];
}
int Shader::getAttribute(const std::string &name) 
{
    return m_attributes[name];
}
int Shader::getUniformLocation(const std::string &uniformName) const
{
    int location =  glGetUniformLocation(m_program, uniformName.c_str());
      if (location == -1)
         Log(2, "SHADER: [ID %i] Failed to find shader uniform: %s", m_program, uniformName.c_str());
    return location;
}
int Shader::getAttribLocation( const std::string &attribName) const
{
    int location  = glGetAttribLocation(m_program, attribName.c_str());
    if (location == -1)
        Log(2, "SHADER: [ID %i] Failed to find shader attribute: %s", m_program, attribName.c_str());
    return location;
}
bool Shader::addUniform(const std::string &name)
{
    int location = -1;
    location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1)
    {
        Log(2, "SHADER: [ID %i] Failed to find shader uniform: %s", m_program, name.c_str());
        return false;
    }
    

    // Log(0, "SHADER: [ID %i] shader uniform (%s) set at location: %i", m_program, name.c_str(), location);
    m_uniforms[name] = location;
    return true;
}
bool Shader::addAttribute(const std::string &name)
{
    int location =  glGetAttribLocation(m_program, name.c_str());
    if (location == -1)
    {
        Log(2, "SHADER: [ID %i] Failed to find shader attribute: %s", m_program, name.c_str());
        return false;
    }
    m_attributes[name]=location;
    return true;
}



 bool Shader::ContainsUniform(const std::string &name) const
{
    return m_uniforms.find(name) != 0;

}
void Shader::SetInt(const std::string &name, int value) 
{
    if (ContainsUniform(name) )
        glUniform1i(getUniform(name), value);
}
void Shader::SetFloat(const std::string &name, float value)
{
    if (ContainsUniform(name) )
        glUniform1f(getUniform(name), value);
}
void Shader::SetMatrix4(const std::string &name, const float *value)
{
     if (ContainsUniform(name) )
        glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, value);
}

void Shader::SetFloat2(const std::string& name, const glm::vec2& vector)
{
    if (ContainsUniform(name) )
        glUniform2f(getUniform(name),  vector.x, vector.y);
}
void Shader::SetFloat2(const std::string& name, float x, float y)
{
    if (ContainsUniform(name) )
        glUniform2f(getUniform(name),  x,y);
}
void Shader::SetFloat3(const std::string& name,float x, float y, float z)
{
    if (ContainsUniform(name))
        glUniform3f(getUniform(name), x, y, z);
}
void Shader::SetFloat4(const std::string& name, float x, float y, float z, float w)
{
	if (ContainsUniform(name) )
		glUniform4f(getUniform(name),  x, y, z, w);
}   
void Shader::SetFloat3(const std::string& name, const glm::vec3& vector)
{
    if (ContainsUniform(name) )
        glUniform3f(getUniform(name),  vector.x, vector.y, vector.z);
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& vector)
{
    if (ContainsUniform(name) )
        glUniform4f(getUniform(name),  vector.x, vector.y, vector.z, vector.w);
}

void Shader::SetMatrix4(const std::string& name, const glm::mat4& matrix)
{
     glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, glm::value_ptr(matrix));

   //   Log(2, "%d  : %s  id %d", getUniform(name), name.c_str(), m_program);

    // if (ContainsUniform(name) )
    // {
    //     // Log(2, "SHADER: [ID %i] Failed to find shader uniform: %s", getUniform(name), name.c_str());

    //  //   glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, glm::value_ptr(matrix));
    // } else 
    // {
    //     Log(2, "SHADER: [ID %i] Failed to find shader uniform: %s", m_program, name.c_str());
    // }
}

void Shader::SetMatrix3(const std::string& name, const glm::mat3& matrix)
{
    if (ContainsUniform(name) )
        glUniformMatrix3fv(getUniform(name), 1, GL_FALSE, glm::value_ptr(matrix));
}


void Shader::print()
{

Log(0,"[SHADER]  Id(%d) Num Attributes(%d)  Num Uniforms (%d)",m_program, m_numAttributes,m_numUniforms);
{
    Map<std::string,int>::ConstIterator it = m_attributes.getConstIterator();
    while (!it.atEnd()) 
    {
        const auto& node = *it;
        Log(0,"[SHADER]  Id(%d)  attribute  index(%d) name(%s)",m_program, node.getValue(),node.getKey().c_str());
        it++;
    }
}
{
    Map<std::string,int>::ConstIterator it = m_uniforms.getConstIterator();

    while (!it.atEnd()) 
    {
        const auto& node = *it;
        Log(0,"[SHADER]  Id(%d) uniform index(%d) name(%s)",m_program, node.getValue(),node.getKey().c_str());
        it++;
    }
}
}
void Shader::LoadDefaults()
{
    GLint numActiveAttribs = 0;
    GLint maxAttribNameLength = 0;
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
    m_numAttributes=numActiveAttribs;
    for(GLint attrib = 0; attrib < numActiveAttribs; ++attrib)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        GLenum type = GL_ZERO;
        glGetActiveAttrib(m_program, attrib,  sizeof(name) - 1, &namelen, &num, &type, name);
        name[namelen] = 0;
        addAttribute(std::string((char*)&name[0]));
        glBindAttribLocation(m_program, attrib, (char*)&name[0]);
        Log(0,"SHADER: [ID %i] Active attribute (%s) set at location: %i", m_program, name,attrib);
    }
    // Get available shader uniforms
    int uniformCount = -1;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &uniformCount);
    m_numUniforms=uniformCount;

    for (int i = 0; i < uniformCount; i++)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        GLenum type = GL_ZERO;
        glGetActiveUniform(m_program, i, sizeof(name) - 1, &namelen, &num, &type, name);
        name[namelen] = 0;
        addUniform(std::string((char*)&name[0]));
        Log(0,"SHADER: [ID %i] Active uniform (%s) set at location: %i", m_program, name, glGetUniformLocation(m_program, name));
    }
}