# lock_test/Makefile

MODULE_big = lock_test
OBJS = lock_test.o

EXTENSION = lock_test
DATA = lock_test--1.0.sql lock_test--unpackaged--1.0.sql

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
