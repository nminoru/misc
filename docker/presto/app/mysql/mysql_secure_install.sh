#!/bin/bash

MYSQL=$(grep 'temporary password' /var/log/mysqld.log | sed -r 's/^.*root@localhost: (.*)$/\1/')
MYSQL_ROOT_PASSWORD="3E6hW2qr2V$"

expect -c '

set timeout 10;
spawn mysql_secure_installation;

expect "Enter password for user root:";
send "'"${MYSQL}"'\n";

expect "New password:";
send "'"${MYSQL_ROOT_PASSWORD}"'\n";

expect "Re-enter new password:";
send "'"${MYSQL_ROOT_PASSWORD}"'\n";

expect "Change the password for root ? \(\(Press y\|Y for Yes, any other key for No\) :";
send "n\n";

expect "Do you wish to continue with the password provided?\(Press y|Y for Yes, any other key for No\) :";
send "y\n";

expect "Remove anonymous users? \(Press y\|Y for Yes, any other key for No\) :";
send "y\n";

expect "Disallow root login remotely? \(Press y\|Y for Yes, any other key for No\) :";
send "y\n";

expect "Remove test database and access to it? \(Press y\|Y for Yes, any other key for No\) :";
send "y\n";

expect "Reload privilege tables now? \(Press y\|Y for Yes, any other key for No\) :";
send "y\n";

expect eof;'
