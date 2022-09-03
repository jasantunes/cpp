# https://stackoverflow.com/questions/9178285/how-can-makefile-use-separate-directories-for-source-code-and-binaries
CC          = g++
LD          = g++
CFLAG       = -std=c++17 \
-pedantic  \
-pedantic-errors \
-Waggregate-return \
-Wall \
-Wcast-qual \
-Wconversion \
-Wdisabled-optimization \
-Weffc++ \
-Werror \
-Wextra \
-Wfloat-equal \
-Wformat-nonliteral
-Wformat-security  \
-Wformat-y2k \
-Wformat=2 \
-Wimplicit \
-Wimport  \
-Winit-self  \
-Winline \
-Winvalid-pch   \
-Wlong-long \
-Wmissing-field-initializers \
-Wmissing-format-attribute   \
-Wmissing-include-dirs \
-Wmissing-noreturn \
-Wpointer-arith \
-Wredundant-decls \
-Wshadow \
-Wstack-protector \
-Wstrict-aliasing=2 \
-Wswitch-default \ \
-Wswitch-enum \
-Wunreachable-code \
-Wunused \
-Wunused-parameter \
-Wvariadic-macros \
-Wwrite-strings
PROG_NAME   = hello

SRC_DIR     = ./src
BUILD_DIR   = ./build
BIN_DIR     = ./bin
SRC_LIST = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_LIST = $(BUILD_DIR)/$(notdir $(SRC_LIST:.cpp=.o))

.PHONY: all clean $(PROG_NAME) compile

all: $(PROG_NAME)

compile:
	$(CC) -c $(CFLAG) $(SRC_LIST) -o $(OBJ_LIST)

$(PROG_NAME): compile
	$(LD) $(OBJ_LIST) -o $(BIN_DIR)/$@

clean:
	rm -f $(BIN_DIR)/$(PROG_NAME) $(BUILD_DIR)/*.o
