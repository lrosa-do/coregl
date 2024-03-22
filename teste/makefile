CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra # -Winvalid-pch --include=../include/pch.h
ifeq ($(OS),Windows_NT)
       LIBS =-L../lib/win  -mwindows -lmingw32 -lgame -lSDL2main -lSDL2 -lm  -lopengl32 -lgdi32 -luser32 -lkernel32 
		CXXFLAGS += -Dmain=SDL_main -DWIN32 -D__WIN32__
else
    LIBS =-L../main/lib -lcore -lSDL2 -ldl -lGL
    CXXFLAGS +=   #-fsanitize=undefined -fno-omit-frame-pointer 
endif



CXXFLAGS += -I../main/include

SRCDIR = src
OBJDIR = obj

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

TARGET = game

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	./$(TARGET)
	rm -f $(TARGET)
	rm -f obj/main.o

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
