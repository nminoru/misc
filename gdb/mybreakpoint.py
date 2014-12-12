import gdb

class MyBreakpoint (gdb.Breakpoint):
    def __init__(self, spec):
        super(MyBreakpoint, self).__init__(spec, gdb.BP_BREAKPOINT)

    def stop (self):
        print "Happy stopping breakpoint= " + str(event.breakpoint.number)

class MyBreakpointCommand (gdb.Command):
    def __init__(self, spec):
        super(MyBreakpointCommand, self).__init__("mybreakpoint", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        MyBreakpoint(str(arg))
