TARGET=Sparkplug
OUT_DIR=../build/src
SRC_DIR=.
INCLUDE_DIR=../include
INCLUDE_LIB_DIR=../lib
INSTALL_DIR=/usr/bin

PKGCONFIG = $(shell which pkg-config)

# compiler
CC=g++
# debug
DEBUG=-g
# optimisation
OPT=-O3
# warnings
WARN=-Wall

PTHREAD=-pthread

C_OBJECTS=$(patsubst ${SRC_DIR}/%.c, $(OUT_DIR)/%.o, $(wildcard ${SRC_DIR}/*.c))
CPP_OBJECTS=$(patsubst ${SRC_DIR}/%.cpp, $(OUT_DIR)/%.o, $(wildcard ${SRC_DIR}/*.cpp))

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe -std=c++0x

LD=g++
LFLAGS=-I${INCLUDE_DIR} -I${INCLUDE_LIB_DIR}
LDFLAGS=$(PTHREAD) -lpaho-mqtt3as -L${INCLUDE_LIB_DIR} -ltahu 

#    paho-mqtt3a - asynchronous (MQTTAsync)
#    paho-mqtt3as - asynchronous with SSL/TLS (MQTTAsync)
#    paho-mqtt3c - "classic" / synchronous (MQTTClient)
#    paho-mqtt3cs - "classic" / synchronous with SSL/TLS (MQTTClient)

MKDIR_P = mkdir -p

all: ${OUT_DIR} $(C_OBJECTS) $(CPP_OBJECTS)
	$(LD) -o $(OUT_DIR)/$(TARGET) $(C_OBJECTS) $(CPP_OBJECTS) $(LDFLAGS) $(LFLAGS)

install:
	cp $(OUT_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

$(C_OBJECTS): ${OUT_DIR}/%.o : ${SRC_DIR}/%.c
	$(CC) -c $< $(CCFLAGS) $(LFLAGS) $(LDFLAGS) -o $@

$(CPP_OBJECTS): ${OUT_DIR}/%.o : ${SRC_DIR}/%.cpp
	$(CC) -c $< $(CCFLAGS) $(LFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f ${OUT_DIR}/*.o ${OUT_DIR}/*.c ${OUT_DIR}/*.cpp $(TARGET)
