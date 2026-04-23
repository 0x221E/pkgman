CC = gcc

TARGET_EXEC = ./pkgman
BUILD_DIR = ./build
SRC_DIR = ./src
INCLUDE_DIR = ./include

SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES = $(SRC_FILES:%=$(BUILD_DIR)/%.o)

CFLAGS = -lcurl -I./include

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(CFLAGS)
	@echo "[CC] Compiled $< to $@"

debug: $(OBJ_FILES)
	@$(CC) $(OBJ_FILES) -o $(TARGET_EXEC) -g $(CFLAGS) -DDEBUG -fsanitize=address
	@echo "[DONE] Finished compiling $(TARGET_EXEC) in debug build."

debug-nosan: $(OBJ_FILES)
	@$(CC) $(OBJ_FILES) -o $(TARGET_EXEC) -g -DDEBUG $(CFLAGS)
	@echo "[DONE] Finished compiling $(TARGET_EXEC) in debug build."

prod: $(OBJ_FILES)
	@$(CC) $(OBJ_FILES) -o $(TARGET_EXEC) $(CFLAGS)
	@echo "[DONE] Finished compiling $(TARGET_EXEC) in production build."

install: prod
	@cp ./pkgman /usr/bin/
	@echo "Installed application to /usr/bin"

clean:
	@rm -rf build/
	@rm pkgman
	@echo "[CLEAN] Cleaned up the project."
