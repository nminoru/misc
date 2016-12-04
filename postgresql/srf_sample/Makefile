# srf_sample/Makefile

MODULE_big = srf_sample
OBJS = diskstats.o

EXTENSION = srf_sample
DATA = srf_sample--1.0.sql

REGRESS = diskstats

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
