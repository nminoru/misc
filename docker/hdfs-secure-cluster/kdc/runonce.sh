#! /bin/sh

set -eux

echo runonce.sh

/usr/sbin/kadmin.local -q "ktadd -k /var/keytabs/hadoop.keytab nn/namenode.example.com@EXAMPLE.COM"
/usr/sbin/kadmin.local -q "ktadd -k /var/keytabs/hadoop.keytab dn/datanode.example.com@EXAMPLE.COM"
/usr/sbin/kadmin.local -q "ktadd -k /var/keytabs/hadoop.keytab host/namenode.example.com@EXAMPLE.COM"
/usr/sbin/kadmin.local -q "ktadd -k /var/keytabs/hadoop.keytab host/datanode.example.com@EXAMPLE.COM"
/usr/sbin/kadmin.local -q "ktadd -k /var/keytabs/hadoop.keytab HTTP/namenode.example.com@EXAMPLE.COM"
/usr/sbin/kadmin.local -q "ktadd -k /var/keytabs/hadoop.keytab HTTP/datanode.example.com@EXAMPLE.COM"

chmod a+rx /var/keytabs
chmod a+r  /var/keytabs/hadoop.keytab
