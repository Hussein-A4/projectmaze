
CC := g++
CXXFLAGS := -std=c++17 -Wall -I./SDL/install/include
LDFLAGS := -L./SDL/install/lib -lSDL2

#OBJS specifies which files to compile as part of the project
OBJS = maze.cpp

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = maze

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
