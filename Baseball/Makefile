#OBJS =		src/main.cpp src/scene.cpp src/md2model.cpp src/vec3f.cpp src/imageloader.cpp src/stuff.c



OBJS = src/*.cpp
#OBJS = src/main.cpp src/scene.cpp src/bsptree.cpp src/Console.cpp src/Bitmap.cpp

CXXFLAGS = -O2 -ggdb -Wall -fmessage-length=0
LIBS =	-lGL -lGLU -lglut
INCS =  -I"/usr/include/GL/" -I"./headers/"
TARGET =	baseball



$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) $(INCS)


all:	$(TARGET)



clean:
	rm -f src/*.o baseball
