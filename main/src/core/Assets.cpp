
#include "Assets.hpp"
#include "Utils.hpp"
#include "Array.hpp"


constexpr const int MAX_LIGHTS = 8;

Assets*  Assets::instance = nullptr;


Assets& Assets::Instance()
{
    if (instance == nullptr)
    {
        instance = new Assets();
    }
    return *instance;
}

Assets::Assets()
{
   
    m_defaultTexture = nullptr;
}

Assets::~Assets()
{
    Clear();
}

void Assets::Clear()
{
    Log(0, "ASSETS: Clearing ");
    while (!m_shaders.empty())
    {
    
        auto node = m_shaders.getRoot();
        std::string key = node->getKey();
        Shader * value = node->getValue();
        Log(0, "ASSETS: Removing shader %s", key.c_str());
        delete value; 
        m_shaders.remove(key);


    }

    m_shaders.clear();


    while (!m_textures.empty())
    {
        auto node = m_textures.getRoot();
        std::string key = node->getKey();
        Texture2D * value = node->getValue();
        Log(0, "ASSETS: Removing texture %s", key.c_str());
        delete value; 
        m_textures.remove(key);
    }
    m_textures.clear();

    while (!m_fonts.empty())
    {
        auto node = m_fonts.getRoot();
        std::string key = node->getKey();
        Font * value = node->getValue();
        Log(0, "ASSETS:Removing font %s", key.c_str());
        delete value; 
        m_fonts.remove(key);
    }


      m_fonts.clear();
}


Shader *LoadtexturedShader()
{
    const char* vShader = GLSL(
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec2 texCoord;


        out vec2 TexCoord;

        uniform mat4 mvp;


        void main()
        {
            gl_Position = mvp * vec4(position, 1.0);
            TexCoord = texCoord;

        }
    );

    const char* fShader = GLSL(
        in vec2 TexCoord;


        out vec4 FragColor;
        uniform sampler2D sample0;

        void main()
        {
          

            FragColor = texture(sample0, TexCoord) ;
        }
    );

    Shader* shader = new Shader();
    shader->Create(vShader, fShader);
    shader->LoadDefaults();
    return shader;


}

