CREATE FUNCTION public.sql_exec_twice(IN sql text) RETURNS SETOF record
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;
exec_sql_twice--1.0.sql