import gdb

def mybacktrace():
    bactraces = gdb.execute("backtrace", True, True)
    print "backtrace"
    print bactraces
