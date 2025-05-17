#!/bin/bash

# Get system timezone
if [ -f /etc/timezone ]; then
    SYSTEM_TZ=$(cat /etc/timezone)
else
    SYSTEM_TZ=$(timedatectl | grep 'Time zone' | awk '{print $3}')
fi

echo "System timezone: $SYSTEM_TZ"
echo

for version in $(ls /etc/php | grep -E '^[0-9]+\.[0-9]+$'); do
    INI_FILE="/etc/php/$version/fpm/php.ini"
    if [ -f "$INI_FILE" ]; then
        PHP_TZ=$(grep "^date.timezone" "$INI_FILE" | awk -F'=' '{print $2}' | xargs)
        if [ -z "$PHP_TZ" ]; then
            PHP_TZ="(not set)"
        fi

        # Print configured timezone
        echo "PHP $version (fpm) php.ini timezone: $PHP_TZ"

        # Print actual timezone PHP is using
        if command -v php$version > /dev/null 2>&1; then
            ACTUAL_TZ=$(php$version -d "error_reporting=0" -r 'echo date_default_timezone_get();')
            echo "PHP $version (fpm) actual timezone: $ACTUAL_TZ"
        else
            ACTUAL_TZ="$PHP_TZ"
            echo "  Could not run php$version to check actual timezone."
        fi

        if [ "$ACTUAL_TZ" != "$SYSTEM_TZ" ]; then
            echo "  WARNING: PHP $version (fpm) actual timezone does not match system timezone!"
            read -p "  Do you want to fix the timezone for PHP $version (fpm) to '$SYSTEM_TZ'? [Y/n] " answer
            answer=${answer:-Y}
            if [[ "$answer" =~ ^[Yy]$ ]]; then
                # Update or add date.timezone in php.ini
                if grep -q "^date.timezone" "$INI_FILE"; then
                    sudo sed -i "s|^date.timezone.*|date.timezone = $SYSTEM_TZ|" "$INI_FILE"
                elif grep -q "^;date.timezone" "$INI_FILE"; then
                    sudo sed -i "s|^;date.timezone.*|date.timezone = $SYSTEM_TZ|" "$INI_FILE"
                else
                    echo "date.timezone = $SYSTEM_TZ" | sudo tee -a "$INI_FILE" > /dev/null
                fi
                echo "  Updated $INI_FILE to set date.timezone = $SYSTEM_TZ"
                sudo systemctl restart php${version}-fpm
                echo "  Restarted php${version}-fpm"
            else
                echo "  Skipped fixing timezone for PHP $version (fpm)."
            fi
        else
            echo "  OK: PHP $version (fpm) actual timezone matches system timezone."
        fi
        echo
    fi
done 