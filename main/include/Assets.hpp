#pragma once


#include <string>
#include "Config.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Batch.hpp"




class  Assets : public IDrop
{
    
      
    public:
        ~Assets();
       

        Shader*    LoadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath);
        Shader*    LoadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath);
        Shader*    CreateShader(const std::string &name, const char *vertexCode, const char *fragmentCode);
        Shader*    CreateShader(const std::string &name, const char *vertexCode, const char *fragmentCode, const char *geometryCode);

        Texture2D *LoadTexture(const std::string &name, const std::string &path);
        Texture2D *LoadTexture(const std::string &name, const Pixmap &pixmap);
        Texture2D *LoadTexture(const std::string &name, const u8 *buffer,u16 components, int width, int height);


        Texture2D *CreateTexture(const Pixmap &pixmap);
        Texture2D *CreateTexture(const std::string &file_name);
        Texture2D *CreateTexture(const u8 *buffer,u16 components, int width, int height);


        void RemoveShader(const std::string &name);
        void RemoveTexture(const std::string &name);

        void AddShader(const std::string &name, Shader *shader);
        void AddTexture(const std::string &name, Texture2D *texture);

        bool ShaderExists(const std::string &name);
        bool TextureExists(const std::string &name);

        Shader* GetShader(const std::string &name);
        Texture2D* GetTexture(const std::string &name);

        Font* GetFont(const std::string &name);
        void AddFont(const std::string &name, Font *font);
        void RemoveFont(const std::string &name);
        bool FontExists(const std::string &name);
        Font *LoadFont(const std::string &name, const std::string &path);


         void Clear();



         Texture2D *GetDefaultTexture() { return m_defaultTexture; }
         Font *GetDefaultFont() { return m_fonts["default"]; }


        static Assets& Instance();
 

    private:
             Map<std::string, Shader*> m_shaders;
             Map<std::string, Texture2D*> m_textures;
             Map<std::string, Font*> m_fonts;
             Texture2D *m_defaultTexture;

              static Assets* instance;
              Assets(const Assets&) = delete;

    private:
        friend class Device;
        friend class Driver;
     
        Assets();

        void LoadDefaultAssets();
        Font *BuildDefaultFont();
        

};

