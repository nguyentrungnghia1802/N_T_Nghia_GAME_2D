CXX = g++
CXXFLAGS = -Wall -std=c++17 -Isrc/include -static-libgcc -static-libstdc++
LDFLAGS = -Lsrc/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
TARGET = main.exe
SOURCES = src/main.cpp src/CommonFunc.cpp src/BaseObject.cpp src/gamemap.cpp src/MainObject.cpp src/ImpTimer.cpp src/BulletObject.cpp src/ThreatObject.cpp src/PlayHealth.cpp src/TextObject.cpp
OBJECTS = $(SOURCES:src/%.cpp=src/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
