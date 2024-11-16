# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -lgdi32 -lshcore -lws2_32 -lMswsock -static -static-libgcc -static-libstdc++

# Get all .cpp files in the current directory
SRCS = $(wildcard *.cpp)

# Filter out the main .cpp files
SRCS_MAIN_SERVER = $(filter-out A.cpp main.cpp client.cpp,$(SRCS))
SRCS_MAIN_CLIENT = $(filter-out B.cpp main.cpp server.cpp,$(SRCS))
SRCS_MAIN = $(filter-out A.cpp B.cpp,$(SRCS))

# Object files
OBJS_MAIN_SERVER = $(SRCS_MAIN_SERVER:.cpp=.o)
OBJS_MAIN_CLIENT = $(SRCS_MAIN_CLIENT:.cpp=.o)
OBJS_MAIN = $(SRCS_MAIN:.cpp=.o)

# Targets
all: main_server.exe main_client.exe main.exe clean

main_server.exe: $(OBJS_MAIN_SERVER)
	$(CXX) -o $@ $^ $(CXXFLAGS)

main_client.exe: $(OBJS_MAIN_CLIENT)
	$(CXX) -o $@ $^ $(CXXFLAGS)

main.exe: $(OBJS_MAIN)
	$(CXX) -o $@ $^ $(CXXFLAGS)

# Object files compilation
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Clean up build files except executables
clean:
	-rm -f *.o 
	-del /Q *.o

full_clean:
	-rm -f *.o *.exe
	-del /Q *.o *.exe

.PHONY: all clean