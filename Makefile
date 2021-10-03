
COMPILER       = clang
COMPILER_FLAGS = -Wno-unused-command-line-argument
PKG_CONFIG     = `pkg-config --cflags --libs gtk4`

OBJ_DIR   = obj
SRC_DIR   = src
BIN_DIR   = bin

TARGET    = ${BIN_DIR}/xclicker
SRC       = $(wildcard ${SRC_DIR}/*.c)
OBJ       = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: build
build: ${TARGET}

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(COMPILER) $^ -o $@ ${PKG_CONFIG} ${COMPILER_FLAGS}

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(COMPILER) -c $< -o $@ ${PKG_CONFIG} ${COMPILER_FLAGS}

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

.PHONY: run
run:
	${TARGET}

.PHONY: all
all:
	make build
	make run

.PHONY: clean
clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

#clang src/XClicker.c -o xclicker `pkg-config --cflags --libs gtk4`
#gcc src/xclicker.c -o xclicker -lX11
