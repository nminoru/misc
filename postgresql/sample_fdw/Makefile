MODULE_big = sample_fdw
OBJS = \
	sample_fdw.o \
	path_cost_size.o \
	import/src/backend/optimizer/plan/createplan.o

EXTENSION = sample_fdw
DATA = sample_fdw--1.0.sql

REGRESS = \
	aggregate \
	multi-join \
	subquery

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