Shader* LoadLightShader()
{
  


            const char* vShader = GLSL(
            layout(location = 0) in vec3 position;
            layout(location = 1) in vec2 texCoord;
            layout(location = 2) in vec3 normal;



            out vec3 FragPos;
            out vec3 Normal;
            out vec2 TexCoord;      
 

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

       
            void main()
            {
                FragPos = vec3(model * vec4(position, 1.0));
                Normal = mat3(transpose(inverse(model))) * normal;
                gl_Position = projection * view * vec4(FragPos, 1.0);
                TexCoord = texCoord;
               
            }
            );


            const char* fShader = GLSL(
            
                
            out vec4 FragColor;
  


            struct Material
            {
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                float shininess;
            };

            struct PointLight 
            {
                vec3 position;

                float constant;
                float linear;
                float quadratic;

                vec3 ambient;
                vec3 diffuse;
                vec3 specular;

                bool enabled;
            };

            struct DirectionalLight
            {
                vec3 direction;

                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                bool enabled;
            };
            struct SpotLight 
            {
                vec3 position;
                vec3 direction;
                float cutOff;
                float outerCutOff;

                float constant;
                float linear;
                float quadratic;

                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                bool enabled;
            };


            in vec3 FragPos;
            in vec3 Normal;
            in vec2 TexCoord;

            uniform vec3         viewPosition;
            uniform Material     material;
            
            uniform PointLight   pointLights[8];
            uniform DirectionalLight directionalLight;
            uniform SpotLight spotLight;
            uniform sampler2D    texture0;

            vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
            
            vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal,  vec3 viewDir);

            vec3  CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

            void main()
            {
                vec4 textureColor = texture(texture0, TexCoord) ;

                vec3 normal = normalize(Normal);
                vec3 viewDir = normalize(viewPosition - FragPos);

                vec3 result = CalcDirectionalLight(directionalLight, normal,  viewDir);
              
                 //    result += CalcPointLight(pointLights, normal, FragPos, viewDir);
           //   result += CalcSpotLight(spotLight, normal, FragPos, viewDir);

              
             //   FragColor = vec4(result, 1.0) * textureColor;
                FragColor =  textureColor;

            }

            vec3  CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
            {

                if (!light.enabled)
                {
                    return vec3(0.0, 0.0, 0.0);
                }

                vec3 lightDir = normalize(light.position - fragPos);
				// diffuse shading
				float diff = max(dot(normal, lightDir), 0.0);
				// specular shading
				vec3 reflectDir = reflect(-lightDir, normal);
				float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
				// attenuation
				float distance = length(light.position - fragPos);
				float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
				// spotlight intensity
				float theta = dot(lightDir, normalize(-light.direction));
				float epsilon = light.cutOff - light.outerCutOff;
				float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
				// combine results
				vec3 ambient = light.ambient * material.ambient;
				vec3 diffuse = light.diffuse * (diff * material.diffuse);
				vec3 specular = light.specular * (spec * material.specular);
				ambient *= attenuation;
				diffuse *= attenuation * intensity;
				specular *= attenuation * intensity;
				return (ambient + diffuse + specular);
            }

            vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
            {
                vec3 diffuse = vec3(0.0, 0.0, 0.0);
                vec3 ambient = vec3(0.0, 0.0, 0.0);
                vec3 specular = vec3(0.0, 0.0, 0.0);

                if (!light.enabled)
                {
                    return vec3(0.0, 0.0, 0.0);
                }
                ambient = light.ambient * material.ambient;

                vec3 lightDir = normalize(-light.direction);
				// diffuse shading
				float diff = max(dot(normal, lightDir), 0.0);
                if (diff > 0.0)
                {
                    
                    diffuse = light.diffuse * (diff * material.diffuse);

                    // specular shading
					vec3 reflectDir = reflect(-lightDir, normal);
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                    if (spec > 0.0)
                    {
						
						specular = light.specular * (spec * material.specular);
					}
					/*ambient = light.ambient * material.ambient;
					diffuse = light.diffuse * (diff * material.diffuse);
					specular = light.specular * (spec * material.specular);*/
				}

				//// specular shading
				//vec3 reflectDir = reflect(-lightDir, normal);
				//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
				//// combine results
				//ambient = light.ambient * material.ambient;
				//diffuse = light.diffuse * (diff * material.diffuse);
				//specular = light.specular * (spec * material.specular);
				return (ambient + diffuse + specular);  

            }

            vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
            {
                if (!light.enabled)
                {
                    return vec3(0.0, 0.0, 0.0);
                }
                
             
                vec3 lightDir = normalize(light.position - fragPos);
                // diffuse shading
                float diff = max(dot(normal, lightDir), 0.0);
                // specular shading
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                // attenuation
                float distance = length(light.position - fragPos);
                float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

                // ambient
                vec3 ambient = light.ambient * material.ambient;

                // diffuse        
                vec3 diffuse = light.diffuse * (diff * material.diffuse);

                // specular              
                vec3 specular = light.specular * (spec * material.specular);

                ambient *= attenuation;
                diffuse *= attenuation;
                specular *= attenuation;
                return (ambient + diffuse + specular);
            }


            );


            Shader* shader = new Shader();
            shader->Create(vShader, fShader);
            shader->LoadDefaults();
            return shader;

}

Shader* LoadDefaultShader()
{

    
        const char* vShader = GLSL(
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec2 texCoord;
        layout(location = 2) in vec4 color;



        uniform mat4 mvp;

        out vec2 TexCoord;
        out vec4 vertexColor;
        void main()
        {
            gl_Position = mvp * vec4(position, 1.0);
            TexCoord = texCoord;
            vertexColor = color;
        }
        );


        const char* fShader = GLSL(
            in vec2 TexCoord;
        out vec4 color;
        in vec4 vertexColor;
        uniform sampler2D texture0;
        void main()
        {
            color = texture(texture0, TexCoord) * vertexColor;
            //    if (texelColor.a <= 0.1) discard; 
    //      if ( (texelColor.r==0.0) &&  (texelColor.g==0.0) && (texelColor.r==0.0)) discard;
        }
        );




        Log(0, "ASSETS: Loading default assets");
        Shader* defaultShader = new Shader();
        defaultShader->Create(vShader, fShader);
        defaultShader->LoadDefaults();
        return defaultShader;

}



