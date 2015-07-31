-- PostgreSQL 9.4 に含まれているシステム由来の regproc を洗い出し、純粋に関数と呼べる pg_proc エントリを洗い出す。

DROP TABLE IF EXISTS sys_func_table;

CREATE TEMPORARY TABLE test (funcoid oid);

INSERT INTO test (funcoid) SELECT unnest(ARRAY[aggfnoid, aggtransfn, aggfinalfn, aggmtransfn, aggminvtransfn, aggmfinalfn]) FROM pg_aggregate;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[aminsert, ambeginscan, amgettuple, amgetbitmap, amrescan, amendscan, ammarkpos, amrestrpos, ambuild, ambuildempty, ambulkdelete, amvacuumcleanup, amcanreturn, amcostestimate, amoptions]) FROM pg_am;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[amproc]) FROM pg_amproc;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[castfunc]) FROM pg_cast;
--  ascii_to_mic など
INSERT INTO test (funcoid) SELECT unnest(ARRAY[conproc]) FROM pg_conversion;
INSERT INTO test (funcoid) SELECT evtfoid FROM pg_event_trigger;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[fdwhandler, fdwvalidator]) FROM pg_foreign_data_wrapper;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[lanplcallfoid, laninline, lanvalidator]) FROM pg_language;
INSERT INTO test (funcoid) SELECT tgfoid FROM pg_trigger;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[oprcode, oprrest, oprjoin]) FROM pg_operator;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[rngcanonical, rngsubdiff]) FROM pg_range;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[prsstart, prstoken, prsend, prsheadline, prslextype]) FROM pg_ts_parser;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[tmplinit, tmpllexize]) FROM pg_ts_template;
INSERT INTO test (funcoid) SELECT unnest(ARRAY[typinput, typoutput, typreceive, typsend, typmodin, typmodout, typanalyze]) FROM pg_type;

CREATE TABLE sys_func_table (funcoid oid UNIQUE);

INSERT INTO sys_func_table SELECT distinct funcoid FROM test WHERE funcoid > 0 ORDER BY funcoid;

SELECT pg_proc.oid, *, EXISTS (SELECT funcoid FROM sys_func_table WHERE pg_proc.oid = sys_func_table.funcoid) FROM pg_proc;
