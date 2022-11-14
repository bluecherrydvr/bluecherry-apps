#!/bin/bash
export PATH=~/.local/bin:${PATH}

subdomain=$1
email=$2
token=$3

if [ "$#" -ne 3 ]; then
    echo "Invalid or missing arguments"
    exit 1
fi

pip3 install --user setuptools_rust certbot certbot-dns-subdomain-provider
pip3 install --user --upgrade pip
pip3 install --user --upgrade cryptography

function start_nginx()
{
    if [ ! -z `which service` ]
    then
        service nginx start || true
    else
        invoke-rc.d nginx start || true
    fi
}

function stop_nginx()
{
    if [ ! -z `which service` ]
    then
        service nginx stop || true
    else
        invoke-rc.d nginx stop || true
    fi
}

credentials=/tmp/dns-subdomain-credentials.ini

if test -f $credentials; then
    rm $credentials
fi

subdomain_conf=/usr/share/bluecherry/nginx-includes/subdomain.conf
snakeoil_conf=/usr/share/bluecherry/nginx-includes/snakeoil.conf
endpoint=https://domains.bluecherrydvr.com/subdomain-provider

echo "dns_subdomain_provider_endpoint_url=$endpoint" >> $credentials
echo "dns_subdomain_provider_token=$token" >> $credentials
chmod 600 $credentials

# Generate certificates
echo "Generating certs..."

certbot certonly --non-interactive --agree-tos --work-dir=/tmp --logs-dir=/tmp \
    --config-dir=/usr/share/bluecherry/nginx-includes/letsencrypt/ \
    --dns-subdomain-provider-credentials $credentials \
    -m $email --authenticator dns-subdomain-provider \
    -d $subdomain.bluecherry.app -v

rm $credentials

if [ ! -f "/usr/share/bluecherry/nginx-includes/letsencrypt/live/$subdomain.bluecherry.app/fullchain.pem" ] || \
   [ ! -f "/usr/share/bluecherry/nginx-includes/letsencrypt/live/$subdomain.bluecherry.app/privkey.pem" ]; then
    echo "No certificates found"
    exit 1
fi

# Stop nginx before changing configuration
stop_nginx

if test -f $subdomain_conf; then
    rm $subdomain_conf
fi

echo "ssl_certificate /usr/share/bluecherry/nginx-includes/letsencrypt/live/$subdomain.bluecherry.app/fullchain.pem;" >> $subdomain_conf
echo "ssl_certificate_key /usr/share/bluecherry/nginx-includes/letsencrypt/live/$subdomain.bluecherry.app/privkey.pem;" >> $subdomain_conf

# Change existing snakeoil configuration with new one
sed -i "s#$snakeoil_conf#$subdomain_conf#g" /etc/nginx/sites-enabled/bluecherry.conf
chown -R www-data:www-data "/usr/share/bluecherry/nginx-includes/letsencrypt"    

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
