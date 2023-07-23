.PHONY: all
.PHONY: build
.PHONY: test
.PHONY: clean

all: test

build:
	mkdir -p build && cd build \
		&& cmake -DCMAKE_BUILD_TYPE=Debug .. \
		&& make -j4

test: build
	./build/tests

clean:
	rm -rf build
