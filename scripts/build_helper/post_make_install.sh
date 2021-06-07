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
cp    ${SRC_PATH}/misc/remove_all_data.sh ${DST_DIR}/usr/share/bluecherry/
install -d ${DST_DIR}/usr/share/bluecherry/scripts
install ${SRC_PATH}/scripts/build_helper/get_distro_release_name.sh \
	${DST_DIR}/usr/share/bluecherry/scripts
install ${SRC_PATH}/scripts/update_subdomain_certs.sh \
	${DST_DIR}/usr/share/bluecherry/scripts
cp -a ${SRC_PATH}/misc/ponvif*       ${DST_DIR}/usr/share/bluecherry/
rm -rf ${DST_DIR}/usr/share/bluecherry/ponvif*/.git
install -D ${SRC_PATH}/debian/bluecherry.conf.in \
	${DST_DIR}/usr/share/bluecherry/bluecherry.conf.in
install -m644 -D ${SRC_PATH}/debian/bluecherry.monit \
       ${DST_DIR}/etc/monit/conf.d/bluecherry
mkdir ${DST_DIR}/etc/cron.d
cp    ${SRC_PATH}/debian/bluecherry.cron ${DST_DIR}/etc/cron.d/bluecherry
cp -a ${SRC_PATH}/www ${DST_DIR}/usr/share/bluecherry
echo "${VERSION}" > ${DST_DIR}/usr/share/bluecherry/version

if [[ ${PKG_TYPE} == "deb" ]]
then
	mkdir ${DST_DIR}/DEBIAN
	cp ${SRC_PATH}/debian/templates ${DST_DIR}/DEBIAN/ # TODO How to simplify this?

	mkdir -p ${DST_DIR}/usr/share/bluecherry/nginx-includes/
	cp ${SRC_PATH}/nginx-configs/php/* ${DST_DIR}/usr/share/bluecherry/nginx-includes/
	_CODENAME_=`cat /etc/os-release | grep UBUNTU_CODENAME | cut -d = -f 2`

	mkdir -p ${DST_DIR}/etc/nginx/sites-enabled/
	cat ${SRC_PATH}/nginx-configs/bluecherry.conf | sed \
		-e "s/__BLUECHERRY_DIST_CODENAME__/$_CODENAME_/" \
		> ${DST_DIR}/etc/nginx/sites-enabled/bluecherry.conf

	touch ${DST_DIR}/usr/share/bluecherry/nginx-includes/snakeoil.conf
	echo "ssl_certificate /etc/ssl/certs/ssl-cert-snakeoil.pem;" >> ${DST_DIR}/usr/share/bluecherry/nginx-includes/snakeoil.conf
	echo "ssl_certificate_key /etc/ssl/private/ssl-cert-snakeoil.key;" >> ${DST_DIR}/usr/share/bluecherry/nginx-includes/snakeoil.conf
fi

# Apache configurations are no more required
#if [[ ${PKG_TYPE} == "deb" ]]
#then
#	install -d ${DST_DIR}/etc/apache2/sites-available
#	cat ${SRC_PATH}/debian/apache.conf | sed \
#		-e "s/__BLUECHERRY_APACHE_ERROR_LOG__/\/var\/log\/apache2\/bluecherry-error.log/" \
#	        -e "s/__BLUECHERRY_APACHE_ACCESS_LOG__/\/var\/log\/apache2\/bluecherry-access.log/" \
#		-e "s/__BLUECHERRY_APACHE_CERTIFICATE_FILE__/\/etc\/ssl\/certs\/ssl-cert-snakeoil.pem/" \
#		-e "s/__BLUECHERRY_APACHE_CERTIFICATE_KEY_FILE__/\/etc\/ssl\/private\/ssl-cert-snakeoil.key/" \
#	        > ${DST_DIR}/etc/apache2/sites-available/bluecherry.conf
#else
#	install -d ${DST_DIR}/etc/httpd/sites-available
#	cat ${SRC_PATH}/debian/apache.conf | sed \
#	        -e "s/__BLUECHERRY_APACHE_ERROR_LOG__/\/var\/log\/httpd\/bluecherry_error_log/" \
#	        -e "s/__BLUECHERRY_APACHE_ACCESS_LOG__/\/var\/log\/httpd\/bluecherry_access_log/" \
#	        -e "s/__BLUECHERRY_APACHE_CERTIFICATE_FILE__/\/etc\/pki\/tls\/certs\/localhost.crt/" \
#		-e "s/__BLUECHERRY_APACHE_CERTIFICATE_KEY_FILE__/\/etc\/pki\/tls\/private\/localhost.key/" \
#	        > ${DST_DIR}/etc/httpd/sites-available/bluecherry.conf
#fi
#       install -m644${DST_DIR}.apparmor \
#              ${DST_DIR}/etc/apparmor.d/usr.sbin.bc-server