void Assets::LoadDefaultAssets()
{

    m_shaders["default"] = LoadDefaultShader();
    m_shaders["light"]   = LoadLightShader();
    m_shaders["textured"] = LoadtexturedShader();



   
    unsigned char pixels[4] = {         255, 255, 255, 255      }; 
    Pixmap image(1,1,4,pixels);
    m_defaultTexture =new  Texture2D(image);
    m_textures["default"] =  m_defaultTexture;;
    BuildDefaultFont();

}



Shader *Assets::LoadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath)
{
    Shader *shader = new Shader();
    shader->Load(vertexPath.c_str(), fragmentPath.c_str());
    m_shaders[name] = shader;
    return shader;
}

Shader *Assets::LoadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath)
{
    Shader *shader = new Shader();
    shader->Load(vertexPath.c_str(), fragmentPath.c_str(), geometryPath.c_str());
    m_shaders[name] = shader;
    return shader;
}

Shader *Assets::CreateShader(const std::string &name, const char *vertexCode, const char *fragmentCode)
{
    Shader *shader = new Shader();
    shader->Create(vertexCode, fragmentCode);
    m_shaders[name] = shader;
    return shader;
}

Shader *Assets::CreateShader(const std::string &name, const char *vertexCode, const char *fragmentCode, const char *geometryCode)
{
    Shader *shader = new Shader();
    shader->Create(vertexCode, fragmentCode, geometryCode);
    m_shaders[name] = shader;
    return shader;
}

Texture2D *Assets::LoadTexture(const std::string &name, const std::string &path)
{
    if (TextureExists(name))
    {
        return m_textures[name];
    }
    Texture2D *texture = new Texture2D();



    if (!texture->Load(path))
    {
        delete texture;
        return m_defaultTexture;
    }
    m_textures[name] = texture;
    return texture;
}

Texture2D *Assets::LoadTexture(const std::string &name, const Pixmap &pixmap)
{
    if (TextureExists(name))
    {
        return m_textures[name];
    }
    Texture2D *texture = new Texture2D(pixmap);
    m_textures[name] = texture;
    return texture;
}

Texture2D *Assets::LoadTexture(const std::string &name, const u8 *buffer, u16 components, int width, int height)
{
    if (TextureExists(name))
    {
        return m_textures[name];
    }
    Texture2D *texture = new Texture2D();
    if (!texture->LoadFromMemory(buffer, components, width, height))
    {
        delete texture;
        return m_defaultTexture;
    }
    m_textures[name] = texture;
    return texture;
}

Texture2D *Assets::CreateTexture(const Pixmap &pixmap)
{
    Texture2D *texture = new Texture2D(pixmap);
    return texture;
}

Texture2D *Assets::CreateTexture(const std::string &file_name)
{
    Texture2D *texture = new Texture2D();
    texture->Load(file_name);
    return texture;
}

Texture2D *Assets::CreateTexture(const u8 *buffer,u16 components, int width, int height)
{
    Texture2D *texture = new Texture2D();
    texture->LoadFromMemory(buffer,components,width,height);
    return texture;
}

void Assets::RemoveShader(const std::string &name)
{
    auto node = m_shaders.find(name);
    if (node != 0)
    {
        delete node->getValue();
        m_shaders.remove(name);
    }
}


void Assets::RemoveTexture(const std::string &name)
{
    auto node = m_textures.find(name);
    if (node != 0)
    {
        delete node->getValue();
        m_textures.remove(name);
    }
}

void Assets::AddShader(const std::string &name, Shader *shader)
{
    m_shaders[name] = shader;
}

void Assets::AddTexture(const std::string &name, Texture2D *texture)
{
    m_textures[name] = texture;
}

bool Assets::ShaderExists(const std::string &name)
{
    return m_shaders.find(name) != 0;
}

