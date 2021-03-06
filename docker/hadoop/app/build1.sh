#! /bin/sh

set -eux

ldconfig

echo "export LANG=ja_JP.UTF-8" >  /etc/locale.conf
echo "export LANG=ja_JP.UTF-8" >> /root/.bashrc

useradd hadoop
useradd hdfs

echo -e "root:root"     | chpasswd
echo -e "hadoop:hadoop" | chpasswd
echo -e "hdfs:hdfs"     | chpasswd

echo "hadoop ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

yum install -y yum-plugin-priorities
yum install -y yum-utils
yum install -y createrepo

yum clean all
yum makecache -y

yum install -y sudo rsyslog openssh bash openssl rsync openssh-server openssh-clients
yum install -y less wget curl which telnet net-tools bind-utils
yum install -y java-1.8.0-openjdk java-1.8.0-openjdk-devel java-1.8.0-openjdk-headless
yum install -y krb5-workstation

rm -f /root/.ssh/id_rsa
ssh-keygen -q -N "" -f /root/.ssh/id_rsa
cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
chmod 600 /root/.ssh/authorized_keys

mkdir /opt/hadoop
