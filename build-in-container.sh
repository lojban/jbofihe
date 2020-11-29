#!/bin/bash

cd /src
rm -rf build/
mkdir build/
fpm --version
ls -ld build
ls -l | tail
perl config.pl --prefix=/usr/local
make all
DESTDIR=build/ make install
rm -rf build/usr/lib/.build-id
cp find_lojban.pl build/usr/local/bin/find_lojban
rm -f jbofihe-0.41-1.x86_64.rpm
fpm -s dir -t rpm -n jbofihe -v 0.41 --rpm-rpmbuild-define "_build_id_links none" -C build/ usr/local
rpm -qpl jbofihe-0.41-1.x86_64.rpm | sort
