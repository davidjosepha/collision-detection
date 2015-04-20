CC = g++
CCFLAGS =

ifeq ($(OS),Windows_NT)
	# Windows flags, don't yet include gsl
	CCFLAGS += -mwindows -lglut32 -lopengl32 -lglu32  -std=c++11
else
	UNAME := $(shell uname)

	ifeq ($(UNAME),Linux)
		CCFLAGS += -lGL -lGLU -lglut -lgsl -lgslcblas -lm -std=gnu++11
	endif

	# Mac flags, don't yet include gsl
	ifeq ($(UNAME),Darwin)
		CCFLAGS += -framework GLUT -framework OPENGL
	endif
endif

all:
	$(CC) main.cpp cuboid.cpp viewer.cpp collisionevent.cpp motionengine.cpp -o model $(CCFLAGS)
clean:
	rm *.o model
