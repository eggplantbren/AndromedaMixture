CXX = g++
FLAGS = -std=c++17
INCLUDE = -I $(DNEST4_PATH)
OPTIM = -O3 -march=native
WARN = -Wall -Wextra -pedantic
ALL = $(FLAGS) $(INCLUDE) $(OPTIM) $(WARN)

default:
	$(CXX) $(ALL) -c main.cpp
	$(CXX) -pthread -L$(DNEST4_PATH) -o main main.o -lpthread -dnest4
	rm -f main.o


tidy:
	clang-tidy main.cpp -- $(INCLUDE)
