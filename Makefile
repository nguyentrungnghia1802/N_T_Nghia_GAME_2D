CXX ?= g++
CPPFLAGS ?=
CXXFLAGS ?=
CXXFLAGS += -Wall -Wextra -Wpedantic -std=c++17
LDFLAGS ?=
LDLIBS ?=

ifeq ($(OS),Windows_NT)
TARGET ?= main.exe
CPPFLAGS += -Isrc/include
LDFLAGS += -static-libgcc -static-libstdc++ -Lsrc/lib
LDLIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
else
TARGET ?= main
SDL_CFLAGS ?= $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer)
SDL_LIBS ?= $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer)
CPPFLAGS += $(SDL_CFLAGS)
LDLIBS += $(SDL_LIBS)
endif

SOURCES = src/main.cpp src/CommonFunc.cpp src/BaseObject.cpp src/gamemap.cpp src/MainObject.cpp src/ImpTimer.cpp src/BulletObject.cpp src/ThreatObject.cpp src/PlayHealth.cpp src/TextObject.cpp src/Profiler.cpp
OBJECTS = $(SOURCES:src/%.cpp=src/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

src/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
