if [ $# -eq 0 ]
  then
    php import-cambase-data.php
    exit 1
fi

echo ""
echo "You are about to import Cambase data into a local dump file."
echo "NOTE: This is a long operation, it can take several minutes."
echo ""

read -p "Press [ENTER] key to start ... Press CTRL-C To exit.";

php import-cambase-data.php $@
