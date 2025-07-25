TARGET_EXEC := Mocha

B_DIR := build
S_DIR := src
I_DIR := src src/libs
I_DIR := $(addprefix -I, $(I_DIR))

L_DIR := src/libs
L_DIR := $(addprefix -L, $(L_DIR))

SRC := $(wildcard $(S_DIR)/**/*.cpp)
SRC += $(wildcard $(S_DIR)/*.cpp)

OBJS := $(patsubst $(S_DIR)/%,$(B_DIR)/%,$(SRC))
OBJS := $(patsubst %.cpp,%.o,$(OBJS))

CXX := g++
CXX_FLAGS := -std=c++20

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
LIBS := -lGL -ldl -lglfw
endif
ifeq ($(UNAME), MSYS_NT-10.0-26100)
LIBS := -libglfw3.a -lgdi32 -lopengl32 -llua54.a
endif

all: $(TARGET_EXEC)

$(TARGET_EXEC) : $(OBJS)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(L_DIR) $(LIBS)

# Get .cpp -> .o for src/common
$(B_DIR)/%.o: $(S_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(I_DIR) -c $< -o $@

# Get .cpp -> .o for src/common
$(B_DIR)/%/%.o: $(S_DIR)/%/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(I_DIR) -c $< -o $@

# Get .cpp -> .o for src/libs
$(B_DIR)/%/%/%.o: $(S_DIR)/%/%/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(I_DIR) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(B_DIR)/*

.PHONY: run
run: $(TARGET_EXEC)
	./$(TARGET_EXEC)