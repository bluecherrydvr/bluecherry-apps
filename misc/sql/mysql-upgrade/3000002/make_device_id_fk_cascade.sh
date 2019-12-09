#!/bin/bash

echo "alter table \`EventsCam\` drop foreign key \`EventsCam_ibfk_3\`;\
alter table \`EventsCam\` add CONSTRAINT \`EventsCam_ibfk_3\` FOREIGN KEY (\`device_id\`) REFERENCES \`Devices\` (\`id\`) ON UPDATE CASCADE;\
alter table \`Media\` drop foreign key \`Media_ibfk_1\`;\
alter table \`Media\` add CONSTRAINT \`Media_ibfk_1\` FOREIGN KEY (\`device_id\`) REFERENCES \`Devices\` (\`id\`) ON UPDATE CASCADE;\
alter table \`PTZPresets\` drop foreign key \`PTZPresets_ibfk_1\`;\
alter table \`PTZPresets\` add CONSTRAINT \`PTZPresets_ibfk_1\` FOREIGN KEY (\`device_id\`) REFERENCES \`Devices\` (\`id\`) ON UPDATE CASCADE;" | mysql -h"$host" -D"$dbname" -u"$user" -p"$password"

exit 0

