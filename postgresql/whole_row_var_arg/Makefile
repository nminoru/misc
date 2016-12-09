MODULE_big = whole_row_var_arg
OBJS = whole_row_var_arg.o

EXTENSION = whole_row_var_arg
DATA = whole_row_var_arg--1.0.sql

REGRESS = base transform1

EXTRA_CLEAN = sql/transform1.sql expected/transform1.out

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

ifdef PG_PORT
    EXTRA_REGRESS_OPTS = --port=$(PG_PORT)
endif

