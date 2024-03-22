

#include "Device.hpp"
#include "Input.hpp"
#include "Utils.hpp"




double GetTime()
{
    return  static_cast<double> (SDL_GetTicks())/1000;
}

Device *Device::m_instance = nullptr;

Device* Device::GetInstance()
{
    return m_instance;
}



Device::Device():IDrop()
{
    m_shouldclose = false;
    m_current = 0;
    m_previous = 0;
    m_update = 0;
    m_draw = 0;
    m_frame = 0;
    m_width = 0;
    m_height = 0;
    m_free = false;
 

    Log(0, "Device created!");
}

Device::~Device()
{
    Close();
}

void Device::Close()
{
    if (!m_free)
    {
       

        Log(0, "Closing device...");

 

        Assets::Instance().Drop();
        
        if (m_driver != nullptr)
        {
            m_driver->m_device = nullptr;
            delete m_driver;
   
            m_driver=nullptr;
        }
      
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);


        Log(0, "Device closed!");
        SDL_Quit();
        m_free = true;
    }
}

bool Device::CreateWindow(int width, int height, const std::string &tile, bool vsync)
{

   
   

   #if defined(SYSTEM_DESKTOP)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        Log(0, "Opengl desktop");
    
    #else

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        Log(0, "Opengl 3 ES");


    #endif

    float scale = 1.0f; // 125%
    m_width = static_cast<int>(width * scale);
    m_height = static_cast<int>(height * scale);



        window = SDL_CreateWindow(tile.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        if (window == NULL)
        {
            Log(0, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return false;
        }
        else
        {
            Log(0, "Window created!");
            context = SDL_GL_CreateContext(window);
            if (context == NULL)
            {
                Log(0, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
                return false;
            } else
            {
                Log(0, "OpenGL context created!");
                
                if (vsync)
                {
                    if (SDL_GL_SetSwapInterval(1) < 0)
                    {
                        Log(0, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
                    }
                }
                else
                {
                    if (SDL_GL_SetSwapInterval(0) < 0)
                    {
                        Log(0, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
                    }
                }
           
                m_free = false;
            } 
        
        
        
        
              m_driver = new Driver();
          
        

        
            if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
            {
                Log(2, "Failed to initialize GLAD");
                return -1;
            }    
   #if defined(SYSTEM_DESKTOP)
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
    #endif

            Log(0,"[DEVICE] Vendor  :  %s",glGetString(GL_VENDOR));
            Log(0,"[DEVICE] Renderer:  %s",glGetString(GL_RENDERER));
            Log(0,"[DEVICE] Version :  %s",glGetString(GL_VERSION));

            Assets::Instance().LoadDefaultAssets();

            m_driver->SetViewport(0, 0, m_width, m_height);
            m_driver->SetScissor(0, 0, m_width, m_height);


            m_driver->Init(this, m_width, m_height);
            m_driver->SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



        }

        Mouse::Init();
        Keyboard::Init();


 
    return true;

   
}

bool Device::Run()
{


        m_driver->ResetStats();


        m_current = GetTime();            // Number of elapsed seconds since InitTimer()
        m_update = m_current - m_previous;
        m_previous = m_current;


    Mouse::Update();
    Keyboard::Update();

    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
       
        m_driver->ProcessEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
            {
                m_shouldclose = true;
                break;
            }
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        m_width = event.window.data1;
                        m_height = event.window.data2;
                        m_driver->resize(m_width, m_height);
                        break;
                    }
                }
                break;
            }
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym==SDLK_ESCAPE)
                {
                    m_shouldclose = true;
                    break;
                }
               
               Keyboard::setKeyState(event.key.keysym.scancode, true);
        
                break;
            }
            
            case SDL_KEYUP:
            {
               
                Keyboard::setKeyState(event.key.keysym.scancode, false);
                break;
            
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
               
                int btn = event.button.button - 1;
                if (btn == 2) btn = 1;
                else if (btn == 1) btn = 2;
                Mouse::setMouseButton(btn, true);
            }break;
            case SDL_MOUSEBUTTONUP:
            {
                int btn = event.button.button - 1;
                if (btn == 2) btn = 1;
                else if (btn == 1) btn = 2;
                Mouse::setMouseButton(btn, false);
                
                break;
            }
            case SDL_MOUSEMOTION:
            {
               Mouse::setMousePosition(event.motion.x, event.motion.y,event.motion.xrel, event.motion.yrel);
            break;   
            }
            
            case SDL_MOUSEWHEEL:
            {
                Mouse::setMouseWheel(event.wheel.x, event.wheel.y);
                break;
            }

             
        }
    } 
    
    return !m_shouldclose;
}

