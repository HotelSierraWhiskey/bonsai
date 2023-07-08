DIR := $(dir $(abspath $(dir $$PWD)))
CPP = g++
CPPFLAGS = -I$(DIR) -fpermissive -w

SOURCES := $(wildcard *.cpp src/*.cpp tests/*/*.cpp)

compile:
	$(CPP) $(CPPFLAGS) $(SOURCES)

