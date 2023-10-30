.PHONY: all clean

SOURCES = $(wildcard *.cpp)
SOURCES += $(wildcard internal/*.cpp)
SOURCES += $(wildcard internal/messages/*.cpp)
SOURCES += $(wildcard internal/connectivity/*.cpp)
SOURCES += $(wildcard ../common/*.cpp)
SOURCES += $(wildcard ../common/messages/*.cpp)
OUTPUT = ../../build/server.exe
OBJDIR = ../../build/obj/server
OBJECTS := $(SOURCES:%.cpp=$(OBJDIR)/%.o)
CXX = g++

CXXFLAGS = -std=gnu++2a -O3 -march=native -Wall -Wextra -Iinclude -flto=auto
LDLIBS = -lm

ifdef OS
	ifeq ($(LBITS),64)
		CPPFLAGS += -D'_WIN64='
		LDLIBS += -lws2_64 -lwsock64
	else
		CPPFLAGS += -D'_WIN32='
		LDLIBS += -lws2_32 -lwsock32
	endif
endif

default: all

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	@mkdir -p '$(@D)'
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@rm -rf $(OBJDIR)
