.PHONY: all
.PHONY: build
.PHONY: test
.PHONY: clean

all: test

init:
	paru -S lcov gcc cmake nlohmann-json boost perl-datetime

build:
	mkdir -p build && cd build \
		&& cmake -DCMAKE_BUILD_TYPE=Debug .. \
		&& make -j4

test: build
	./build/tests

cov: build test
	mkdir -p coverage \
		&& lcov \
			--directory build  \
			--capture  \
			--output-file coverage/coverage.info \
			--ignore-errors mismatch \
			--include "*/json_ext/*" \
			--exclude "*/json_ext/tests" \
			--exclude "gtest/" \
		&& genhtml coverage/coverage.info --output-directory coverage

clean:
	rm -rf build coverage
