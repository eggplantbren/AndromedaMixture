CXX = g++
FLAGS = -std=c++17
INCLUDE = -I . -I ./DNest4/code
OPTIM = -O3 -march=native
WARN = -Wall -Wextra -pedantic
ALL = $(FLAGS) $(INCLUDE) $(OPTIM) $(WARN)

default:
	make -C DNest4/code noexamples
	$(CXX) $(ALL) -c main.cpp
	$(CXX) -pthread -L ./DNest4/code -o main main.o -lpthread -ldnest4
	rm -f main.o

tidy:
	clang-tidy main.cpp -- $(INCLUDE)
