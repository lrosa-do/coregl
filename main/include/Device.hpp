#pragma once

#include <string>
#include  "Config.hpp"
#include  <SDL2/SDL.h>
#include "glad/glad.h"
#include  "DriverGL.hpp"
#include  "Assets.hpp"


class   Device: public IDrop
{
    private:

 
 
        SDL_Window *window;
        SDL_GLContext context;

        bool m_free{false};
        int m_width;
        int m_height;


        Driver *m_driver;

        
        bool m_shouldclose;
  
        double m_current;                 
        double m_previous;                  
        double m_update;                    
        double m_draw;                       
        double m_frame;   



    private:
        friend class Driver;
        static Device *m_instance;
            
        Device();
        ~Device();

          friend Device* CreateDevice(int width, int height, const char *tile, bool vsync);
         
          bool CreateWindow(int width, int height, const std::string &tile, bool vsync = true);

          void resize (int width, int height);

    
    public:

        static Device* GetInstance();



  

    void Close();

    bool Run();

    void Swap();

    void Wait(float ms);
    void SetTargetFPS(int fps);
    int GetFPS(void);

    SDL_Window* GetWindow(){return window;};
    SDL_GLContext GetContext(){return context;};
    Driver* GetDriver(){return m_driver;};




    float GetFrameTime(void);
    double GetTime(void);

    u32 GetTicks(void) ;

    int GetWidth();
    int GetHeight();

        
    void SetShouldClose(bool shouldClose);
    bool ShouldClose();
    void SetSize(int width, int height);
    void SetFullScreen(bool fullScreen);
    void SetIcon(const std::string &icon);
    void SetCursor(const std::string &cursor);
    void SetRelativeMouse(bool relative);
    void SetMousePosition(int x, int y);
    void SetMouseVisible(bool visible);
    void SetMouseGrab(bool grab);
    void SetMouseFocus(bool focus);
    void SetClipboardText(const std::string &text);
    std::string GetClipboardText();
    void SetWindowTitle(const std::string &title);
    void SetWindowPosition(int x, int y);
    void SetWindowBordered(bool bordered);
    void SetWindowResizable(bool resizable);
    void SetWindowMaximized(bool maximized);
    void SetWindowMinimized(bool minimized);
    void SetWindowOpacity(float opacity);
    void SetWindowBrightness(float brightness);
    void SetWindowInputFocus();
    void SetWindowMouseFocus();
    void SetWindowKeyboardFocus();
    void SetWindowGrab();
    void SetWindowIcon(const std::string &icon);
    void SetWindowFullscreen();
    void SetWindowFullscreenDesktop();



  
};

class  Stream : public IDrop
{
    public:
    Stream();

 
    u64 Read(void* buffer, u64 size) ;
    u64 Write(const void* buffer, u64 size) ;
    u64 Cursor() const ;
    u64 Seek(u64 offset, bool relative = false) ;

    virtual void Close() ;

    u64  Size() const { return m_size; }
    bool IsOpen() const { return m_open; }

    char ReadChar();
    unsigned char ReadByte();
    short         ReadShort();
    int           ReadInt();
    long          ReadLong();
    float         ReadFloat();
    double        ReadDouble();



    std::string ReadString() ;
    size_t  WriteString(const std::string& value);

    size_t  WriteUTFString(const std::string& value);
    std::string  ReadUTFString()  ;

    size_t  WriteChar(char value);
    size_t  WriteByte(unsigned char value);
    size_t  WriteShort(short value);
    size_t  WriteInt(int value);
    size_t  WriteLong(long value);
    size_t  WriteFloat(float value);
    size_t  WriteDouble(double value);
protected:
    SDL_RWops* f_file;
    bool m_open;
    u64 m_size;

    virtual ~Stream();
};

class  FileStream : public Stream
{
public:
    FileStream();
    FileStream(const std::string& filePath, const std::string& mode);
    bool Open(const std::string& filePath, const std::string& mode);
    bool Create(const std::string& filePath, bool overwrite = false);

};


class  ByteStream : public Stream
{
public:
    ByteStream();


    bool Load(const void* data, u64 size);
    bool Load(void* data, u64 size);
    bool Create(u64 size);

    void Close() override;



    void* GetPointer() const;

    void* GetPointer(u64 offset) const;


    private:
    void* m_data;
    bool m_owner;
    u64 m_offset;
    u64 m_capacity;
    u64 m_size;

 
};





Device* CreateDevice(int width, int height, const char *tile, bool vsync);

