#! /bin/sh

set -eux

echo init-kdc.sh

/usr/sbin/kdb5_util -r EXAMPLE.COM -P changeme create -s

/usr/sbin/kadmin.local -q "addprinc -pw password root/admin"

/usr/sbin/kadmin.local -q "addprinc -pw hdfs  hdfs"
/usr/sbin/kadmin.local -q "addprinc -pw alice alice"
/usr/sbin/kadmin.local -q "addprinc -pw bob   bob"
/usr/sbin/kadmin.local -q "addprinc -randkey  host/kdc.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  host/namenode.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  host/datanode.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  nn/namenode.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  dn/datanode.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  hdfs/namenode.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  hdfs/datanode.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  HTTP/namenode.example.com"
/usr/sbin/kadmin.local -q "addprinc -randkey  HTTP/datanode.example.com"
