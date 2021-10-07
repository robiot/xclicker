
BIN_DIR   = /usr/bin
BUILD_DIR = build
BINNAME   = xclicker
TARGET    = ${BIN_DIR}/${BINNAME}
DESKTOP   = /usr/share/applications
DESKFILE  = xclicker.desktop

.PHONY: build
build:
	meson build
	meson compile -C build
	mkdir -p ./${BIN_DIR}
	sudo cp -f ./${BUILD_DIR}/src/${BINNAME} ${TARGET}
	sudo cp -f ./${BUILD_DIR}/src/${DESKFILE} ${DESKTOP}
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
