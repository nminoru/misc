#! /bin/sh

set -eux

yum install -y https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm
yum install -y postgresql96 postgresql96-server postgresql96-libs postgresql96-docs postgresql96-contrib

/sbin/runuser -l postgres -c "/usr/pgsql-9.6/bin/initdb -D /var/lib/pgsql/9.6/data --encoding=UTF-8 --locale=C"
/sbin/runuser -l postgres -c "cp  -f /app/postgresql/pg_hba.conf     /var/lib/pgsql/9.6/data/pg_hba.conf"
/sbin/runuser -l postgres -c "cat /app/postgresql/postgresql.conf >> /var/lib/pgsql/9.6/data/postgresql.conf"
/sbin/runuser -l postgres -c "/usr/pgsql-9.6/bin/pg_ctl start  -w -D /var/lib/pgsql/9.6/data/"
/sbin/runuser -l postgres -c "/usr/pgsql-9.6/bin/psql postgres -f /app/postgresql/install.sql"
/sbin/runuser -l postgres -c "/usr/pgsql-9.6/bin/pg_ctl stop   -w -D /var/lib/pgsql/9.6/data/"

systemctl enable postgresql-9.6.service
