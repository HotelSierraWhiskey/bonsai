DIR := $(dir $(abspath $(dir $$PWD)))
CPP = g++
CPPFLAGS = -I$(DIR) -fpermissive -w

SOURCES := $(wildcard *.cpp src/*.cpp tests/*/*.cpp)

ifeq (test,$(firstword $(MAKECMDGOALS)))  
  TEST_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))  
  $(eval $(TEST_ARGS):;@:)
endif

compile:
	$(CPP) $(CPPFLAGS) $(SOURCES)

test:
	python3 template.py $(TEST_ARGS)