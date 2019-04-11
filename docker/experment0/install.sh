#! /bin/sh

/usr/pgsql-9.6/bin/postgresql96-setup initdb

/sbin/runuser -l postgres -c psql postgres -f install.sql


