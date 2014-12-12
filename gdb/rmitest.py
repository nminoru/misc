import gdb


def set_breakpoint(filename, bnum):
    target   = gdb.parse_and_eval("global_options.verify.target_function").string()
    position = gdb.parse_and_eval("global_options.verify.before_position").string()
    if (position == ""):
        break_at_keyword(target, filename, +1)
    else:
        break_at_keyword_before_position(target, position, filename, +1)
    
    gdb.parse_and_eval("$breakcounter" + str(bnum) + " = 0")


def handle_breakpoint(bnum):
    etype        = gdb.parse_and_eval("global_options.verify.etype")
    var          = gdb.parse_and_eval("global_options.verify.variable").string()
    error_no     = gdb.parse_and_eval("global_options.verify.error_no")
    count_limit  = gdb.parse_and_eval("global_options.verify.count")

    breakcounter = "$breakcounter" + str(bnum)
    count        = gdb.parse_and_eval(breakcounter)

    count += 1;

    if (count < count_limit):
        gdb.parse_and_eval(breakcounter + " = " + str(count))
        return

    if (etype == 1):
        # the returning value is -1, errno is set to error number
        gdb.parse_and_eval(var + " = -1")
        gdb.parse_and_eval("errno  = " + str(error_no))
    elif (etype == 2):
        # the returning value is NULL, errno is set to error number
        gdb.parse_and_eval(var + " = 0")
        gdb.parse_and_eval("errno  = " + str(error_no))
    elif (etype == 3):
        # the returning value of error number
        gdb.parse_and_eval(var + " = " + str(error_no));
    else:
        die("Can't match etype");
