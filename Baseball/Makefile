
OBJS = src/*.cpp 
#game/*.cpp

CXXFLAGS = -g3 
# These are more flags to use later when releasing: -O2 -Wall -fmessage-length=0

LIBS = -lglu32 -lfreeglut -lopengl32 -Wl,--subsystem,windows
#-lpthread
INCS =  -I"C:\MinGW\include\GL" -I"./headers/" -I"./game/"
TARGET =	baseball.exe

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(INCS)

all:	$(TARGET)

clean:
	rm -f src/*.o baseball.exe
