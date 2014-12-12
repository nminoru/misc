source break_at_keyword.py
source rmitest.py

# Breakpoint 1
break main

run

# Breakpoint 2
python set_breakpoint("test1.c", 2)
commands
    silent
    python handle_breakpoint(2)
    continue
end

# continue from breakpoint 1
continue
