#!/bin/bash

# script to change permission of file

# File variable to store location

FILE="$1"

#if [[ ! -e "${FILE}" ]]; then
#    mkdir -p "${FILE}"
##        echo "creating directory..."
#elif [[ ! -d "${FILE}" ]]; then
#    echo "FILE already exists but is not a directory"
#fi

## to remove  write permission of other and group of file
#chmod 770 "${FILE}"
#chown -R bluecherry:bluecherry "${FILE}"

# find out if file has write permission or not
#[ -w $FILE ] && W="Write = yes" || W="Write = No"
[ -w $FILE ] && W=w || W='-'

# find out if file has excute permission or not
[ -x $FILE ] && X=x || X='-'

# find out if file has read permission or not
[ -r $FILE ] && R=r || R='-'

#echo "$FILE permissions"
echo "-$W$R$X"
