

#include <stdarg.h>
#include <time.h>


#include "Utils.hpp"
#include <SDL2/SDL.h>
#include "glad/glad.h"


    #if defined(_WIN32) && (defined(_MSC_VER))
        #include <io.h>         // _findfirst and _findnext set errno iff they return -1
        #include <stdlib.h>
        #include <string.h>
        #include <errno.h>
        #include <direct.h>             // Required for: _getch(), _chdir()
        #include "dirent.h"            // Required for: SetConsoleTextAttribute()

        #define GETCWD _getcwd          // NOTE: MSDN recommends not to use getcwd(), chdir()
        #define CHDIR _chdir
        #include <io.h>                 // Required for: _access() [Used in FileExists()]
        #ifndef MAX_PATH
        #define MAX_PATH 1025
        #endif
    
    #else
        #include <unistd.h>             // Required for: getch(), chdir() (POSIX), access()
        #define GETCWD getcwd
        #define CHDIR chdir
    #endif


bool SetCurrentDirectory(const std::string& directory)
{
	return CHDIR(directory.c_str()) == 0;
}


void Log(int severity, const char* fmt, ...)
{




		const char* type="info";
        const char* color = CONSOLE_COLOR_GREEN;
		switch (severity) 
        {
			case  0:
				type = "info";
				color = CONSOLE_COLOR_GREEN;
				break;
			case 1:
				type = "warning";
				color = CONSOLE_COLOR_PURPLE;
				break;
            case 2:
				type = "error";
				color = CONSOLE_COLOR_RED;
				break;
			default:
				break;
		}

		time_t rawTime;
		struct tm* timeInfo;
		char timeBuffer[80];

		time(&rawTime);
		timeInfo = localtime(&rawTime);

		strftime(timeBuffer, sizeof(timeBuffer), "[%H:%M:%S]", timeInfo);

		char consoleFormat[1024];
		snprintf(consoleFormat, 1024, "%s%s %s%s%s: %s\n", CONSOLE_COLOR_CYAN,
				timeBuffer, color, type, CONSOLE_COLOR_RESET, fmt);


		char fileFormat[1024];
		snprintf(fileFormat, 1024, "%s %s: %s\n", timeBuffer, type, fmt);

		va_list argptr;

		va_start(argptr, fmt);
		vprintf(consoleFormat, argptr);
		va_end(argptr);

		char buf[4096];

		va_start(argptr, fmt);
		vsprintf(buf, fmt, argptr);
		va_end(argptr);

        #ifdef SYSTEM_WINDOWS
        SDL_Log(buf);

        #else
   
        #endif


       
}




std::string ExtractFilename(const std::string& path)
{
    size_t index = path.find_last_of("/\\");
    if (index != std::string::npos)
    {
        return path.substr(index + 1);
    }
    return path;
}

std::string ExtractDirectory(const std::string& path)
{
    size_t index = path.find_last_of("/\\");
    if (index != std::string::npos)
    {
        return path.substr(0, index);
    }
    return path;
}

std::string ExtractFileExtension(const std::string& path)
{
    size_t index = path.find_last_of(".");
    if (index != std::string::npos)
    {
        return path.substr(index + 1);
    }
    return path;
}

std::string RemoveFileExtension(const std::string& path)
{
    size_t index = path.find_last_of(".");
    if (index != std::string::npos)
    {
        return path.substr(0, index);
    }
    return path;
}

std::string ExtractFilenameNoExtension(const std::string& path)
{
    std::string filename = ExtractFilename(path);
    return RemoveFileExtension(filename);
}

bool FileExists(const std::string& path)
{
    return access(path.c_str(), F_OK) != -1;
}

// bool DirectoryExists(const std::string& path)
// {
//     struct stat info;
//     if (stat(path.c_str(), &info) != 0) return false;
//     return (info.st_mode & S_IFDIR) != 0;
// }

// bool CreateDirectory(const string& path)
// {
//     return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
// }

// bool DeleteDirectory(const string& path)
// {
//     return rmdir(path.c_str()) == 0;
// }

// bool CopyFile(const string& source, const string& destination)
// {
//     ifstream src(source, ios::binary);
//     ofstream dst(destination, ios::binary);
//     dst << src.rdbuf();
//     return src && dst;
// }

// bool MoveFile(const string& source, const string& destination)
// {
//     return rename(source.c_str(), destination.c_str()) == 0;
// }

// bool DeleteFile(const string& path)
// {
//     return remove(path.c_str()) == 0;
// }

// vector<string> GetFilesInDirectory(const string& path, const string& extension)
// {
//     vector<string> files;
//     DIR* directory = opendir(path.c_str());
//     if (directory)
//     {
//         struct dirent* entry;
//         while ((entry = readdir(directory)) != NULL)
//         {
//             string filename = entry->d_name;
//             if (filename != "." && filename != "..")
//             {
//                 if (extension.empty() || filename.find(extension) != string::npos)
//                 {
//                     files.push_back(filename);
//                 }
//             }
//         }
//         closedir(directory);
//     }
//     return files;
// }

