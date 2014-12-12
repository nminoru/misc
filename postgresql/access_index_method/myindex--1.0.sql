CREATE FUNCTION myindex_build(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_buildempty(internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_insert(internal, internal, internal, internal, internal, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_bulkdelete(internal, internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_vacuumcleanup(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_canreturn(internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION myindex_costestimate(internal, internal, internal, internal, internal, internal, internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_options(_text, bool)
RETURNS bytea
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_beginscan(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_rescan(internal, internal, internal, internal, internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_gettuple(internal, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_getbitmap(internal, internal)
RETURNS int8
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_endscan(internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_markpos(internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION myindex_restrpos(internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C VOLATILE STRICT;

INSERT INTO pg_am VALUES ('myindex',
       1, 0,
       false, false, false, false, true, false, false, false, false, false, false, 0,
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_insert'       ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_beginscan'    ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_gettuple'     ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_getbitmap'    ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_rescan'       ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_endscan'      ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_markpos'      ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_restrpos'     ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_build'        ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_buildempty'   ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_bulkdelete'   ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_vacuumcleanup'),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_canreturn'    ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_costestimate' ),
       (SELECT oid FROM pg_proc WHERE proname = 'myindex_options'));

CREATE OPERATOR CLASS boolean_ops DEFAULT FOR TYPE boolean USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS int2_ops DEFAULT FOR TYPE int2   USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS int4_ops DEFAULT FOR TYPE int4   USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS int8_ops DEFAULT FOR TYPE int8   USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS float4_ops DEFAULT FOR TYPE float4 USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS float8_ops DEFAULT FOR TYPE float8 USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS numeric_ops DEFAULT FOR TYPE numeric USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS str_ops DEFAULT FOR TYPE character USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS bytea_ops DEFAULT FOR TYPE bytea USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS name_ops DEFAULT FOR TYPE name USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS money_ops DEFAULT FOR TYPE money USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS uuid_ops DEFAULT FOR TYPE uuid USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS bit_ops DEFAULT FOR TYPE bit USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS varbit_ops DEFAULT FOR TYPE varbit USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS date_ops DEFAULT FOR TYPE date   USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS time_ops DEFAULT FOR TYPE time   USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS time_with_time_zone_ops DEFAULT FOR TYPE time with time zone   USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS timestamp_witi_time_zone DEFAULT FOR TYPE timestamp with time zone USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS interval_ops DEFAULT FOR TYPE interval USING myindex AS OPERATOR 1 =;
