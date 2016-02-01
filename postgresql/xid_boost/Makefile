# xid_boost/Makefile

MODULE_big = xid_boost
OBJS = xid_boost.o

EXTENSION = xid_boost
DATA = xid_boost--1.0.sql xid_boost--unpackaged--1.0.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
