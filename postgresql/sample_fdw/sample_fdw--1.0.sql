\echo Use "CREATE EXTENSION sample_fdw" to load this file. \quit

CREATE FUNCTION sample_fdw_handler()
RETURNS fdw_handler
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION sample_fdw_validator(text[], oid)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FOREIGN DATA WRAPPER sample_fdw
  HANDLER sample_fdw_handler
  VALIDATOR sample_fdw_validator;
