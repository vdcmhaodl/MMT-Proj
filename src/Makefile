CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS = -lgdi32 -lshcore -lws2_32 -lMswsock -lmfplat -lmf -lmfuuid -mfreadwrite
# -lstrmiids -lquartz -lole32 -luuid
STATIC_LDFLAGS = -static -static-libgcc -static-libstdc++
CURL-FLAGS = -I ../third_party/libcurl/include -L ../third_party/libcurl/lib -lcurl

SRCS = $(wildcard *.cpp ./app/*.cpp ./mail/*.cpp ./socket/*.cpp)
# SRCS = $(find RootFolderName -type f -iregex ".*\.cpp")

$(info SRCS=$(SRCS))

SRCS_MAIN_SERVER = $(filter-out main_client.cpp ./socket/client.cpp ./mail/Gmail.cpp ./app/UI.cpp,$(SRCS))
SRCS_MAIN_CLIENT = $(filter-out main_server.cpp ./socket/server.cpp,$(SRCS))

OBJS_MAIN_SERVER = $(SRCS_MAIN_SERVER:.cpp=.o)
OBJS_MAIN_CLIENT = $(SRCS_MAIN_CLIENT:.cpp=.o)

all: main_server.exe main_client.exe clean

main_server.exe: $(OBJS_MAIN_SERVER)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(STATIC_LDFLAGS)

main_client.exe: $(OBJS_MAIN_CLIENT)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(CURL-FLAGS)

# Object files compilation
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS) $(CURL-FLAGS) $(STATIC_LDFLAGS)

# Clean up build files except executables
clean:
	-rm -f *.o
	-rm -f ./app/*.o
	-rm -f ./mail/*.o
	-rm -f ./socket/*.o

full_clean:
	-rm -f *.o *.exe
	-rm -f ./app/*.o ./app/*.exe
	-rm -f ./mail/*.o ./mail/*.exe
	-rm -f ./socket/*.o ./socket/*.exe

.PHONY: all clean full_clean