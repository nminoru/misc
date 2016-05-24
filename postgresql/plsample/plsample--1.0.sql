CREATE FUNCTION plsample_call_handler()
RETURNS language_handler
AS 'MODULE_PATHNAME'
LANGUAGE C;

CREATE FUNCTION plsample_inline_handler(internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C;

CREATE FUNCTION plsample_validator(oid)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C;

CREATE PROCEDURAL LANGUAGE plsample
    HANDLER     plsample_call_handler
    INLINE      plsample_inline_handler
    VALIDATOR   plsample_validator;

COMMENT ON PROCEDURAL LANGUAGE plsample IS 'PL/sample procedural language';
