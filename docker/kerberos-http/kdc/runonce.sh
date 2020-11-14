#! /bin/sh

echo runonce.sh

/usr/sbin/kadmin.local -q "ktadd -k /var/keytabs/http.keytab HTTP/www.example.com"
chown apache /var/keytabs/http.keytab
