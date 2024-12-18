CXX = g++

CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic
LDFLAGS = -lgdi32 -lshcore -lws2_32 -liphlpapi -lMswsock -lmfplat -lmf -lmfuuid -lmfreadwrite -lstrmiids -lquartz -lole32 -luuid -lshell32 -lwinmm -lcomctl32 # -mwindows
STATIC_LDFLAGS = -static -static-libgcc -static-libstdc++
CURL_FLAGS = -I ../third_party/libcurl/include -L ../third_party/libcurl/lib -lcurl

SRC_DIR = .

# SRCS := $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/app/*.cpp $(SRC_DIR)/app/services/*.cpp ./mail/*.cpp ./socket/*.cpp)
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")

SRCS_MAIN_SERVER = $(filter-out $(SRC_DIR)/build_client_GUI/%.cpp $(SRC_DIR)/build_client_terminal/%.cpp $(SRC_DIR)/socket/client.cpp $(SRC_DIR)/mail/Gmail.cpp,$(SRCS))
SRCS_MAIN_CLIENT_TERMINAL = $(filter-out $(SRC_DIR)/build_server/%.cpp $(SRC_DIR)/build_client_GUI/%.cpp $(SRC_DIR)/socket/server.cpp,$(SRCS))
SRCS_MAIN_CLIENT_GUI = $(filter-out $(SRC_DIR)/build_server/%.cpp $(SRC_DIR)/build_client_terminal/%.cpp $(SRC_DIR)/socket/server.cpp,$(SRCS))

output_file:
	$(info SRCS: $(SRCS))
	$(info SERVER: $(SRCS_MAIN_SERVER))
	$(info CLIENT - GUI: $(SRCS_MAIN_CLIENT_GUI))
	$(info CLIENT - TERMINAL: $(SRCS_MAIN_CLIENT_TERMINAL))

OBJS_MAIN_SERVER = $(SRCS_MAIN_SERVER:.cpp=.o)
OBJS_MAIN_CLIENT_GUI = $(SRCS_MAIN_CLIENT_GUI:.cpp=.o)
OBJS_MAIN_CLIENT_TERMINAL = $(SRCS_MAIN_CLIENT_TERMINAL:.cpp=.o)

help:
	$(info Uncomment at line 10 to run Makefile in Windows environment)
	$(info Uncomment at line 11 to run Makefile in Unix environment (kinda))
	$(info make: Build both the server and the client application (GUI version only))
	$(info make help: Get help)
	$(info make main_server.exe: Build the server application side)
	$(info make main_GUI.exe: Build the client application side (GUI version))
	$(info make main_terminal.exe: Build the client application side (TUI version))

all: main_server.exe main_GUI.exe # main_terminal.exe clean

main_server.exe: $(OBJS_MAIN_SERVER)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(STATIC_LDFLAGS)

main_GUI.exe: $(OBJS_MAIN_CLIENT_GUI)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(CURL_FLAGS)

main_terminal.exe: $(OBJS_MAIN_CLIENT_TERMINAL)
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