#! /bin/sh

set -eux

mv /app/core-site.xml /opt/hadoop/hadoop-3.3.0/etc/hadoop/core-site.xml
mv /app/hdfs-site.xml /opt/hadoop/hadoop-3.3.0/etc/hadoop/hdfs-site.xml

export JAVA_HOME=/usr/lib/jvm/java-1.8.0/
export HADOOP_HOME=/opt/hadoop/hadoop-3.3.0/

systemctl enable sshd.service
systemctl enable rsyslog.service
systemctl enable hdfs.service

# echo "/sbin/runuser -l hadoop ${HADOOP_HOME}/sbin/start-dfs.sh" >> /etc/rc.d/rc.local

chmod u+x /etc/rc.d/rc.local
