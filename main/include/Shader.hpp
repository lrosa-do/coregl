#pragma once


#include <string>

#include "Config.hpp"
#include <SDL2/SDL.h>

#include "glad/glad.h"
#include "Map.hpp"
#include "Math.hpp"


class  Shader 
{
    public:
           Shader();
            virtual ~Shader();
        

        bool Create(const char* vShaderCode, const char* fShaderCode);
        bool Create(const char* vShaderCode, const char* fShaderCode, const char* gShaderCode);
        bool Load(const char* vShaderCode, const char* fShaderCode);
        bool Load(const char* vShaderCode, const char* fShaderCode, const char* gShaderCode);
        bool operator ==(const Shader&      other) const { return m_program == other.m_program; }
        bool operator !=(const Shader&      other) const { return m_program != other.m_program; }


   

        u32 GetID()    {        return m_program;    }

        void Use() const;



   

        void SetInt(const std::string &name, int value) ;
        void SetFloat(const std::string &name, float value) ;
        void SetMatrix4(const std::string &name, const float *value) ;

        void SetFloat2(const std::string& name, float x, float y);
        void SetFloat3(const std::string& name, float x, float y, float z);
        void SetFloat4(const std::string& name, float x, float y, float z,float w);


        void SetFloat2(const std::string& name, const glm::vec2& vector);
        void SetFloat3(const std::string& name, const glm::vec3& vector);
        void SetFloat4(const std::string& name, const glm::vec4& vector);

        void SetMatrix4(const std::string& name, const glm::mat4& matrix);
        void SetMatrix3(const std::string& name, const glm::mat3& matrix);

        void Release();

        


    void print();

    bool findUniform(const std::string &name)const;

    int  getUniform(const std::string &name);
    int  getAttribute(const std::string &name);


    int  getUniformLocation(const std::string &uniformName) const;
    int  getAttribLocation( const std::string &attribName) const;
    
    bool addUniform(const std::string &name);
    bool addAttribute(const std::string &name);

    bool ContainsUniform(const std::string &name) const;

    void LoadDefaults();

private:
        u32 m_program;
         Map<std::string, int> m_uniforms;
         Map<std::string, int> m_attributes;
        int m_numAttributes;
        int m_numUniforms;


    private:
         void checkCompileErrors(unsigned int shader, const std::string &type);
 
           Shader& operator =(const Shader& other) = delete;
        Shader(const Shader& other) = delete;
          
};