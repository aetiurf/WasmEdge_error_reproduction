MAKE = make

all: reproduction.cpp wat2so
	clang++ reproduction.cpp -lwasmedge -pthread -o reproduction -g

cpp2so: func.cpp
	clang++ func.cpp --target=wasm32 -emit-llvm -c -S
	llc -march=wasm32 -filetype=obj func.ll
	wasm-ld --no-entry --export-all -o func.wasm func.o
	rm -rf func.ll func.o
	wasm2wat func.wasm > func_test.wat

wat2so: func.wat
	wat2wasm func.wat --enable-all
	wasmedgec --enable-all func.wasm func.so

clean:
	rm -rf reproduction *.wasm *.so