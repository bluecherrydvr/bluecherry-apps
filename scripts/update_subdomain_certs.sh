#!/bin/sh

subdomain=$1
email=$2
token=$3

if [ "$#" -ne 3 ]; then
    echo "Invalid or missing arguments"
    exit 1
fi

pip3 install setuptools_rust certbot certbot-dns-subdomain-provider
pip3 install --upgrade pip
pip3 install --upgrade cryptography

function start_nginx
{
    if [ ! -z `which service` ]
    then
        service nginx start || true
    else
        invoke-rc.d nginx start || true
    fi
}

function stop_nginx
{
    if [ ! -z `which service` ]
    then
        service nginx stop || true
    else
        invoke-rc.d nginx stop || true
    fi
}

if test -f "dns-subdomain-credintials.ini"; then
    rm dns-subdomain-credintials.ini
fi

subdomain_conf=/usr/share/bluecherry/nginx-includes/subdomain.conf
snakeoil_conf=/usr/share/bluecherry/nginx-includes/snakeoil.conf
endpoint=https://domains.bluecherrydvr.com/subdomain-provider

echo "dns_subdomain_provider_endpoint_url=$endpoint" >> dns-subdomain-credintials.ini
echo "dns_subdomain_provider_token=$token" >> dns-subdomain-credintials.ini

# Generate certificates
certbot certonly --non-interactive --agree-tos \
    -m $email --authenticator dns-subdomain-provider \
    --dns-subdomain-provider-credentials \
    ./dns-subdomain-credintials.ini \
    -d $subdomain.bluecherry.app

# No more required
rm dns-subdomain-credintials.ini

if [ ! -f "/etc/letsencrypt/live/$subdomain.bluecherry.app/cert.pem" ] || \
   [ ! -f "/etc/letsencrypt/live/$subdomain.bluecherry.app/privkey.pem" ]; then
    echo "No certificates found"
    exit 1
fi

# Stop nginx before changing configuration
stop_nginx

if test -f $subdomain_conf; then
    rm $subdomain_conf
fi

echo "ssl_certificate /etc/letsencrypt/live/$subdomain.bluecherry.app/cert.pem;" >> $subdomain_conf
echo "ssl_certificate_key /etc/letsencrypt/live/$subdomain.bluecherry.app/privkey.pem;" >> $subdomain_conf

# Change existing snakeoil configuration with new one
sed -i "s#$snakeoil_conf#$subdomain_conf#g" \
    /etc/nginx/sites-enabled/bluecherry.conf

# Test new configuration
nginx -t 2>/dev/null > /dev/null

if [[ $? == 0 ]]; then
    # Reenable our site in nginx
    start_nginx
else
    echo "Nginx configuration failure"
    exit 1
fi

exit 0

