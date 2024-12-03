CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS = -lgdi32 -lshcore -lws2_32 -lMswsock -lmfplat -lmf -lmfuuid -mfreadwrite -lstrmiids -lquartz -lole32 -luuid -lshell32 -mwindows -lwinmm
STATIC_LDFLAGS = -static -static-libgcc -static-libstdc++
CURL_FLAGS = -I ../third_party/libcurl/include -L ../third_party/libcurl/lib -lcurl

SRC_DIR = .

# SRCS := $(wildcard $(SRC_DIR)/*.cpp ./app/*.cpp ./mail/*.cpp ./socket/*.cpp)
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")

SRCS_MAIN_SERVER = $(filter-out $(SRC_DIR)/build_client/%.cpp $(SRC_DIR)/socket/client.cpp $(SRC_DIR)/mail/Gmail.cpp $(SRC_DIR)/app/UI.cpp $(SRC_DIR)/app/window/%.cpp,$(SRCS))
SRCS_MAIN_CLIENT = $(filter-out $(SRC_DIR)/build_server/%.cpp $(SRC_DIR)/socket/server.cpp,$(SRCS))

OBJS_MAIN_SERVER = $(SRCS_MAIN_SERVER:.cpp=.o)
OBJS_MAIN_CLIENT = $(SRCS_MAIN_CLIENT:.cpp=.o)

all: main_server.exe main_client.exe clean

main_server.exe: $(OBJS_MAIN_SERVER)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(STATIC_LDFLAGS)

main_client.exe: $(OBJS_MAIN_CLIENT)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(CURL_FLAGS)

# Object files compilation
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS) $(CURL_FLAGS) $(STATIC_LDFLAGS)

clean:
	-find . -name "*.o" -exec rm -f {} +
	-del /s /q *.o

full_clean:
	-find . -name "*.o" -exec rm -f {} +
	-find . -name "*.exe" -exec rm -f {} +
	-del /s /q *.o
	-del /s /q *.exe

.PHONY: all clean full_clean