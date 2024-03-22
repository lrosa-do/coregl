#pragma once
#include "Config.hpp"
#include "Math.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "imgui.h"

class Device;
class Driver;

class Gui 
{
    public:

    void Begin(int fb_width, int fb_height);
    void End();

    
    ImGuiIO& getIO();

    private:
        friend class Device;
        friend class Driver;
        Gui(Driver *driver, Device *device);
        ~Gui();
        void ProcessEvent(const SDL_Event* event);


        void RenderDrawData(ImDrawData* draw_data);


       

        bool     CreateFontsTexture();
        void     TriangleVert(ImDrawVert& idx_vert);
        void     RenderTriangles(unsigned int count, int indexStart, const ImVector<ImDrawIdx>& indexBuffer, const ImVector<ImDrawVert>& vertBuffer);
        void     EnableScissor(float x, float y, float width, float height);


  
        Texture2D *m_fontTexture{nullptr};
        Shader *m_shader{nullptr};
        Device *m_device{nullptr};
        Driver *m_driver{nullptr};
        bool m_ready{false};
        

        unsigned int vaoId;         
        unsigned int vboId;      
        unsigned int iboId;   
};