// vector<string> GetDirectoriesInDirectory(const string& path)
// {
//     vector<string> directories;
//     DIR* directory = opendir(path.c_str());
//     if (directory)
//     {
//         struct dirent* entry;
//         while ((entry = readdir(directory)) != NULL)
//         {
//             string filename = entry->d_name;
//             if (filename != "." && filename != "..")
//             {
//                 if (DirectoryExists(path + "/" + filename))
//                 {
//                     directories.push_back(filename);
//                 }
//             }
//         }
//         closedir(directory);
//     }
//     return directories;
// }

// string GetWorkingDirectory()
// {
//     char buffer[MAX_PATH];
//     return GETCWD(buffer, MAX_PATH) ? string(buffer) : string("");
// }

// string GetExecutableDirectory()
// {
//     char buffer[MAX_PATH];
//     return GETCWD(buffer, MAX_PATH) ? string(buffer) : string("");
// }
// vector<string> SplitString(const string& string, const string& delimiters)
// {
//     size_t start = 0;
//     size_t end = string.find_first_of(delimiters);

//     vector<string> result;

//     while (end <= string::npos)
//     {
//         string token = string.substr(start, end - start);
//         if (!token.empty())
//         {
//             result.push_back(token);
//         }

//         if (end == string::npos)
//         {
//             break;
//         }

//         start = end + 1;
//         end = string.find_first_of(delimiters, start);
//     }

//     return result;

// }


unsigned char* LoadFileData(const char* fileName, unsigned int* bytesRead)
{
    unsigned char* data = NULL;
    *bytesRead = 0;

 
        SDL_RWops* file = SDL_RWFromFile(fileName, "rb");


        if (file != NULL)
        {
            unsigned int size = (int)SDL_RWsize(file);

            if (size > 0)
            {
                data = (unsigned char*)malloc(size * sizeof(unsigned char));

                unsigned int count = (unsigned int)SDL_RWread(file, data, sizeof(unsigned char), size);
                *bytesRead = count;

                if (count != size) Log(1, "FILEIO: [%s] File partially loaded", fileName);
                else  Log(0, "FILEIO: [%s] File loaded successfully", fileName);
            }
            else Log(1, "FILEIO: [%s] Failed to read file", fileName);
            SDL_RWclose(file);
        }
        else Log(1, "FILEIO: [%s] Failed to open file", fileName);

    return data;
}

bool SaveFileData(const char* fileName, void* data, unsigned int bytesToWrite)
{
    bool success = false;

   
        SDL_RWops* file = SDL_RWFromFile(fileName, "wb");
        if (file != NULL)
        {
            unsigned int count = (unsigned int)SDL_RWwrite(file, data, sizeof(unsigned char), bytesToWrite);
            if (count == 0) Log(2, "FILEIO: [%s] Failed to write file", fileName);
            else if (count != bytesToWrite) Log(1, "FILEIO: [%s] File partially written", fileName);
            else  Log(0, "FILEIO: [%s] File saved successfully", fileName);

            int result = SDL_RWclose(file);
            if (result == 0) success = true;
        }
        else Log(2, "FILEIO: [%s] Failed to open file", fileName);
 
    return success;
}

char* LoadFileText(const char* fileName)
{
    char* text = NULL;

    if (fileName != NULL)
    {
        SDL_RWops* textFile = SDL_RWFromFile(fileName, "rt");
        if (textFile != NULL)
        {
            unsigned  int size = (int)SDL_RWsize(textFile);
            if (size > 0)
            {
                text = (char*)malloc((size + 1) * sizeof(char));
                unsigned int count = (unsigned int)SDL_RWread(textFile, text, sizeof(char), size);
                if (count < size) text = (char*)realloc(text, count + 1);
                text[count] = '\0';

                Log(0, "FILEIO: [%s] Text file loaded successfully", fileName);
            }
            else Log(2, "FILEIO: [%s] Failed to read text file", fileName);

            SDL_RWclose(textFile);
        }
        else Log(2, "FILEIO: [%s] Failed to open text file", fileName);
    }
    else Log(2, "FILEIO: File name provided is not valid");

    return text;
}

bool SaveFileText(const char* fileName, char* text)
{
    bool success = false;

    if (fileName != NULL)
    {
        SDL_RWops* file = SDL_RWFromFile(fileName, "wt");
        if (file != NULL)
        {
            size_t strLen = SDL_strlen(text);
            int count = SDL_RWwrite(file, text, 1, strLen);
            if (count < 0) Log(3, "FILEIO: [%s] Failed to write text file", fileName);
            else Log(0, "FILEIO: [%s] Text file saved successfully", fileName);

            int result = SDL_RWclose(file);
            if (result == 0) success = true;
        }
        else Log(2, "FILEIO: [%s] Failed to open text file", fileName);
    }
    else Log(2, "FILEIO: File name provided is not valid");
    return success;
}


