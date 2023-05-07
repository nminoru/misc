#!/usr/bin/env python
# coding: utf-8

# catalog_returns.dat catalog_sales.dat inventory.dat store_returns.dat store_sales.dat web_returns.dat web_sales.dat

import sys

# 2452519 | 2002-09-01
# 2452520 | 2002-09-02
# 2452521 | 2002-09-03
# 2452522 | 2002-09-04
# 2452523 | 2002-09-05
# 2452524 | 2002-09-06
# 2452525 | 2002-09-07
newdate = 2452525

args = sys.argv

for arg in args[1:]:
    with open(arg) as infile:
        with open(arg + '.new', mode = 'w') as outfile:        
            while (True):
                line = infile.readline()
                if (not line):
                    break

                try:
                    columns = line.split('|')
                    col0_null = not columns[0]
                    col2_null = not columns[2]
                    if (col0_null):
                        new_line = line
                    elif not col2_null and (arg == 'catalog_sales.dat' or arg == 'web_sales.dat'):
                        columns[0] = str(newdate)
                        new_line = '|'.join(columns)                        
                        sold_date = int(columns[0])
                        ship_date = int(columns[2])
                        columns[0] = str(newdate)
                        columns[2] = str(newdate + ship_date - sold_date)
                        new_line = '|'.join(columns)
                    else:
                        columns[0] = str(newdate)
                        new_line = '|'.join(columns)
                except ValueError as e:
                    # sys.stderr.write(e)
                    sys.stderr.write(line)
                    sys.exit(-1)
                    
                # sys.stdout.write(new_line)
                outfile.write(new_line)
