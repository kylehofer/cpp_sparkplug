#  SOURCE CONFIGURATION  #
TARGET=SparkplugTests

BUILD=../build
BUILD_LIB=$(BUILD)/lib
BUILD_TESTS=$(BUILD)/tests
SRC_DIR=../src
TEST_DIR=.
INCLUDE_DIR=../include
TEST_INCLUDE_DIR=.

C_SOURCE=$(wildcard $(TEST_DIR)/*.c) $(wildcard $(TEST_DIR)/*/*.c)
CXX_SOURCE=$(wildcard $(TEST_DIR)/*.cpp) $(wildcard $(TEST_DIR)/*/*.cpp)

C_OBJECTS=$(patsubst %, $(BUILD_TESTS)/%, $(C_SOURCE:.c=.o))
CXX_OBJECTS=$(patsubst %, $(BUILD_TESTS)/%, $(CXX_SOURCE:.cpp=.o))
OBJECTS_DEP=$(CXX_OBJECTS) $(C_OBJECTS)

#  COMPILER FLAGS  #
CPP=g++
DEBUG=-g
OPT=-O0
WARN=-Wall
PTHREAD=-pthread
CPPFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -std=c++11

#  LINKER FLAGS  #  -fPIC
LD=g++
LFLAGS=-I${INCLUDE_DIR} -I${TEST_INCLUDE_DIR}
LDFLAGS=$(PTHREAD) -lpaho-mqtt3as -L${BUILD_LIB} -ltahu -lgtest -lgmock -lmosquitto -lcppsparkplug

#  UTILITY  #
MKDIR_P = mkdir -p


.PHONY: all src clean valgrind

all: src ${BUILD_TESTS} $(C_OBJECTS) $(CXX_OBJECTS)
	$(LD) -o $(BUILD_TESTS)/$(TARGET) $(OBJECTS_DEP) $(LDFLAGS) $(LFLAGS) 

valgrind: all
	valgrind --leak-check=yes -s $(BUILD_TESTS)/$(TARGET)

run: all
	$(BUILD_TESTS)/$(TARGET)

${BUILD_TESTS}:
	${MKDIR_P} ${BUILD_TESTS}

$(C_OBJECTS): $(BUILD_TESTS)/%.o : $(TEST_DIR)/%.c
	$(CPP) -c $< $(CPPFLAGS) $(LFLAGS) -o $@

$(CXX_OBJECTS): $(BUILD_TESTS)/%.o : $(TEST_DIR)/%.cpp
	$(CPP) -c $< $(CPPFLAGS) $(LFLAGS) -o $@

clean:
	rm -rf ${TEST_DIR}/*.o $(BUILD_TESTS)

src:
	$(MAKE) -C ../src

