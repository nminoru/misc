CREATE FUNCTION public.show_shm_test()
RETURNS int4
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;
