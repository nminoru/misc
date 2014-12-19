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

CREATE OPERATOR CLASS bool_ops DEFAULT FOR TYPE bool USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS bytea_ops DEFAULT FOR TYPE bytea USING myindex AS OPERATOR 1 =;
-- CREATE OPERATOR CLASS char_ops DEFAULT FOR TYPE char USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS name_ops DEFAULT FOR TYPE name USING myindex AS OPERATOR 1 =;
-- CREATE OPERATOR CLASS name_ops DEFAULT FOR TYPE name USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS int8_ops DEFAULT FOR TYPE int8 USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS int2_ops DEFAULT FOR TYPE int2 USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS int4_ops DEFAULT FOR TYPE int4 USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS text_ops DEFAULT FOR TYPE text USING myindex AS OPERATOR 1 =;
-- CREATE OPERATOR CLASS varchar_ops FOR TYPE text USING myindex AS OPERATOR 1 =;
-- CREATE OPERATOR CLASS var_pattern_ops FOR TYPE text USING myindex AS OPERATOR 1 =;
-- CREATE OPERATOR CLASS text_pattern_ops FOR TYPE text USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS oid_ops DEFAULT FOR TYPE oid USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS tid_ops DEFAULT FOR TYPE tid USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS oidvector_ops DEFAULT FOR TYPE oidvector USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS float4_ops DEFAULT FOR TYPE float4 USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS float8_ops DEFAULT FOR TYPE float8 USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS abstime_ops DEFAULT FOR TYPE abstime USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS reltime_ops DEFAULT FOR TYPE reltime USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS tinterval DEFAULT FOR TYPE tinterval USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS money_ops DEFAULT FOR TYPE money USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS macaddr_ops DEFAULT FOR TYPE macaddr USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS inet_ops DEFAULT FOR TYPE inet USING myindex AS OPERATOR 1 =;
-- CREATE OPERATOR CLASS cidr_ops FOR TYPE cidr USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS bpchar_ops DEFAULT FOR TYPE bpchar USING myindex AS OPERATOR 1 =;
-- CREATE OPERATOR CLASS bpchar_pattern_ops FOR TYPE bpchar USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS date_ops DEFAULT FOR TYPE date USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS time_ops DEFAULT FOR TYPE time USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS timestamp_ops DEFAULT FOR TYPE timestamp USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS timestamptz_ops DEFAULT FOR TYPE timestamptz USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS interval_ops DEFAULT FOR TYPE interval USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS timetz_ops DEFAULT FOR TYPE timetz USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS bit_ops DEFAULT FOR TYPE bit USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS varbit_ops DEFAULT FOR TYPE varbit USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS numeric_ops DEFAULT FOR TYPE numeric USING myindex AS OPERATOR 1 =;
-- CREATE OPERATOR CLASS record_image_ops FOR TYPE record USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS record_ops DEFAULT FOR TYPE record USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS array_ops DEFAULT FOR TYPE anyarray USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS uuid_ops DEFAULT FOR TYPE uuid USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS pg_lsn_ops DEFAULT FOR TYPE pg_lsn USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS enum_ops DEFAULT FOR TYPE anyenum USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS tsvector_ops DEFAULT FOR TYPE tsvector USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS tsquery_ops DEFAULT FOR TYPE tsquery USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS jsonb_ops DEFAULT FOR TYPE jsonb USING myindex AS OPERATOR 1 =;
CREATE OPERATOR CLASS range_ops DEFAULT FOR TYPE anyrange USING myindex AS OPERATOR 1 =;
