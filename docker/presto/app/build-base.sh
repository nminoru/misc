#! /bin/sh

set -eux

echo -e "root:root" | chpasswd

yum clean all
yum makecache -y

yum install -y yum-plugin-priorities
yum install -y yum-utils
yum install -y createrepo

yum install -y yum-plugin-priorities
yum install -y yum-utils
yum install -y createrepo
yum install -y sudo bash rsyslog curl wget which patch file rsync less telnet net-tools initscripts expect telnet
yum install -y java-1.8.0-openjdk java-1.8.0-openjdk-devel java-1.8.0-openjdk-headless

echo '/usr/local/lib' >> /etc/ld.so.conf.d/local.conf
ldconfig

echo "export LANG=en_US.UTF-8" >  /etc/locale.conf
echo "export LANG=en_US.UTF-8" >> /root/.bash_profile

localedef -f UTF-8 -i en_US en_US.UTF-8
