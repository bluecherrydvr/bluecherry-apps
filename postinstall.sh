# Ensure PHP-FPM timezone matches system timezone
bash scripts/fix_php_timezone.sh 

function install_certbot
{
    source /etc/os-release

    # Handle known modern distros where apt is preferred but we can't run it inside dpkg
    if [[ ("$ID" == "ubuntu" && "$VERSION_CODENAME" == "noble") || \
          ("$ID" == "linuxmint" && "$VERSION_ID" == "22.1" && "$VERSION_CODENAME" == "xia") || \
          ("$ID" == "linuxmint" && "$VERSION_ID" == "22.2" && "$VERSION_CODENAME" == "zara") ]]; then
        echo 'Detected Ubuntu 24.04 or Linux Mint 22.1/22.2 (Noble/Xia/Zara-based) or newer'
        echo 'Skipping pip-based certbot install to preserve system integrity.'
        echo 'Please manually run the following after install completes:'
        echo '  sudo apt install certbot python3-certbot-nginx python3-certbot-dns-cloudflare'
        return
    fi

    # For older distros, fallback to pip method
    install_pip || return 0

    pip3 install --user --upgrade setuptools_rust certbot certbot-dns-subdomain-provider || true
    pip3 install --user --upgrade pip || true
    pip3 install --user --upgrade cryptography || true
    pip3 install pyopenssl --upgrade || true
} 