const char *TextFormat(const char *text, ...)
{
    #define MAX_TEXTFORMAT_BUFFERS 4        // Maximum number of static buffers for text formatting

    // We create an array of buffers so strings don't expire until MAX_TEXTFORMAT_BUFFERS invocations
    static char buffers[MAX_TEXTFORMAT_BUFFERS][MAX_TEXT_BUFFER_LENGTH] = { 0 };
    static int  index = 0;

    char *currentBuffer = buffers[index];
    memset(currentBuffer, 0, MAX_TEXT_BUFFER_LENGTH);   // Clear buffer before using

    va_list args;
    va_start(args, text);
    vsnprintf(currentBuffer, MAX_TEXT_BUFFER_LENGTH, text, args);
    va_end(args);

    index += 1;     // Move to next buffer for next function call
    if (index >= MAX_TEXTFORMAT_BUFFERS) index = 0;

    return currentBuffer;
}


void gl_check_error(const char* function, const char *file, int line)
{
    GLenum error = 0;

    while ((error = glGetError()) != GL_NO_ERROR)
    {
  
        switch (error)
        {
        case GL_INVALID_ENUM: Log(2,"GL_INVALID_ENUM");
            break;
        case GL_INVALID_VALUE: Log(2,"GL_INVALID_VALUE");
            break;
        case GL_INVALID_OPERATION: Log(2,"INVALID_OPERATION");
            break;
        case GL_STACK_OVERFLOW: Log(2,"STACK_OVERFLOW");
            break;
        case GL_STACK_UNDERFLOW: Log(2,"STACK_UNDERFLOW");
            break;
        case GL_OUT_OF_MEMORY: Log(2,"OUT_OF_MEMORY");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: Log(2,"INVALID_FRAMEBUFFER_OPERATION");
            break;
        case GL_CONTEXT_LOST: Log(2,"GL_CONTEXT_LOST");
            break;
        case GL_TABLE_TOO_LARGE: Log(2,"GL_TABLE_TOO_LARGE");
            break;
        default:
            Log(2,"Unknown error code %d", error);
        }
        Log(2," encountered at function '%s' (%s:%d)\n", function, file, line);
    }

}



/*bool ChangeCurrentDirectory(const char* path)
{
	return chdir(path) == 0;
}  */ 


void glDebugOutput(u32 source,
                   u32 type,
                   u32 id,
                   u32 severity,
                   s32 length,
                   const char *message,
                   const void *userParam)
{
    (void)length;
    (void)userParam;
    GLuint ignore_ids[1] = { 131185 }; // "will use video memory..."

    for (int i = 0 ; i <(int) ARRAY_SIZE_IN_ELEMENTS(ignore_ids) ; i++) 
    {
        if (id == ignore_ids[i]) {
            return;
        }
    }

  //  Log(1,"!!! Debug callback !!!\n");
    Log(2,"Debug message: id %d, %s", id, message);

 //   Log(1,"Message source: ");
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             Log(1,"API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   Log(1,"Window System"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: Log(1,"Shader Compiler"); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     Log(1,"Third Party"); break;
    case GL_DEBUG_SOURCE_APPLICATION:     Log(1,"Application"); break;
    case GL_DEBUG_SOURCE_OTHER:           Log(1,"Other"); break;
    }

  //  printf("Error type: ");
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               Log(2,"Error"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: Log(2,"Deprecated Behaviour"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  Log(2,"Undefined Behaviour"); break;
    case GL_DEBUG_TYPE_PORTABILITY:         Log(2,"Portability"); break;
    case GL_DEBUG_TYPE_PERFORMANCE:         Log(2,"Performance"); break;
    case GL_DEBUG_TYPE_MARKER:              Log(2,"Marker"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          Log(2,"Push Group"); break;
    case GL_DEBUG_TYPE_POP_GROUP:           Log(2,"Pop Group"); break;
    case GL_DEBUG_TYPE_OTHER:               Log(2,"Other"); break;
    }


    // switch (severity)
    // {
    // case GL_DEBUG_SEVERITY_HIGH:         Log(1,"Severity High"); break;
    // case GL_DEBUG_SEVERITY_MEDIUM:       Log(1,"Severity Medium"); break;
    // case GL_DEBUG_SEVERITY_LOW:          Log(1,"Severity Low"); break;
    // case GL_DEBUG_SEVERITY_NOTIFICATION: Log(1,"Severity Notification"); break;
    // }
}

