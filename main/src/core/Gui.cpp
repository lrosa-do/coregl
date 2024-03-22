#include "Gui.hpp"
#include "Utils.hpp"
#include "imgui_impl_sdl2.h"
#include "Device.hpp"


void Gui::EnableScissor(float x, float y, float width, float height)
{
    
    
    ImGuiIO& io = ImGui::GetIO();

      m_driver->SetScissor(
        (int)(x * io.DisplayFramebufferScale.x),
        (int)(y * io.DisplayFramebufferScale.y),
         (int)(width * io.DisplayFramebufferScale.x),
         (int)(height * io.DisplayFramebufferScale.y));

    //   m_driver->SetScissor(
    //      (int)x,
    //   //   (int)(io.DisplaySize.y - (int)(y + height)),

    //         (int)(y),
    //      (int)(width ),
    //      (int)(height));

   
}



bool Gui::CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);  

    Pixmap pixmap(width,height,4,pixels);
  //  pixmap.Save("font.png");

    m_fontTexture = new Texture2D(pixmap);
   
    


    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)(intptr_t)m_fontTexture->GetID());


    return true;
}




void Gui::Begin(int fb_width, int fb_height)
{

      
        
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = fb_width;
        io.DisplaySize.y = fb_height;
        io.DisplayFramebufferScale = ImVec2(1.0f,1.0f);

        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
   
        m_driver->SetViewport(0,0, fb_width, fb_height);
        m_driver->SetScissorTest(true);
        m_driver->SetCullFace(false);
        m_driver->SetBlend(true);
        m_driver->SetBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_driver->SetDepthTest(false);
           
    
        



}

void Gui::End()
{

    //    ImGuiIO& io = ImGui::GetIO();


        // bool open = true;
        // ImGui::ShowDemoWindow(&open);

        ImGui::Render();
        RenderDrawData(ImGui::GetDrawData());




        m_driver->SetScissorTest(false);
        m_driver->SetCullFace(true);
        m_driver->SetBlend(false);
        m_driver->SetDepthTest(true);
        glBindVertexArray(0);

       
}

ImGuiIO &Gui::getIO()
{
    return ImGui::GetIO();
}

void Gui::ProcessEvent(const SDL_Event* event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
}


void Gui::RenderDrawData(ImDrawData *draw_data)
{
   // ImGuiIO& io = ImGui::GetIO();


    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, pos));
    glVertexAttribPointer(1, 2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, uv));
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, col));
    glBindVertexArray(0);




	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
    // clip_scale.x *= 1.25f;
    // clip_scale.y *= 1.25f;

	int fb_width = (int)(draw_data->DisplaySize.x  * clip_scale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * clip_scale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    
    m_driver->SetViewport(0,0, fb_width, fb_height);


    float L = draw_data->DisplayPos.x ;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

    const float ortho_projection[4][4] =
    {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
    };

 // glm::mat4 mvp = glm::ortho(0.0f, (float)fb_width / 1.25f, (float)fb_height/ 1.25f, 0.0f, -10.0f, 10.0f);

//   glm::mat4 proj = glm::ortho(0.0f, (float)fb_width , (float)fb_height, 0.0f, -10.0f, 10.0f);
//   glm::mat4 view = glm::mat4(1.0f);
//   view = glm::scale(view, glm::vec3(1.25f));
//   glm::mat4 mvp = proj * view;
   

    m_driver->SetShader(m_shader);
   m_shader->SetMatrix4("ProjMtx", &ortho_projection[0][0]);
   // m_shader->SetMatrix4("ProjMtx", mvp);


    

    glBindVertexArray(vaoId);
    

    
    



    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];


        const GLsizeiptr vtx_buffer_size = (GLsizeiptr)cmd_list->VtxBuffer.Size * (int)sizeof(ImDrawVert);
        const GLsizeiptr idx_buffer_size = (GLsizeiptr)cmd_list->IdxBuffer.Size * (int)sizeof(ImDrawIdx);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, vtx_buffer_size, (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_size, (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);
    

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
               const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_min.x < 0.0f) { clip_min.x = 0.0f; }
                if (clip_min.y < 0.0f) { clip_min.y = 0.0f; }
                if (clip_max.x > (float)fb_width) { clip_max.x = (float)fb_width; }
                if (clip_max.y > (float)fb_height) { clip_max.y = (float)fb_height; }
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

            		EnableScissor(
                    pcmd->ClipRect.x - draw_data->DisplayPos.x, 
                    pcmd->ClipRect.y - draw_data->DisplayPos.y, 
                    pcmd->ClipRect.z - (pcmd->ClipRect.x - draw_data->DisplayPos.x),
                    pcmd->ClipRect.w - (pcmd->ClipRect.y - draw_data->DisplayPos.y));
	
                
                m_driver->SetTextureId(0,(GLuint)(intptr_t)pcmd->GetTexID());
                m_driver->DrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));

                   
        }
    }



}




Gui::Gui(Driver *driver, Device *device):m_device(device),m_driver(driver)
{



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(device->GetWindow(), device->GetContext());


    io.BackendRendererUserData = (void*)this;
    io.BackendRendererName = "opengl3";
  
    CreateFontsTexture();

        const char* vertexShaderSource = R"(
        #version 330 
        precision highp float;
        layout (location = 0) in vec2 Position;
        layout (location = 1) in vec2 UV;
        layout (location = 2) in vec4 Color;
        uniform mat4 ProjMtx;
        out vec2 Frag_UV;
        out vec4 Frag_Color;
        void main()
        {
            Frag_UV = UV;
            Frag_Color = Color;
            gl_Position = ProjMtx * vec4(Position.xy,0,1);
        }
        )";

     const char* fragmentShaderSource = R"(
        #version 330
        precision mediump float;
        uniform sampler2D texture0;
        in vec2 Frag_UV;
        in vec4 Frag_Color;
        layout (location = 0) out vec4 Out_Color;
        void main()
        {
            Out_Color = Frag_Color * texture(texture0, Frag_UV.st);
        }
        )";

    m_shader = new Shader();
    m_shader->Create(vertexShaderSource,fragmentShaderSource);
    m_shader->LoadDefaults();
    m_shader->SetInt("texture0", 0);
   // m_shader->print();

    glGenVertexArrays(1,&vaoId);
    glBindVertexArray(vaoId);
    glGenBuffers(1, &vboId);
    glGenBuffers(1, &iboId);
    glBindVertexArray(0);

    m_ready = true;
   


}

Gui::~Gui()
{
    Log(0,"Removing Gui system");



    glBindVertexArray(0);
    glDeleteBuffers(1, &vboId);
    glDeleteBuffers(1, &iboId);

    glDeleteVertexArrays(1, &vaoId);



    ImGuiIO &io = ImGui::GetIO();
    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    
    delete m_fontTexture;
    delete m_shader;

    io.Fonts->SetTexID(0);
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    m_device = nullptr;
    m_driver = nullptr;
}
