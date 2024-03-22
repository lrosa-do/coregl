#pragma once


#include  "Config.hpp"
#include <string>



#define MAX_TEXT_BUFFER_LENGTH              1024      

#define GLSL(src) "#version 330\n" #src



#ifdef SYSTEM_WINDOWS

#define CONSOLE_COLOR_RESET ""
#define CONSOLE_COLOR_GREEN ""
#define CONSOLE_COLOR_RED ""
#define CONSOLE_COLOR_PURPLE ""
#define CONSOLE_COLOR_CYAN ""


#else

#define CONSOLE_COLOR_RESET "\033[0m"
#define CONSOLE_COLOR_GREEN "\033[1;32m"
#define CONSOLE_COLOR_RED "\033[1;31m"
#define CONSOLE_COLOR_PURPLE "\033[1;35m"
#define CONSOLE_COLOR_CYAN "\033[0;36m"

#endif // SYSTEM_WINDOWS









void Log(int severity, const char* fmt, ...);


#define ERROR0(msg) Error(2,__FILE__, __LINE__, msg)
#define ERROR(msg, ...) Error(2,__FILE__, __LINE__, msg, __VA_ARGS__)


#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))




bool SetCurrentDirectory(const std::string& directory);

bool SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite);
char* LoadFileText(const char *fileName);

unsigned char *LoadFileData(const char *fileName, unsigned int *bytesRead);
 bool SaveFileText(const char *fileName, char *text);

const char *TextFormat(const char *text, ...);
//std::string getFileExtension(const std::string& filename) ;


std::string ExtractFilename(const std::string& path);
std::string ExtractDirectory(const std::string& path);
std::string ExtractFileExtension(const std::string& path);
std::string RemoveFileExtension(const std::string& path);
std::string ExtractFilenameNoExtension(const std::string& path);
bool FileExists(const std::string& path);
// bool DirectoryExists(const std::string& path);
// bool CreateDirectory(const std::string& path);
// bool DeleteDirectory(const std::string& path);
// bool CopyFile(const std::string& source, const std::string& destination);
// bool MoveFile(const std::string& source, const std::string& destination);
// bool DeleteFile(const std::string& path);

void glDebugOutput(u32 source,
                   u32 type,
                   u32 id,
                   u32 severity,
                   s32 length,
                   const char *message,
                   const void *userParam);

void gl_check_error(const char* function, const char *file, int line);


