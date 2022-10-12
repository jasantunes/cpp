SRC_DIR   = src
BIN_DIR   = bin
SOURCES   = $(wildcard $(SRC_DIR)/*.cpp)
BINARIES  = $(patsubst $(SRC_DIR)/%,$(BIN_DIR)/%,$(SOURCES:.cpp=))
BREW_PREFIX = $(shell brew --prefix)
DEPS      = folly boost glog gflags fmt
INCLUDES  = $(DEPS:%=-I$(BREW_PREFIX)/opt/%/include)
LIBS      = -L$(BREW_PREFIX)/lib -lfolly -lfmt
CXX       = clang++
LD        = clang++
CXXFLAGS  = -std=c++17 \
            -stdlib=libc++ \
            -pedantic \
            -pedantic-errors \
            -Wall \
            -Wdisabled-optimization \
            -Weffc++ \
            -Werror \
            -Wextra

.PHONY: all clean

all: $(BINARIES)

debug:
	$(info $$SOURCES is [${SOURCES}])
	$(info $$BINARIES is [${BINARIES}])
	$(info $$DEPS is [${DEPS}])
	$(info $$INCLUDES is [${INCLUDES}])

%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR)/%: %.o
	$(LD) $(LIBS) -o $@ $^

deps: folly

folly:
	brew list folly &> /dev/null || brew install folly

clean:
	rm -f $(BINARIES)
