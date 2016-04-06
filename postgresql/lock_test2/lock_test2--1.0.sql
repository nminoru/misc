CREATE FUNCTION lock_test2(IN table_oid regclass, IN lock_type text, IN sleep_time float8)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;
