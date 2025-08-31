BIN=emb
CC=g++

CXXFLAGS = -Wno-write-strings -O2

ifeq ($(NAIVE),1)
    CXXFLAGS += -D NAIVE
endif

emb:
	g++ emb.cpp $(CXXFLAGS) -o emb
	
emb_naive:
	g++ emb.cpp $(CXXFLAGS) -o emb_naive -D NAIVE

emb_128:
	g++ emb.cpp $(CXXFLAGS) -o emb_128 -D SIMD_128 

emb_256:
	g++ emb.cpp $(CXXFLAGS) -o emb_256 -D SIMD_256 -mavx 

emb_512:
	g++ emb.cpp $(CXXFLAGS) -o emb_512 -D SIMD_512 -mavx512f

emb_prefetch_simd_512:
	g++ emb.cpp $(CXXFLAGS) -o emb_prefetch_simd_512 -D prefetch_simd_512 -mavx512f

	