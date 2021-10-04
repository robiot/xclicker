
BIN_DIR   = bin
BUILD_DIR = build
BINNAME   = xclicker
TARGET    = ${BIN_DIR}/${BINNAME}

.PHONY: build
build:
	meson build
	meson compile -C build
	mkdir -p ./${BIN_DIR}
	cp -rf ./${BUILD_DIR}/src/${BINNAME} ${TARGET}

.PHONY: all
all:
	make build
	make run

.PHONY: run
run:
	./${TARGET}

.PHONY: clean
clean:
	@$(RM) -rv $(BIN_DIR) ${BUILD_DIR}