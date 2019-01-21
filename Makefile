CXX = g++
FLAGS = -std=c++17
INCLUDE = -I .
OPTIM = -O2 -g
WARN = -Wall -Wextra -pedantic
ALL = $(FLAGS) $(OPTIM) $(WARN)

default:
	$(CXX) $(ALL) -c main.cpp
	$(CXX) -o main main.o
	rm -f main.o


tidy:
	clang-tidy main.cpp -- $(INCLUDE)
