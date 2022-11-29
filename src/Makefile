#  SOURCE CONFIGURATION  #
TARGET=libcppsparkplug

BUILD=../build
BUILD_LIB=$(BUILD)/lib
BUILD_SRC=$(BUILD)/src
SRC_DIR=.
INCLUDE_DIR=../include
INSTALL_LIB_DIR=/usr/local/lib
INSTALL_INCLUDE_DIR=/usr/local/include

C_SOURCE=$(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
CXX_SOURCE=$(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)

C_OBJECTS=$(patsubst %, $(BUILD_SRC)/%, $(C_SOURCE:.c=.o))
CXX_OBJECTS=$(patsubst %, $(BUILD_SRC)/%, $(CXX_SOURCE:.cpp=.o))
OBJECTS_DEP=$(CXX_OBJECTS) $(C_OBJECTS)


#  COMPILER FLAGS  #
CPP=g++
DEBUG=-g
OPT=-O3
WARN=-Wall
PTHREAD=-pthread
CPPFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -std=c++11

#  LINKER FLAGS  #
LD=g++
LFLAGS=-I${INCLUDE_DIR} -fPIC
LDFLAGS=$(PTHREAD) -lpaho-mqtt3as -ltahu

#  UTILITY  #
MKDIR_P = mkdir -p

.PHONY: all clean install

all: ${BUILD_SRC} $(C_OBJECTS) $(CXX_OBJECTS)
	${AR} ${ARFLAGS} $(BUILD_LIB)/${TARGET}.a $(OBJECTS_DEP)
	$(LD) -shared -o $(BUILD_LIB)/$(TARGET).so $(OBJECTS_DEP) $(LDFLAGS) $(LFLAGS)

install:
	${MKDIR_P} ${INSTALL_LIB_DIR}
	${MKDIR_P} ${INSTALL_INCLUDE_DIR}
	cp $(BUILD_LIB)/* $(INSTALL_LIB_DIR)/
	cp $(INCLUDE_DIR)/*.h $(INSTALL_INCLUDE_DIR)/
	find $(INCLUDE_DIR)/ -type d | grep -oP "$(INCLUDE_DIR)/\K.*" | xargs -i mkdir -p $(INSTALL_INCLUDE_DIR)/{}
	find $(INCLUDE_DIR)/ -type f | grep -i h$ | grep -oP "$(INCLUDE_DIR)/\K.*" | xargs -i cp $(INCLUDE_DIR)/{} $(INSTALL_INCLUDE_DIR)/{}

${BUILD_SRC}:
	find $(SRC_DIR)/ -type d | grep -oP "$(SRC_DIR)/\K.*" | xargs -i mkdir -p $(BUILD_SRC)/{}
	${MKDIR_P} ${BUILD_LIB}

$(C_OBJECTS): $(BUILD_SRC)/%.o : $(SRC_DIR)/%.c
	$(CPP) -c $< $(CPPFLAGS) $(LFLAGS) -o $@

$(CXX_OBJECTS): $(BUILD_SRC)/%.o : $(SRC_DIR)/%.cpp
	$(CPP) -c $< $(CPPFLAGS) $(LFLAGS) -o $@

clean:
	rm -rf ${BUILD_SRC} ${BUILD_LIB}

