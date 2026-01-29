LLVM_CONFIG = /opt/homebrew/opt/llvm/bin/llvm-config
CC = /opt/homebrew/opt/llvm/bin/clang

CFLAGS = -g -Iinclude $(shell $(LLVM_CONFIG) --cflags)

LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags --libs)

SRC_DIR = src
OBJ_DIR = build
TARGET = my_jit_compiler

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)
	@echo "Build Complete! Run: ./$(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Cleaned build files."