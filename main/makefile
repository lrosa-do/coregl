# CXX = g++
# CXXFLAGS = -std=c++11 -Wall -Wextra  -fsanitize=undefined -fno-omit-frame-pointer -g


CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g0 -Os -s -Wl,--strip-all -fdata-sections -ffunction-sections 
ifeq ($(OS),Windows_NT)
	LIBS = -mwindows -lmingw32  -lSDL2main -lSDL2 -lm -lopengl32 -lgdi32 -luser32 -lkernel32 
	CXXFLAGS += -Dmain=SDL_main -DWIN32 -D__WIN32__
else
	LIBS = -lSDL2 -ldl -lGL
	CXXFLAGS += #-fsanitize=undefined -fno-omit-frame-pointer
endif


INCLUDE  = -Iinclude -Isrc -Isrc/core -Isrc/utils -Isrc/main -Isrc/gui

CXXFLAGS += -Iinclude -Isrc -Isrc/core -Isrc/utils -Isrc/main -Isrc/gui
SRCDIR = src
OBJDIR = obj

LIBDIR = lib

SUBDIRS = core utils main gui

# SRCS = $(wildcard $(SRCDIR)/*.cpp)
# OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))


SRCDIRS = $(addprefix $(SRCDIR)/,$(SUBDIRS))
OBJDIRS = $(addprefix $(OBJDIR)/,$(SUBDIRS))


# SRCS = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
# OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))


SRCS_ALL = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
OBJS_ALL = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS_ALL))

# excluindo a pasta main
SRCS_WITHOUT_MAIN = $(foreach dir,$(filter-out $(SRCDIR)/main,$(SRCDIRS)),$(wildcard $(dir)/*.cpp))
OBJS_WITHOUT_MAIN = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS_WITHOUT_MAIN))


TARGET = main
STATICLIB = libcore.a
SHAREDLIB = libcore.so

ifeq ($(OS),Windows_NT)
	TARGET = core.exe
	STATICLIB = libcore.a
	SHAREDLIB = libcore.dll

endif



all: $(TARGET) 

$(TARGET): $(OBJS_ALL)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	./$(TARGET)




$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) $(OBJDIRS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $@
	@mkdir -p $(LIBDIR)

$(OBJDIRS):
	@mkdir -p $@	

static : CXXFLAGS =-std=c++11   -DSTATIC -w $(INCLUDE) -Wl,--strip-all -fdata-sections -ffunction-sections
static: $(STATICLIB)

$(STATICLIB): $(OBJS_WITHOUT_MAIN)
	ar rcs $(LIBDIR)/$@ $^

shared: CXXFLAGS += -w -DSHARED -fPIC  -Wl,--strip-all -fdata-sections -ffunction-sections
shared: $(SHAREDLIB)

$(SHAREDLIB): $(OBJS_WITHOUT_MAIN)
	$(CXX) -shared -o  $(LIBDIR)/$@ $^ $(LIBS)
	strip --strip-unneeded $(LIBDIR)/$@
	cp $(LIBDIR)/$@ ~/code/lib/$@

bin: $(TARGET)

clean:
	rm -rf $(OBJDIR)  $(LIBDIR) $(TARGET)

re: clean all

.PHONY: all clean re static shared bin
