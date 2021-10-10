
BIN_DIR   = bin
BUILD_DIR = build
BINNAME   = xclicker
TARGET    = ${BIN_DIR}/${BINNAME}
DESKFILE  = xclicker.desktop

.PHONY: build
build:
	@if test -d "./${BUILD_DIR}"; then echo "Build dir is already made"; else meson build; fi
	meson compile -C build
	mkdir -p ./${BIN_DIR}
	cp -f ./${BUILD_DIR}/src/${BINNAME} ${TARGET}
	
.PHONY: all
all:
	make build
	make run

.PHONY: run
run:
	./${TARGET}

.PHONY: install
install:
	make build
	sudo cp -f ./${BUILD_DIR}/src/${BINNAME} /usr/bin/${BINNAME} 
	sudo cp -f ./${DESKFILE} /usr/share/applications
	
.PHONY: clean
clean:
	@$(RM) -rv $(BIN_DIR) ${BUILD_DIR}
