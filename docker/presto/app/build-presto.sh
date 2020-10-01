#! /bin/sh

set -eux

cd /tmp

wget https://repo1.maven.org/maven2/com/facebook/presto/presto-server/0.241/presto-server-0.241.tar.gz

cd /opt

tar xvf /tmp/presto-server-0.241.tar.gz
rm /tmp/presto-server-0.241.tar.gz

cp -r /app/presto/etc/ /opt/presto-server-0.241/etc/
cp /app/presto/presto.service /etc/systemd/system/presto.service
mkdir -p /var/presto/data

systemctl enable presto.service
