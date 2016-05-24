# plsample/Makefile

MODULE_big = plsample
OBJS = plsample.o

EXTENSION = plsample
DATA = plsample--1.0.sql

# ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
# else
# subdir = contrib/myoptimizer
# top_builddir = ../..
# include $(top_builddir)/src/Makefile.global
# include $(top_srcdir)/contrib/contrib-global.mk
# endif
