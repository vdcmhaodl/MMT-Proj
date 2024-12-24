CXX = g++

CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic
LDFLAGS = -lgdi32 -lshcore -lws2_32 -liphlpapi -lMswsock -lmfplat -lmf -lmfuuid -lmfreadwrite -lstrmiids -lquartz -lole32 -luuid -lshell32 -lwinmm -lcomctl32 -lgdiplus # -mwindows
STATIC_LDFLAGS = -static -static-libgcc -static-libstdc++
CURL_FLAGS = -I ../third_party/libcurl/include -L ../third_party/libcurl/lib -lcurl

SRC_DIR = .

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
SRCS_MAIN_SERVER = $(filter-out $(SRC_DIR)/build_client_GUI/%.cpp $(SRC_DIR)/socket/client.cpp $(SRC_DIR)/mail/Gmail.cpp,$(SRCS))
SRCS_MAIN_CLIENT = $(filter-out $(SRC_DIR)/build_server/%.cpp $(SRC_DIR)/socket/server.cpp,$(SRCS))

output_file:
	$(info SRCS: $(SRCS))
	$(info SERVER: $(SRCS_MAIN_SERVER))
	$(info CLIENT - GUI: $(SRCS_MAIN_CLIENT))

OBJS_MAIN_SERVER = $(SRCS_MAIN_SERVER:.cpp=.o)
OBJS_MAIN_CLIENT = $(SRCS_MAIN_CLIENT:.cpp=.o)

all: ServerApp/main_server.exe ClientApp/main_GUI.exe # clean

ServerApp/main_server.exe: $(OBJS_MAIN_SERVER)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(STATIC_LDFLAGS)

ClientApp/main_GUI.exe: $(OBJS_MAIN_CLIENT)
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

.PHONY: all clean full_clean help