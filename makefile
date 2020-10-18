CXX=g++
CXXFLAGS=-Wall `pkg-config opencv --cflags`
LDLIBS= `pkg-config opencv --libs`
ALL:main.o BlokusRecognize.o findOptimalMove.o
	$(CXX) $(CXXFLAGS) -o main main.o -lm BlokusRecognize.o findOptimalMove.o $(LDLIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -o main.o -c main.cpp $(LDLIBS)

BlokusRecognize.o: BlokusRecognize.cpp BlokusRecognize.hpp
	$(CXX) $(CXXFLAGS) -o BlokusRecognize.o -c BlokusRecognize.cpp $(LDLIBS)

findOptimalMove.o: findOptimalMove.cpp findOptimalMove.hpp
	$(CXX) $(CXXFLAGS) -o findOptimalMove.o -c findOptimalMove.cpp
