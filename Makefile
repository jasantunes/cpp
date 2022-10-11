SRC_DIR   = ./src
BIN_DIR   = ./bin
SOURCES   = $(wildcard $(SRC_DIR)/*.cpp)
BINARIES  = $(patsubst $(SRC_DIR)/%,$(BIN_DIR)/%,$(SOURCES:.cpp=))
INCLUDES  =
LIBS      =
CXX       = g++
LD        = g++
CXXFLAGS  = -std=c++17 \
            -pedantic \
            -pedantic-errors \
            -Wall \
            -Wdisabled-optimization \
            -Weffc++ \
            -Werror \
            -Wextra

.PHONY: all clean

all: $(BINARIES)

%.o: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR)/%: %.o
	$(LD) $(LIBS) -o $@ $^

clean:
	rm -f $(BINARIES)