bool Assets::TextureExists(const std::string &name)
{
    return m_textures.find(name) != 0;
}

Shader *Assets::GetShader(const std::string &name)
{
    if (m_shaders.find(name) == 0)
    {
        Log(0, "ASSETS: Shader %s not found", name.c_str());
        return m_shaders["default"];
    } 
    
    return m_shaders[name];
}

Texture2D *Assets::GetTexture(const std::string &name)
{

    if (m_textures.find(name) == 0)
    {
        Log(0, "ASSETS: Texture %s not found", name.c_str());
		return m_defaultTexture;
	}
    return m_textures[name];

}

Font *Assets::GetFont(const std::string &name)
{
    if (m_fonts.find(name) == 0)
    {
        Log(0, "Font %s not found", name.c_str());
        return nullptr;
    }
    return m_fonts[name];
}


void Assets::RemoveFont(const std::string &name)
{
    auto node = m_fonts.find(name);
    if (node != 0)
    {

        delete node->getValue();
        m_fonts.remove(name);
    }
}
bool Assets::FontExists(const std::string &name)
{
    return m_fonts.find(name) != 0;
}
Font *Assets::LoadFont(const std::string &name, const std::string &path)
{
    Font *font = new Font();

    if (!font->Load(path))
    {
        delete font;
        return nullptr;
    }
    m_fonts[name] = font;
    return font;

}

