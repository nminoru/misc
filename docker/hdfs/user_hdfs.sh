#! /bin/sh

set -eux

echo "export JAVA_HOME=/usr/lib/jvm/java-1.8.0/" >> ~/.bashrc

rm -f ~/.ssh/id_rsa
ssh-keygen -q -N "" -f ~/.ssh/id_rsa
cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys

export JAVA_HOME=/usr/lib/jvm/java-1.8.0/
export HADOOP_HOME=/opt/hadoop/hadoop-3.3.0/

$HADOOP_HOME/bin/hadoop namenode -format
