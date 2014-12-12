import gdb

tuple = gdb.decode_line("test1.c:print_name")
symtab_and_line = tuple[0]

print "line: " + str(symtab_and_line.line)
print "last: " + str(symtab_and_line.last)

print "filename: " + symtab_and_line.symtab.filename
print "fullname: " + symtab_and_line.symtab.fullname()


tuple = gdb.decode_line("test2.c:print_name")
symtab_and_line = tuple[0]

print "line: " + str(symtab_and_line.line)
print "last: " + str(symtab_and_line.last)

print "filename: " + symtab_and_line.symtab.filename
print "fullname: " + symtab_and_line.symtab.fullname()
