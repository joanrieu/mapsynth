CXX = clang++
CXXFLAGS = -std=c++11

all: mod3 test

mod3: mod3.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: mod3
	./mod3 | play -t f32 -r 48k -c 1 - &>/dev/null

clean:
	rm -rf mod3

.PHONY: all test clean
