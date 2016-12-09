\echo Use "CREATE EXTENSION whole_row_var_arg" to load this file. \quit

CREATE FUNCTION public.whole_row_var_arg(IN script text, IN dummy record)
RETURNS int
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;
