MODULE_big = sample_fdw
OBJS = sample_fdw.o createplan.o

EXTENSION = sample_fdw
DATA = sample_fdw--1.0.sql

REGRESS = test_sample_fdw

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
