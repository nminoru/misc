#!/usr/bin/env python3
# coding: utf-8

import sys
import subprocess

base_number = 400000000
diff_number =    300000

with open('tpcds_gen_hourly.sql') as infile:
           sql2 = infile.read()

for day in range(7):
           directory = "data-2004-01-%02d" % (day + 1)
           sys.stdout.write('#' + directory + "\n")
           commands_string = '''psql tpcds -U tpcdsuser -c "UPDATE generator_parameter SET diff_data_sk = 0, diff_number = {number};"
psql tpcds -U tpcdsuser -f tpcds_gen_daily.sql'''.format(number = str(base_number + diff_number * day))
           commands = commands_string.split("\n")
           for l in commands:
                      sys.stdout.write('*** ' + l)
                      subprocess.run([l], shell=True)
                      sys.stdout.write("\n")
                      
           # 2 時間づつ
           for hour in range(int(24 / 2)):
                      directory = "data-2004-01-%02d/%02d" % (day + 1, hour)
                      sys.stdout.write('#' + directory + "\n")
                      start_time  = 3600 * 2 * hour
                      end_time    = 3600 * 2 * (hour + 1) - 1
                      sql2_mod = sql2.format(start_time = str(start_time), end_time = str(end_time))
                      commands = 'psql tpcds -U tpcdsuser -c "' + ' '.join(sql2_mod.split("\n")) + '"'
                      sys.stdout.write('*** ' + commands)
                      subprocess.run([commands], shell=True)
                      sys.stdout.write("\n")

                      commands_string = '''mkdir -p  /home/nminoru/data/hourly/{dir}/
chmod a+w /home/nminoru/data/hourly/{dir}/
psql tpcds -U tpcdsuser -c "COPY store_sales4     TO '/home/nminoru/data/hourly/{dir}/store_sales.dat'     WITH (FORMAT 'csv', DELIMITER '|');"
psql tpcds -U tpcdsuser -c "COPY store_returns4   TO '/home/nminoru/data/hourly/{dir}/store_returns.dat'   WITH (FORMAT 'csv', DELIMITER '|');"
psql tpcds -U tpcdsuser -c "COPY web_sales4       TO '/home/nminoru/data/hourly/{dir}/web_sales.dat'       WITH (FORMAT 'csv', DELIMITER '|');"
psql tpcds -U tpcdsuser -c "COPY web_returns4     TO '/home/nminoru/data/hourly/{dir}/web_returns.dat'     WITH (FORMAT 'csv', DELIMITER '|');" 
psql tpcds -U tpcdsuser -c "COPY catalog_sales4   TO '/home/nminoru/data/hourly/{dir}/catalog_sales.dat'   WITH (FORMAT 'csv', DELIMITER '|');"
psql tpcds -U tpcdsuser -c "COPY catalog_returns4 TO '/home/nminoru/data/hourly/{dir}/catalog_returns.dat' WITH (FORMAT 'csv', DELIMITER '|');"'''.format(dir = directory)
                      commands = commands_string.split("\n")
                      for l in commands:
                                 sys.stdout.write('*** ' + l)
                                 subprocess.run([l], shell=True)
                                 sys.stdout.write("\n")
                                 
