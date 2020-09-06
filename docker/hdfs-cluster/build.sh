#! /bin/sh

rm -f /root/.ssh/id_rsa
ssh-keygen -q -N "" -f /root/.ssh/id_rsa
cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
chmod 600 /root/.ssh/authorized_keys