void Assets::AddFont(const std::string &name, Font *font)
{
    m_fonts[name] = font;
}


    unsigned int defaultFontData[512] = {
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200020, 0x0001b000, 0x00000000, 0x00000000, 0x8ef92520, 0x00020a00, 0x7dbe8000, 0x1f7df45f,
        0x4a2bf2a0, 0x0852091e, 0x41224000, 0x10041450, 0x2e292020, 0x08220812, 0x41222000, 0x10041450, 0x10f92020, 0x3efa084c, 0x7d22103c, 0x107df7de,
        0xe8a12020, 0x08220832, 0x05220800, 0x10450410, 0xa4a3f000, 0x08520832, 0x05220400, 0x10450410, 0xe2f92020, 0x0002085e, 0x7d3e0281, 0x107df41f,
        0x00200000, 0x8001b000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc0000fbe, 0xfbf7e00f, 0x5fbf7e7d, 0x0050bee8, 0x440808a2, 0x0a142fe8, 0x50810285, 0x0050a048,
        0x49e428a2, 0x0a142828, 0x40810284, 0x0048a048, 0x10020fbe, 0x09f7ebaf, 0xd89f3e84, 0x0047a04f, 0x09e48822, 0x0a142aa1, 0x50810284, 0x0048a048,
        0x04082822, 0x0a142fa0, 0x50810285, 0x0050a248, 0x00008fbe, 0xfbf42021, 0x5f817e7d, 0x07d09ce8, 0x00008000, 0x00000fe0, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000c0180,
        0xdfbf4282, 0x0bfbf7ef, 0x42850505, 0x004804bf, 0x50a142c6, 0x08401428, 0x42852505, 0x00a808a0, 0x50a146aa, 0x08401428, 0x42852505, 0x00081090,
        0x5fa14a92, 0x0843f7e8, 0x7e792505, 0x00082088, 0x40a15282, 0x08420128, 0x40852489, 0x00084084, 0x40a16282, 0x0842022a, 0x40852451, 0x00088082,
        0xc0bf4282, 0xf843f42f, 0x7e85fc21, 0x3e0900bf, 0x00000000, 0x00000004, 0x00000000, 0x000c0180, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000402, 0x41482000, 0x00000000, 0x00000800,
        0x04000404, 0x4100203c, 0x00000000, 0x00000800, 0xf7df7df0, 0x514bef85, 0xbefbefbe, 0x04513bef, 0x14414500, 0x494a2885, 0xa28a28aa, 0x04510820,
        0xf44145f0, 0x474a289d, 0xa28a28aa, 0x04510be0, 0x14414510, 0x494a2884, 0xa28a28aa, 0x02910a00, 0xf7df7df0, 0xd14a2f85, 0xbefbe8aa, 0x011f7be0,
        0x00000000, 0x00400804, 0x20080000, 0x00000000, 0x00000000, 0x00600f84, 0x20080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0xac000000, 0x00000f01, 0x00000000, 0x00000000, 0x24000000, 0x00000f01, 0x00000000, 0x06000000, 0x24000000, 0x00000f01, 0x00000000, 0x09108000,
        0x24fa28a2, 0x00000f01, 0x00000000, 0x013e0000, 0x2242252a, 0x00000f52, 0x00000000, 0x038a8000, 0x2422222a, 0x00000f29, 0x00000000, 0x010a8000,
        0x2412252a, 0x00000f01, 0x00000000, 0x010a8000, 0x24fbe8be, 0x00000f01, 0x00000000, 0x0ebe8000, 0xac020000, 0x00000f01, 0x00000000, 0x00048000,
        0x0003e000, 0x00000f00, 0x00000000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000038, 0x8443b80e, 0x00203a03,
        0x02bea080, 0xf0000020, 0xc452208a, 0x04202b02, 0xf8029122, 0x07f0003b, 0xe44b388e, 0x02203a02, 0x081e8a1c, 0x0411e92a, 0xf4420be0, 0x01248202,
        0xe8140414, 0x05d104ba, 0xe7c3b880, 0x00893a0a, 0x283c0e1c, 0x04500902, 0xc4400080, 0x00448002, 0xe8208422, 0x04500002, 0x80400000, 0x05200002,
        0x083e8e00, 0x04100002, 0x804003e0, 0x07000042, 0xf8008400, 0x07f00003, 0x80400000, 0x04000022, 0x00000000, 0x00000000, 0x80400000, 0x04000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00800702, 0x1848a0c2, 0x84010000, 0x02920921, 0x01042642, 0x00005121, 0x42023f7f, 0x00291002,
        0xefc01422, 0x7efdfbf7, 0xefdfa109, 0x03bbbbf7, 0x28440f12, 0x42850a14, 0x20408109, 0x01111010, 0x28440408, 0x42850a14, 0x2040817f, 0x01111010,
        0xefc78204, 0x7efdfbf7, 0xe7cf8109, 0x011111f3, 0x2850a932, 0x42850a14, 0x2040a109, 0x01111010, 0x2850b840, 0x42850a14, 0xefdfbf79, 0x03bbbbf7,
        0x001fa020, 0x00000000, 0x00001000, 0x00000000, 0x00002070, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x08022800, 0x00012283, 0x02430802, 0x01010001, 0x8404147c, 0x20000144, 0x80048404, 0x00823f08, 0xdfbf4284, 0x7e03f7ef, 0x142850a1, 0x0000210a,
        0x50a14684, 0x528a1428, 0x142850a1, 0x03efa17a, 0x50a14a9e, 0x52521428, 0x142850a1, 0x02081f4a, 0x50a15284, 0x4a221428, 0xf42850a1, 0x03efa14b,
        0x50a16284, 0x4a521428, 0x042850a1, 0x0228a17a, 0xdfbf427c, 0x7e8bf7ef, 0xf7efdfbf, 0x03efbd0b, 0x00000000, 0x04000000, 0x00000000, 0x00000008,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200508, 0x00840400, 0x11458122, 0x00014210,
        0x00514294, 0x51420800, 0x20a22a94, 0x0050a508, 0x00200000, 0x00000000, 0x00050000, 0x08000000, 0xfefbefbe, 0xfbefbefb, 0xfbeb9114, 0x00fbefbe,
        0x20820820, 0x8a28a20a, 0x8a289114, 0x3e8a28a2, 0xfefbefbe, 0xfbefbe0b, 0x8a289114, 0x008a28a2, 0x228a28a2, 0x08208208, 0x8a289114, 0x088a28a2,
        0xfefbefbe, 0xfbefbefb, 0xfa2f9114, 0x00fbefbe, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00210100, 0x00000004, 0x00000000, 0x00000000, 0x14508200, 0x00001402, 0x00000000, 0x00000000,
        0x00000010, 0x00000020, 0x00000000, 0x00000000, 0xa28a28be, 0x00002228, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000,
        0xa28a28aa, 0x000022a8, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000, 0xbefbefbe, 0x00003e2f, 0x00000000, 0x00000000,
        0x00000004, 0x00002028, 0x00000000, 0x00000000, 0x80000000, 0x00003e0f, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };


