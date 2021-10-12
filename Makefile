BUILD_DIR = build
PKG_DIR   = pkg

BINNAME   = xclicker
TARGET    = build/src/${BINNAME}
DESKFILE  = xclicker.desktop

debpkgdir="./${PKG_DIR}/deb/package"

.PHONY: build
build:
	@if test -d "./${BUILD_DIR}"; then echo "Build dir is already made"; else meson build; fi
	meson compile -C build

.PHONY: run
run:
	./${TARGET}
	
.PHONY: all
all: build run

.PHONY: install
install: build
	@sudo install -Dm755 ./${BUILD_DIR}/src/${BINNAME} /usr/bin/${BINNAME}
	@sudo install -Dm755 ./${DESKFILE} /usr/share/applications/xclicker.desktop
	@echo "Installed XClicker"

.PHONY: deb
deb: build
	@rm -rf ${debpkgdir}
	@mkdir -p ${debpkgdir}

	@install -Dm644 ./${PKG_DIR}/deb/control ${debpkgdir}/DEBIAN/control
	@install -Dm644 ./${BUILD_DIR}/src/${BINNAME} ${debpkgdir}/bin/${BINNAME}
	@install -Dm644 ./${DESKFILE} ${debpkgdir}/usr/share/applications/xclicker.desktop
	@dpkg-deb --build ${debpkgdir}
	@dpkg-name ${PKG_DIR}/deb/package.deb -o

.PHONY: clean
clean:
	@$(RM) -rv ${BUILD_DIR}
