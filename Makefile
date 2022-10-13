SRC_DIR   = src
BIN_DIR   = bin
SOURCES   = $(wildcard $(SRC_DIR)/*.cpp)
BINARIES  = $(patsubst $(SRC_DIR)/%,$(BIN_DIR)/%,$(SOURCES:.cpp=))
BREW_PREFIX = $(shell brew --prefix)
DEPS      = folly boost glog gflags fmt double-conversion libevent
INCLUDES  = $(DEPS:%=-I$(BREW_PREFIX)/opt/%/include)
LIBS      = -stdlib=libc++ -L$(BREW_PREFIX)/lib \
            -lc++abi \
            -lpthread \
            -lm \
            -ldl \
            -lfolly \
            -lglog \
            -lgflags \
            -lfmt \
            -ldouble-conversion \
            -levent
CXX       = clang++
LD        = clang++
CXXFLAGS  = -std=c++17 \
            -stdlib=libc++ \
            -fcoroutines-ts \
            -pedantic \
            -pedantic-errors \
            -Wall \
            -Wdisabled-optimization \
            -Weffc++ \
            -Werror \
            -Wextra \
            -Wno-reserved-user-defined-literal

.PHONY: all clean

all: $(BINARIES)

run: all
	for prog in $(BINARIES); do \
  echo "#################### $$prog ####################: running"; \
  $$prog && \
  echo "#################### $$prog ####################: OK" || \
  exit; \
  done

debug:
	$(info $$SOURCES is [${SOURCES}])
	$(info $$BINARIES is [${BINARIES}])
	$(info $$DEPS is [${DEPS}])
	$(info $$INCLUDES is [${INCLUDES}])

%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR)/%: %.o
	$(LD) $(LIBS) -o $@ $^

deps:
	brew list double-conversion &> /dev/null || brew install double-conversion
	brew list folly &> /dev/null || brew install folly

clean:
	rm -f $(BINARIES)
