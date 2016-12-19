# contrib/myindex/Makefile

MODULE_big = myindex
OBJS = myindex.o

EXTENSION = myindex
DATA = myindex--1.0.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
