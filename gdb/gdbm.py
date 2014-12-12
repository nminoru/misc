#! /usr/bin/python

import dumbdbm

db = dumbdbm.open("backtrace.gdb", 'c')

print db['aaa']

# db['aaa'] = 'bbb'

db.close()
