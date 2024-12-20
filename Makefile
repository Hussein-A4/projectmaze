
CC := g++

INCLUDE_PATHS = ./SDL2-2.30.9/include
LIBRARY_PATHS = ./SDL2-2.30.9/lib

#OBJS specifies which files to compile as part of the project
OBJS = maze.cpp

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = maze

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
