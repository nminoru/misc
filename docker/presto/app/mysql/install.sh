#! /bin/sh

set -eux

/app/mysql/mysql_secure_install.sh
mysql --user=root --password=3E6hW2qr2V$ < /app/mysql/install.sql
