#! /bin/sh

set -eux

useradd alice
useradd bob

echo -e "root:root"   | chpasswd
echo -e "alice:alice" | chpasswd
echo -e "bob:bob"     | chpasswd

/usr/sbin/kdb5_util -r EXAMPLE.COM -P changeme create -s

/usr/sbin/kadmin.local -q "addprinc -pw password root/admin"

/usr/sbin/kadmin.local -q "addprinc -pw alice alice"
/usr/sbin/kadmin.local -q "addprinc -pw bob   bob"
/usr/sbin/kadmin.local -q "addprinc -randkey  host/kdc.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  host/client.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  host/www.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  HTTP/www.example.com"




