CREATE FUNCTION worker_test(IN workers int4)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;
