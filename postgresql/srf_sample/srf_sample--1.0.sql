CREATE FUNCTION public.show_diskstats1()
RETURNS TABLE(major int, minor int, diskname text, read_completed bigint, read_merged bigint, read_sectors bigint, read_time int, write_completed bigint, write_merged bigint, write_sectors bigint, write_time int, io int, io_time int, weighted_io_time int)
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION public.show_diskstats2()
RETURNS TABLE(major int, minor int, diskname text, read_completed bigint, read_merged bigint, read_sectors bigint, read_time int, write_completed bigint, write_merged bigint, write_sectors bigint, write_time int, io int, io_time int, weighted_io_time int)
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;
