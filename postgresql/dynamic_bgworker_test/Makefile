# dynamic_bgworker_test/Makefile

MODULE_big = dynamic_bgworker_test
OBJS = dynamic_bgworker_test.o

EXTENSION = dynamic_bgworker_test
DATA = dynamic_bgworker_test--1.0.sql dynamic_bgworker_test--unpackaged--1.0.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