void Device::Swap()
{
    
    SDL_GL_SwapWindow(window);
  //  State::CalculateFPS();
    m_current = GetTime();
    m_draw = m_current - m_previous;
    m_previous = m_current;

    m_frame = m_update + m_draw;

    // // Wait for some milliseconds...
    // if (m_frame < m_target)
    // {
    //     Wait((float)(m_target -m_frame)*1000.0f);

    //     m_current = GetTime();
    //     double waitTime = m_current - m_previous;
    //     m_previous = m_current;

    //     m_frame += waitTime;      // Total frame time: update + draw + wait
    // }

   }

void Device::Wait(float ms)
{
    SDL_Delay(ms);
}




int Device::GetFPS(void)
{
    #define FPS_CAPTURE_FRAMES_COUNT    30      // 30 captures
    #define FPS_AVERAGE_TIME_SECONDS   0.5f     // 500 millisecondes
    #define FPS_STEP (FPS_AVERAGE_TIME_SECONDS/FPS_CAPTURE_FRAMES_COUNT)

    static int index = 0;
    static float history[FPS_CAPTURE_FRAMES_COUNT] = { 0 };
    static float average = 0, last = 0;
    float fpsFrame = GetFrameTime();

    if (fpsFrame == 0) return 0;

    if ((GetTime() - last) > FPS_STEP)
    {
        last = (float)GetTime();
        index = (index + 1)%FPS_CAPTURE_FRAMES_COUNT;
        average -= history[index];
        history[index] = fpsFrame/FPS_CAPTURE_FRAMES_COUNT;
        average += history[index];
    }

    return (int)roundf(1.0f/average);
}

float Device::GetFrameTime(void)
{
    return (float)m_frame;
}

double Device::GetTime(void)
{
    return (double) SDL_GetTicks()/1000.0;
}

u32 Device::GetTicks(void)
{
    return SDL_GetTicks();
}


int Device::GetWidth()
{
    return m_width;
}

int Device::GetHeight()
{
    return m_height;
}

void Device::SetShouldClose(bool shouldClose)
{
    m_shouldclose = shouldClose;
}

bool Device::ShouldClose()
{
    return m_shouldclose;
}

void Device::resize (int width, int height)
{
    m_width = width;
    m_height = height;
    m_driver->resize(width, height);

}

void Device::SetSize(int width, int height)
{
    m_width = width;
    m_height = height;
    SDL_SetWindowSize(window, width, height);
}

void Device::SetFullScreen(bool fullScreen)
{
    if (fullScreen)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }
    else
    {
        SDL_SetWindowFullscreen(window, 0);
    }
}

void Device::SetIcon(const std::string &icon)
{
    SDL_Surface *surface = SDL_LoadBMP(icon.c_str());
    if (surface != NULL)
    {
        SDL_SetWindowIcon(window, surface);
        SDL_FreeSurface(surface);
    }
    else
    {
        Log(0, "Unable to load icon! SDL Error: %s\n", SDL_GetError());
    }
}

void Device::SetCursor(const std::string &cursor)
{
    SDL_Surface *surface = SDL_LoadBMP(cursor.c_str());
    if (surface != NULL)
    {
        SDL_Cursor *c = SDL_CreateColorCursor(surface, 0, 0);
        SDL_SetCursor(c);
        SDL_FreeCursor(c);
        SDL_FreeSurface(surface);
    }
    else
    {
        Log(0, "Unable to load cursor! SDL Error: %s\n", SDL_GetError());
    }
}

