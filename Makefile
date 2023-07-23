# all: test

.PHONY: build
build:
	mkdir -p build && cd build \
		&& cmake -DCMAKE_BUILD_TYPE=Debug .. \
		&& make -j4

.PHONY: test
test: build
	./build/test

.PHONY: clean
clean:
	rm -rf build
