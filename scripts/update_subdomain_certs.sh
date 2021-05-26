pip3 install setuptools_rust certbot certbot-dns-subdomain-provider
pip3 install --upgrade pip
pip3 install --upgrade cryptography

subdomain_conf=/usr/share/bluecherry/nginx-includes/subdomain.conf
snakeoil_conf=/usr/share/bluecherry/nginx-includes/snakeoil.conf
subdomain=$1
token=$2

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

echo "dns_subdomain_provider_endpoint_url=$endpoint" >> dns-subdomain-credintials.ini
echo "dns_subdomain_provider_token=$token" >> dns-subdomain-credintials.ini

# Generate certificates
sudo certbot certonly --authenticator dns-subdomain-provider \
    --dns-subdomain-provider-credentials \
    ./dns-subdomain-credintials.ini \
    -d $subdomain.bluecherry.app

if [ ! -f "/etc/letsencrypt/live/$subdomain.bluecherry.app/cert.pem" ] || \
   [ ! -f "/etc/letsencrypt/live/$subdomain.bluecherry.app/privkey.pem" ]; then
    echo "No certificates found"
    exit
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
fi


