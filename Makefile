CXX      ?= g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wpedantic -Werror -O0

all: test_filters

test_filters: test_filters.cpp filters.hpp
	$(CXX) $(CXXFLAGS) -o $@ test_filters.cpp

test: test_filters
	./test_filters

coverage: clean
	$(CXX) $(CXXFLAGS) --coverage -o test_filters test_filters.cpp
	./test_filters
	gcov -b test_filters.gcda | grep -A4 "filters.hpp'"

clean:
	rm -f test_filters *.gcda *.gcno *.gcov
