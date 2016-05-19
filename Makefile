INC=include
BIN=bin
OBJ=obj
SRC=src
CXX=g++

CFLAGS=-O2 -msse4 -msse -mmmx -mfpmath=sse -g -ggdb -Wall -I${INC}

LIBS=-lm `pkg-config opencv --libs` -lboost_system -lstdc++

all: RealTimeCounting PeopleTracking

RealTimeCounting: RealTimeCounting.o
	${CXX} ${CFLAGS} -o ${BIN}/RealTimeCounting ${OBJ}/RealTimeCounting.o ${LIBS}

PeopleTracking: PeopleTracking.o
	${CXX} ${CFLAGS} -o ${BIN}/PeopleTracking ${OBJ}/peopletracking.o ${LIBS}



RealTimeCounting.o: ${SRC}/demos/RealTimeCounting.cpp
	${CXX} ${CFLAGS} -c ${SRC}/demos/RealTimeCounting.cpp -o ${OBJ}/RealTimeCounting.o

PeopleTracking.o: ${SRC}/demos/peopletracking.cpp
	${CXX} ${CFLAGS} -c ${SRC}/demos/peopletracking.cpp -o ${OBJ}/peopletracking.o

GaborFilters.o: ${SRC}/filters/GaborFilters.cpp
	${CXX} ${CFLAGS} -c ${SRC}/filters/GaborFilters.cpp -o ${OBJ}/GaborFilters.o

BinaryCV1Faces.o: ${SRC}/features/BinaryCV1Faces.cpp
	${CXX} ${CFLAGS} -c ${SRC}/features/BinaryCV1Faces.cpp -o ${OBJ}/BinaryCV1Faces.o

clean:
	rm -f *.o
	rm -f RealTimeCounting
	rm -f PeopleTracking
