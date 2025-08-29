BIN=emb
CC=g++

CXXFLAGS = -Wno-write-strings -msse2 -mavx  -mavx512f -O2

ifeq ($(NAIVE),0)
    CXXFLAGS += -D NOT_NAIVE
endif

ifeq ($(DEBUG),1)
    CXXFLAGS += -D DEBUG
endif

emb:
	g++ emb.cpp $(CXXFLAGS) -o emb