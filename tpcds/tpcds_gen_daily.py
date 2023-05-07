#!/usr/bin/env python3
# coding: utf-8

import sys
import subprocess

base_number = 300000000
diff_number =    300000

for day in range(7):
           directory = "data-2004-01-%02d" % (day + 1)
           sys.stdout.write(directory + "\n")
           commands_string = '''psql tpcds -U tpcdsuser -c "UPDATE generator_parameter SET diff_data_sk = 0, diff_number = {number};"
psql tpcds -U tpcdsuser -f tpcds_gen_daily.sql
mkdir -p  /home/nminoru/data/daily/{dir}/
chmod a+w /home/nminoru/data/daily/{dir}/
psql tpcds -U tpcdsuser -c "COPY store_sales3     TO '/home/nminoru/data/daily/{dir}/store_sales.dat'     WITH (FORMAT 'csv', DELIMITER '|');"
psql tpcds -U tpcdsuser -c "COPY store_returns3   TO '/home/nminoru/data/daily/{dir}/store_returns.dat'   WITH (FORMAT 'csv', DELIMITER '|');"
psql tpcds -U tpcdsuser -c "COPY web_sales3       TO '/home/nminoru/data/daily/{dir}/web_sales.dat'       WITH (FORMAT 'csv', DELIMITER '|');"
psql tpcds -U tpcdsuser -c "COPY web_returns3     TO '/home/nminoru/data/daily/{dir}/web_returns.dat'     WITH (FORMAT 'csv', DELIMITER '|');" 
psql tpcds -U tpcdsuser -c "COPY catalog_sales3   TO '/home/nminoru/data/daily/{dir}/catalog_sales.dat'   WITH (FORMAT 'csv', DELIMITER '|');"
psql tpcds -U tpcdsuser -c "COPY catalog_returns3 TO '/home/nminoru/data/daily/{dir}/catalog_returns.dat' WITH (FORMAT 'csv', DELIMITER '|');"'''.format(number = str(base_number + diff_number * day), dir = directory)
           commands = commands_string.split("\n")
           for l in commands:
                      sys.stdout.write('*** ' + l)
                      subprocess.run([l], shell=True)
                      sys.stdout.write("\n")