void Device::SetRelativeMouse(bool relative)
{
    if (relative)
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    else
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

void Device::SetMousePosition(int x, int y)
{
    SDL_WarpMouseInWindow(window, x, y);
}

void Device::SetMouseVisible(bool visible)
{
    if (visible)
    {
        SDL_ShowCursor(SDL_ENABLE);
    }
    else
    {
        SDL_ShowCursor(SDL_DISABLE);
    }
}

void Device::SetMouseGrab(bool grab)
{
    if (grab)
    {
        SDL_SetWindowGrab(window, SDL_TRUE);
    }
    else
    {
        SDL_SetWindowGrab(window, SDL_FALSE);
    }
}

void Device::SetMouseFocus(bool focus)
{
    if (focus)
    {
        SDL_SetWindowInputFocus(window);
    }
    else
    {
        SDL_SetWindowInputFocus(NULL);
    }
}

void Device::SetClipboardText(const std::string &text)
{
    SDL_SetClipboardText(text.c_str());
}

std::string Device::GetClipboardText()
{
    return SDL_GetClipboardText();
}

void Device::SetWindowTitle(const std::string &title)
{
    SDL_SetWindowTitle(window, title.c_str());
}

void Device::SetWindowPosition(int x, int y)
{
    SDL_SetWindowPosition(window, x, y);
}

void Device::SetWindowBordered(bool bordered)
{
    if (bordered)
    {
        SDL_SetWindowBordered(window, SDL_TRUE);
    }
    else
    {
        SDL_SetWindowBordered(window, SDL_FALSE);
    }
}

void Device::SetWindowResizable(bool resizable)
{
    if (resizable)
    {
        SDL_SetWindowResizable(window, SDL_TRUE);
    }
    else
    {
        SDL_SetWindowResizable(window, SDL_FALSE);
    }
}

void Device::SetWindowMaximized(bool maximized)
{
    if (maximized)
    {
        SDL_MaximizeWindow(window);
    }
    else
    {
        SDL_RestoreWindow(window);
    }
}

void Device::SetWindowMinimized(bool minimized)
{
    if (minimized)
    {
        SDL_MinimizeWindow(window);
    }
    else
    {
        SDL_RestoreWindow(window);
    }
}

void Device::SetWindowOpacity(float opacity)
{
    SDL_SetWindowOpacity(window, opacity);
}

void Device::SetWindowBrightness(float brightness)
{
    SDL_SetWindowBrightness(window, brightness);
}



void Device::SetWindowInputFocus()
{
    SDL_SetWindowInputFocus(window);
}

void Device::SetWindowMouseFocus()
{
    SDL_SetWindowInputFocus(window);
}

void Device::SetWindowKeyboardFocus()
{
    SDL_SetWindowInputFocus(window);
}

void Device::SetWindowGrab()
{
    SDL_SetWindowGrab(window, SDL_TRUE);
}

void Device::SetWindowIcon(const std::string &icon)
{
    SDL_Surface *surface = SDL_LoadBMP(icon.c_str());
    if (surface != NULL)
    {
        SDL_SetWindowIcon(window, surface);
        SDL_FreeSurface(surface);
    }
    else
    {
        Log(0, "Unable to load icon! SDL Error: %s\n", SDL_GetError());
    }
}

void Device::SetWindowFullscreen()
{
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void Device::SetWindowFullscreenDesktop()
{
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}


//********************************************************************************************************************
// FILE/MEMORY STREAM
//********************************************************************************************************************

Stream::Stream():IDrop()
{
    f_file = nullptr;
    m_open = false;
    m_size = 0;
}

Stream::~Stream()
{
    Close();
}

void Stream::Close()
{
    if (f_file != nullptr)
    {
        Log(0, "STREAM: closed");
        SDL_RWclose(f_file);
        f_file = nullptr;
    }
}

u64 Stream::Read(void *buffer, u64 size)
{
    return SDL_RWread(f_file, buffer, 1, size);
}

u64 Stream::Write(const void *buffer, u64 size)
{
    return SDL_RWwrite(f_file, buffer, 1, size);
}

u64 Stream::Cursor() const
{
    return SDL_RWtell(f_file);
}   

std::string Stream::ReadString() 
{
    static std::string value;
    value="";
    char c;
    while ((c = ReadChar()) != 0)
    {
        value += c;
    }
    return value;
}

size_t  Stream::WriteString(const std::string &value)
{
    size_t  size = 0;
    for (size_t i = 0; i < value.size(); i++)
    {
        size += WriteChar(value[i]);
    }
    size += WriteChar(0);
    return size;
}

size_t  Stream::WriteUTFString(const std::string &value)
{
    size_t  size = 0;
    size = WriteInt(value.size());
    for (size_t i = 0; i < value.size(); i++)
    {
        size += WriteChar(value[i]);
    }
    return size;
}

 std::string Stream::ReadUTFString() 
{

    int size = ReadInt();
    std::string value;

    for (int i = 0; i < size; i++)
    {
       value +=  ReadChar();
    }
 
    return value;
}

char Stream::ReadChar()
{
    char value;
    SDL_RWread(f_file, &value, sizeof(char), 1);
    return value;
}

size_t  Stream::WriteChar(char value)
{
    return SDL_RWwrite(f_file, &value, sizeof(char), 1);
}

unsigned char Stream::ReadByte()
{
    unsigned char value;
    SDL_RWread(f_file, &value, sizeof(unsigned char), 1);
    return value;
}

short Stream::ReadShort()
{
    short value = ReadByte();
    value |= (ReadByte() << 8);
    return value;
}

int Stream::ReadInt()
{
    int value = ReadByte();
    value |= (ReadByte() << 8);
    value |= (ReadByte() << 16);
    value |= (ReadByte() << 24);
    return value;
}

long Stream::ReadLong()
{
    long value = ReadByte();
    value |= (ReadByte() << 8);
    value |= (ReadByte() << 16);
    value |= (ReadByte() << 24);
    return value;
}

float Stream::ReadFloat()
{
    float value;
    SDL_RWread(f_file, &value, sizeof(float), 1);
    return value;
}


double Stream::ReadDouble()
{
    double value;
    SDL_RWread(f_file, &value, sizeof(double), 1);
    return value;
}

size_t  Stream::WriteByte(unsigned char value)
{
   return SDL_RWwrite(f_file, &value, sizeof(unsigned char), 1);
}

size_t  Stream::WriteShort(short value)
{
    WriteByte(value & 0xFF);
    WriteByte((value >> 8) & 0xFF);
    return 2;
}

size_t  Stream::WriteInt(int value)
{
    WriteByte(value & 0xFF);
    WriteByte((value >> 8) & 0xFF);
    WriteByte((value >> 16) & 0xFF);
    WriteByte((value >> 24) & 0xFF);
    return 4;
}

size_t  Stream::WriteLong(long value)
{
    WriteByte(value & 0xFF);
    WriteByte((value >> 8) & 0xFF);
    WriteByte((value >> 16) & 0xFF);
    WriteByte((value >> 24) & 0xFF);
    return 4;
}

size_t  Stream::WriteFloat(float value)
{
    SDL_RWwrite(f_file, &value, sizeof(float), 1);
    return 4;
}

size_t  Stream::WriteDouble(double value)
{
    SDL_RWwrite(f_file, &value, sizeof(double), 1);
    return 8;
}

u64 Stream::Seek(u64 offset, bool relative)
{
    return SDL_RWseek(f_file, offset, relative ? RW_SEEK_CUR : RW_SEEK_SET);
}


FileStream::FileStream():Stream()
{   


}

FileStream::FileStream(const std::string &filePath, const std::string &mode):Stream()
{
    f_file = SDL_RWFromFile(filePath.c_str(), mode.c_str());
    if (f_file == nullptr)
    {
        Log(1, "[FILESTRAM] Cant open: %s", filePath.c_str());
        m_open = false;
        return;
    }

    SDL_RWseek(f_file, 0, RW_SEEK_END);
    m_size = SDL_RWtell(f_file);
    SDL_RWseek(f_file, 0, RW_SEEK_SET);
    m_open = true;
    Log(0, "FILE: file  read: %s", filePath.c_str());
}

bool FileStream::Open(const std::string &filePath, const std::string &mode)
{
    f_file = SDL_RWFromFile(filePath.c_str(), mode.c_str());
    if (f_file == nullptr)
    {
        Log(1, " Cant open: %s", filePath.c_str());
        m_open = false;
        return false;
    }
    SDL_RWseek(f_file, 0, RW_SEEK_END);
    m_size = SDL_RWtell(f_file);
    SDL_RWseek(f_file, 0, RW_SEEK_SET);
    m_open = true;
    Log(0, "FILE: file  read: %s", filePath.c_str());
    return true;
}

bool FileStream::Create(const std::string &filePath, bool overwrite)
{
    if (overwrite)
    {
        f_file = SDL_RWFromFile(filePath.c_str(), "wb");
    }
    else
    {
        f_file = SDL_RWFromFile(filePath.c_str(), "ab");
    }
    if (f_file == nullptr)
    {
        Log(1, " Cant create: %s", filePath.c_str());
        m_open = false;
        return false;
    }
    m_open = true;
    Log(0, "FILE: file created: %s", filePath.c_str());
    return true;
}






ByteStream::ByteStream():Stream()
{
    m_open = false;
    m_size = 0;
    m_capacity = 0;
    m_offset = 0;
    m_data = nullptr;
    m_owner = false;

}

bool ByteStream::Load(const void *data, u64 size)
{
    f_file = SDL_RWFromConstMem(data, size);
    if (f_file == nullptr)
    {
        Log(1, " Cant open: %s", "ByteStream");
        m_open = false;
        return false;
    }
    m_size = size;
    m_open = true;
    Log(0, "BYTE: file  read: %s", "ByteStream");
    return true;
}

bool ByteStream::Load(void *data, u64 size)
{
    f_file = SDL_RWFromMem(data, size);
    if (f_file == nullptr)
    {
        Log(1, " Cant open: %s", "ByteStream");
        m_open = false;
        return false;
    }
    m_size = size;
    m_open = true;
    Log(0, "BYTE: file  read: %s", "ByteStream");
    return true;
}

bool ByteStream::Create(u64 size)
{
    m_data = malloc(size);
    f_file = SDL_RWFromMem(m_data, size);
    if (f_file == nullptr)
    {
        Log(1, " Cant create: %s", "ByteStream");
        m_open = false;
        return false;
    }
    m_size = size;
    m_open = true;
    m_owner = true;
    Log(0, "BYTE: file created: %s", "ByteStream");
    return true;
}

void ByteStream::Close()
{
    if (!m_open)
    {
        return ;
    }
    m_open = false;
    if (m_owner)
    {
        free(m_data);
    }
    Stream::Close();
}

void* ByteStream::GetPointer() const
{
    if (m_data == nullptr)
    {
        return nullptr;
    }
    return m_data;
}

void* ByteStream::GetPointer(u64 offset) const
{
    if (m_data == nullptr)
    {
        return nullptr;
    }
    return (void*)((u8*)m_data + offset);
}







 Device* CreateDevice(int width, int height, const char *tile, bool vsync)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        Log(0, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return nullptr;
    }
 
    Device *device = new Device();
    if (!device->CreateWindow(width, height, tile, vsync))
    {
        Log(2, "Failed to create device!");
        delete device;
        return nullptr;
    }
    Device::m_instance = device;

    return device;
}

