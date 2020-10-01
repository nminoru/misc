#! /bin/sh

set -eux

yum localinstall -y http://dev.mysql.com/get/mysql80-community-release-el7-3.noarch.rpm
yum install -y mysql-community-server mysql-community-client mysql-community-common mysql-community-libs

cp /app/mysql/my.cnf /etc/my.cnf

systemctl enable mysqld
