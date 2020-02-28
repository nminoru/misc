#! /bin/sh

set -eux

mv /tmp/core-site.xml /opt/hadoop/hadoop-3.2.1/etc/hadoop/core-site.xml
mv /tmp/hdfs-site.xml /opt/hadoop/hadoop-3.2.1/etc/hadoop/hdfs-site.xml

export JAVA_HOME=/usr/lib/jvm/java-1.8.0/
export HADOOP_HOME=/opt/hadoop/hadoop-3.2.1/

systemctl enable sshd.service

echo "/sbin/runuser -l hdfs ${HADOOP_HOME}/sbin/start-dfs.sh" >> /etc/rc.d/rc.local

chmod u+x /etc/rc.d/rc.local
