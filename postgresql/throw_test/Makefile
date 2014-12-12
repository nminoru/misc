# throw_test/Makefile

MODULE_big = throw_test
OBJS = throw_test.o

EXTENSION = throw_test
DATA = throw_test--1.0.sql throw_test--unpackaged--1.0.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
