LIBS = -lpthread

TARGET = ktruss.out

FLAGS = --std=c++0x -Wall -DNDEBUG -g

BASE_SOURCES = $(wildcard */*.cpp) $(wildcard *.cpp)
BASE_OBJECTS = ${BASE_SOURCES:.cpp=.o}

OBJECTS = ${BASE_OBJECTS}

all: ${TARGET}

${TARGET}: ${OBJECTS}
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -o ${TARGET} ${OBJECTS} ${LIBS}

clean:
	@$(RM) */*.o
	@$(RM) *.o
	@$(RM) $(TARGET)

.cpp.o:
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all clean
