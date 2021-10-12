pkgname=xclicker
pkgver=v1.0.0.r3.g64d0d16
pkgrel=1
pkgdesc='A blazing fast gui autoclicker for linux'
arch=('x86_64' 'aarch64')
url='https://github.com/robiot/XClicker'
license=('GPL-3.0')
makedepends=('git' 'meson' 'libx11' 'libxi')
sha512sums=('SKIP')
source=("git+https://github.com/robiot/$pkgname.git")

pkgver() {
  cd $pkgname

  echo $(git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g')
}

build() {
  cd $pkgname

  make
}

package() {
  cd $pkgname

  install -Dm 644 img/icon.png ${pkgdir}/usr/share/pixmaps/${pkgname}.png
  install -Dm 755 build/src/${pkgname} ${pkgdir}/usr/bin/${pkgname}
  install -Dm 755 xclicker.desktop ${pkgdir}/usr/share/applications/xclicker.desktop
}

