##! /bin/sh

set -eux

ldconfig

echo "export LANG=ja_JP.UTF-8" >  /etc/locale.conf
echo "export LANG=ja_JP.UTF-8" >> /root/.bash_profile

echo -e "root:root" | chpasswd

useradd hdfs

yum install -y yum-plugin-priorities
yum install -y yum-utils
yum install -y createrepo

yum clean all
yum makecache -y

yum install -y sudo rsyslog openssh bash openssl rsync openssh-server openssh-clients
yum install -y less wget which telnet net-tools bind-utils
yum install -y java-1.8.0-openjdk java-1.8.0-openjdk-devel java-1.8.0-openjdk-headless

echo "export JAVA_HOME=/usr/lib/jvm/java-1.8.0/" >> /root/.bashrc

rm -f /root/.ssh/id_rsa
ssh-keygen -q -N "" -f /root/.ssh/id_rsa
cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
chmod 600 /root/.ssh/authorized_keys

mkdir -p /opt/hadoop
cd /opt/hadoop

tar xvf /tmp/hadoop-3.2.1.tar.gz
rm /tmp/hadoop-3.2.1.tar.gz
rm -rf /opt/hadoop/hadoop-3.2.1/share/doc/

mv /tmp/core-site.xml /opt/hadoop/hadoop-3.2.1/etc/hadoop/core-site.xml
mv /tmp/hdfs-site.xml /opt/hadoop/hadoop-3.2.1/etc/hadoop/hdfs-site.xml

chown -R hdfs /opt/hadoop

export JAVA_HOME=/usr/lib/jvm/java-1.8.0/
export HADOOP_HOME=/opt/hadoop/hadoop-3.2.1/

systemctl enable sshd.service

echo "/sbin/runuser -l hdfs ${HADOOP_HOME}/sbin/start-dfs.sh" >> /etc/rc.d/rc.local

chmod u+x /etc/rc.d/rc.local
