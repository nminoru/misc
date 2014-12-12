#! /usr/bin/perl

my $func_name;
my $invoke_string;

while (<>) {
    @part = split (/\t/, $_);
    $func_name     = "void Assembler::$part[0](";
    $invoke_string = "_impl->$part[0](";

    if ($part[1]) {
	$func_name .= "$part[1]"; 
	if ($part[1] =~ /(\S+)\s+(\S+)/) {
	    $invoke_string .= $2;
	}

	for ($i=2 ; $i<4 ; $i++) {
	    if ($part[$i]) {
		if ($part[$i] =~ /(\S+)\s+(\S+)/) {
		    $invoke_string .= ", " . $2;
		}
		$func_name .= ", $part[$i]";
	    } else {
		last;
	    }
	}
    }

    $func_name     .= ");\n";
    $invoke_string .= ");\n";

    if ($func_name =~ /Any/) {
        $func_name_tmp = $func_name;
        $func_name_tmp =~ s/Any/Reg/;
        print $func_name_tmp;
        $func_name_tmp = $func_name;
        $func_name_tmp =~ s/Any/Address/;
        print $func_name_tmp;
    } else {
        print $func_name;
    }
}


