BIN=mat_mul
CC=g++
CXXFLAGS =-O0 -Wno-write-strings -msse2 -mavx 

naive: mat_mul
	g++ mat_mul.c $(CXXFLAGS) -o $(BIN)/$@

tiling: mat_mul
	g++ mat_mul.c $(CXXFLAGS) -D OPTIMIZE_TILING -o $(BIN)/$@

loop: mat_mul
	g++ mat_mul.c $(CXXFLAGS) -D OPTIMIZE_LOOP_OPT -o $(BIN)/$@

simd: mat_mul
	g++ mat_mul.c $(CXXFLAGS) -D OPTIMIZE_SIMD -o $(BIN)/$@

combination: mat_mul
	g++ mat_mul.c $(CXXFLAGS) -D OPTIMIZE_COMBINED -o $(BIN)/$@

all: mat_mul
	g++ mat_mul.c $(CXXFLAGS) -D OPTIMIZE_TILING -D OPTIMIZE_SIMD -D OPTIMIZE_LOOP_OPT -D OPTIMIZE_COMBINED -o $(BIN)/$@

clean:
	@rm -rf $(BIN)
	@rm -f out.txt

mat_mul:
	@mkdir -p $(BIN)

simd_256: mat_mul
	g++ mat_mul.c -mavx $(CXXFLAGS) -D OPTIMIZE_SIMD_256 -o $(BIN)/$@

simd_512: mat_mul
	g++ mat_mul.c -mavx512f $(CXXFLAGS) -D OPTIMIZE_SIMD_512 -o $(BIN)/$@
