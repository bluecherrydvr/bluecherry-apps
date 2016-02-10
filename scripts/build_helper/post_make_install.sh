#!/bin/bash

# !!! ATTENTION !!!
# This file included from debian/rules override_dh_installdeb
# and rpm .spec %install section
# so DO NOT place here distributive specific things
# AND DO place here everything must be in both packages DEB and RPM

PKG_TYPE=$1 # deb or rpm
SRC_PATH=$2 #./ or %{_builddir}/%{name}
DST_DIR=$3  # debian/bluecherry or %{buildroot}
VERSION=$4  # bluecherry/version from changelog

mkdir -p ${DST_DIR}/usr/share/bluecherry
cp -a ${SRC_PATH}/misc/sql/*         ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/remove_untracked_media_files.sh \
       			             ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/backup_db.sh  ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/list_ether.sh ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/postinstall.sh ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/bc_db_tool.sh ${DST_DIR}/usr/share/bluecherry/
cp -a ${SRC_PATH}/misc/ponvif*       ${DST_DIR}/usr/share/bluecherry/
rm -rf ${DST_DIR}/usr/share/bluecherry/ponvif*/.git

if [[ ${PKG_TYPE} == "deb" ]]
then
	mkdir ${DST_DIR}/DEBIAN
	cp ${SRC_PATH}/debian/templates ${DST_DIR}/DEBIAN/ # TODO How to simplify this?
fi
if [[ ${PKG_TYPE} == "deb" ]]
then
	install -m640 -g www-data -D ${SRC_PATH}/debian/bluecherry.conf.in \
       		${DST_DIR}/usr/share/bluecherry/bluecherry.conf.in
else
	install -m640 -g apache -D ${SRC_PATH}/debian/bluecherry.conf.in \
                ${DST_DIR}/usr/share/bluecherry/bluecherry.conf.in
fi
if [[ ${PKG_TYPE} == "deb" ]]
then
	install -m644 -D ${SRC_PATH}/debian/apache.conf \
       		${DST_DIR}/etc/apache2/sites-available/bluecherry.conf
else
	install -m644 -D ${SRC_PATH}/debian/apache.conf \
		${DST_DIR}/etc/httpd/sites-available/bluecherry.conf
fi
install -m644 -D ${SRC_PATH}/debian/bluecherry.monit \
       ${DST_DIR}/etc/monit/conf.d/bluecherry
#       install -m644${DST_DIR}.apparmor \
#              ${DST_DIR}/etc/apparmor.d/usr.sbin.bc-server
mkdir ${DST_DIR}/etc/cron.d
cp    ${SRC_PATH}/debian/bluecherry.cron ${DST_DIR}/etc/cron.d/bluecherry
cp -a ${SRC_PATH}/www ${DST_DIR}/usr/share/bluecherry
echo "${VERSION}" > ${DST_DIR}/usr/share/bluecherry/version
