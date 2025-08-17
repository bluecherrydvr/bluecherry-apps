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

set -euo pipefail
mkdir -p ${DST_DIR}/usr/share/bluecherry
cp -a ${SRC_PATH}/misc/sql/*         ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/remove_untracked_media_files.sh \
       			             ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/backup_db.sh  ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/list_ether.sh ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/postinstall.sh ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/bc_db_tool.sh ${DST_DIR}/usr/share/bluecherry/

cp    ${SRC_PATH}/misc/remove_all_data.sh ${DST_DIR}/usr/share/bluecherry/
cp    ${SRC_PATH}/misc/bc-health-check ${DST_DIR}/usr/sbin/
install -d ${DST_DIR}/usr/share/bluecherry/scripts
install ${SRC_PATH}/scripts/build_helper/get_distro_release_name.sh \
	${DST_DIR}/usr/share/bluecherry/scripts
install ${SRC_PATH}/scripts/update_subdomain_certs.sh \
	${DST_DIR}/usr/share/bluecherry/scripts
cp -a ${SRC_PATH}/misc/ponvif*       ${DST_DIR}/usr/share/bluecherry/
cp -a ${SRC_PATH}/misc/onvif         ${DST_DIR}/usr/share/bluecherry/
rm -rf ${DST_DIR}/usr/share/bluecherry/ponvif*/.git
install -D ${SRC_PATH}/debian/bluecherry.conf.in \
	${DST_DIR}/usr/share/bluecherry/bluecherry.conf.in
install -m644 -D ${SRC_PATH}/debian/bluecherry.monit \
       ${DST_DIR}/etc/monit/conf.d/bluecherry
mkdir ${DST_DIR}/etc/cron.d
cp    ${SRC_PATH}/debian/bluecherry.cron ${DST_DIR}/etc/cron.d/bluecherry
cp -a ${SRC_PATH}/www ${DST_DIR}/usr/share/bluecherry
cp -a ${SRC_PATH}/misc/sudoers.d ${DST_DIR}/usr/share/bluecherry/
cp -a ${SRC_PATH}/scripts/check_dir_permission.sh ${DST_DIR}/usr/share/bluecherry/scripts/check_dir_permission.sh


echo "${VERSION}" > ${DST_DIR}/usr/share/bluecherry/version

if [[ ${PKG_TYPE} == "deb" ]]
then
	mkdir ${DST_DIR}/DEBIAN
	cp ${SRC_PATH}/debian/templates ${DST_DIR}/DEBIAN/ # TODO How to simplify this?

	mkdir -p ${DST_DIR}/usr/share/bluecherry/nginx-includes/
	cp ${SRC_PATH}/nginx-configs/php/* ${DST_DIR}/usr/share/bluecherry/nginx-includes/
	cp ${SRC_PATH}/nginx-configs/snakeoil.conf ${DST_DIR}/usr/share/bluecherry/nginx-includes/

	if [[ $(cat /etc/os-release | grep "^ID=" | grep debian) ]]
	then
		_CODENAME_=`cat /etc/os-release | grep VERSION_CODENAME | cut -d = -f 2`
	else
		_CODENAME_=`cat /etc/os-release | grep UBUNTU_CODENAME | cut -d = -f 2`
	fi

	mkdir -p ${DST_DIR}/etc/nginx/sites-enabled/

	# This handling is probably because of snakeoil.conf/subdomain.conf dynamicity
	if test -f /etc/nginx/sites-enabled/bluecherry.conf; then
		cp /etc/nginx/sites-enabled/bluecherry.conf \
			${DST_DIR}/etc/nginx/sites-enabled/
	else
		cp ${SRC_PATH}/nginx-configs/bluecherry.conf ${DST_DIR}/etc/nginx/sites-enabled/bluecherry.conf
	fi
fi
