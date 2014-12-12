#! /usr/bin/perl

my $func_name;
my $invoke_string;
my $shift_flag = 0;


while (<>) {
    ($func, $arg1, $arg2, $arg3, $size, $prefix, $opcode, $opfield, $rm, $operand, $imm, $option)
	= split (/\t/, $_);

    ($shift_flag) = ($arg2 =~ /(shift)/);

    my $output_lines = '';

    $func_name = "void Assembler::$func(";

    if ($arg1) {
	$func_name .= $arg1;
    }
    if ($arg2) {
	$func_name .= ", " . $arg2;
    }
    if ($arg3) {
	$func_name .= ", " . $arg3;
    }
    $func_name     .= ")\n";

    $output_lines .= $func_name;
    $output_lines .= "{\n";
    $output_lines .= "writeBegin();\n";

    if ($prefix) { 
	$output_lines .= "setPrefix($prefix);\n";
    }

    if ($size) {
	$output_lines .= "setSize($size);\n";
    }

    if ($shift_flag) {
	$output_lines .= "if ($imm == 1) {\n";
	$output_lines .= "setOpcode($option);\n";
	$output_lines .= "setRM($rm);\n";	
	$output_lines .= "setOperand($operand);\n";
	$output_lines .= "} else {\n";
    }

    $output_lines .= "setOpcode($opcode);\n";
    if ($opfield ne '') {
        $output_lines .= "setOpfield($opfield);\n";
    }
    if ($rm) {
        $output_lines .= "setRM($rm);\n";
    }
    if ($operand) {
        $output_lines .= "setOperand($operand);\n";
    }
    if ($imm) {
	if ($imm eq 'imm8') {
            $output_lines .= "setImm8($imm);\n";
	} elsif ($imm eq 'imm16') {
            $output_lines .= "setImm16($imm);\n";
	} elsif ($imm eq 'imm32') {
            $output_lines .= "setImm32($imm);\n";	    
	} elsif ($imm eq 'imm64') {
            $output_lines .= "setImm64($imm);\n";
	} elsif ($imm eq 'shift') {
            $output_lines .= "setImm8($imm);\n";	    
	}
    }

    if ($shift_flag) {
        $output_lines .= "}\n";
    }
#    print "if (shift == 1) {\n";
#    print "}\n;"
    $output_lines .= "writeEnd();\n";
    $output_lines .= "}\n\n";

    if ($output_lines =~ /(Any)/) {
        $tmp = $output_lines;
        $tmp =~ s/Any/Reg/;
        print $tmp;
        $tmp = $output_lines;
        $tmp =~ s/Any/Address/;
        print $tmp;
    } else {
        print $output_lines;
    }    
}