Font *Assets::BuildDefaultFont()
{

    Font *font = new Font();

   #define BIT_CHECK(a,b) ((a) & (1u << (b)))

    
    font->m_glyphCount = 224;   
    font->m_glyphPadding = 0;   

    

    int charsHeight = 10;
    int charsDivisor = 1;    // Every char is separated from the consecutive by a 1 pixel divisor, horizontally and vertically

 
    int charsWidth[224] = { 3, 1, 4, 6, 5, 7, 6, 2, 3, 3, 5, 5, 2, 4, 1, 7, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 3, 4, 3, 6,
                            7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 6, 5, 7, 6, 6, 6, 6, 6, 6, 7, 6, 7, 7, 6, 6, 6, 2, 7, 2, 3, 5,
                            2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 2, 5, 2, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 3, 1, 3, 4, 4,
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                            1, 1, 5, 5, 5, 7, 1, 5, 3, 7, 3, 5, 4, 1, 7, 4, 3, 5, 3, 3, 2, 5, 6, 1, 2, 2, 3, 5, 6, 6, 6, 6,
                            6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 6, 6, 3, 3, 3, 3, 7, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 4, 6,
                            5, 5, 5, 5, 5, 5, 9, 5, 5, 5, 5, 5, 2, 2, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5 };

 
    Pixmap pixmap(128, 128, 2);

     

    for (int i = 0, counter = 0; i < pixmap.width*pixmap.height; i += 32)
    {
        for (int j = 31; j >= 0; j--)
        {
            
            if (BIT_CHECK(defaultFontData[counter], j))
            {
                ((unsigned short *)pixmap.pixels)[i + j] = 0xffff;
            }
            else ((unsigned short *)pixmap.pixels)[i + j] = 0x00ff;
        } 
        counter++;
    }



     

    font->m_glyphs.resize(font->m_glyphCount);
    font->m_recs.resize(font->m_glyphCount);

    font->texture = Assets::CreateTexture(pixmap);
    //font->texture = Assets::LoadTexture("defaultFont", "font.png",false);

    font->texture ->SetMinFilter(FilterMode::Nearest);
    font->texture ->SetMagFilter(FilterMode::Nearest);
    font->texture ->SetWrapS(WrapMode::Repeat);
    font->texture ->SetWrapT(WrapMode::Repeat);



    Assets::AddTexture("defaultFont", font->texture);

    

    int currentLine = 0;
    int currentPosX = charsDivisor;
    int testPosX = charsDivisor;

    for (int i = 0; i < font->m_glyphCount; i++)
    {
        font->m_glyphs[i].value = 32 + i;  // First char is 32

        font->m_recs[i].x = (float)currentPosX;
        font->m_recs[i].y = (float)(charsDivisor + currentLine*(charsHeight + charsDivisor));
        font->m_recs[i].width = (float)charsWidth[i];
        font->m_recs[i].height = (float)charsHeight;

        testPosX += (int)(font->m_recs[i].width + (float)charsDivisor);

        if (testPosX >= font->texture->GetWidth())
        {
            currentLine++;
            currentPosX = 2*charsDivisor + charsWidth[i];
            testPosX = currentPosX;

            font->m_recs[i].x = (float)charsDivisor;
            font->m_recs[i].y = (float)(charsDivisor + currentLine*(charsHeight + charsDivisor));
        }
        else currentPosX = testPosX;

        // NOTE: On default font character offsets and xAdvance are not required
        font->m_glyphs[i].offsetX = 0;
        font->m_glyphs[i].offsetY = 0;
        font->m_glyphs[i].advanceX = 0;

     
    }

  //pixmap.Save("font.png");
 

    font->m_baseSize = (int)font->m_recs[0].height;

    Assets::AddFont("default", font);

    Log(0, "ASSETS: Default font loaded successfully (%i glyphs)", font->m_glyphCount);   
    return font;
}



