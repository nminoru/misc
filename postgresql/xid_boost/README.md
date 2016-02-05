xid_boost
=========

xid_boost is a PostgreSQL extension in order to accelerate increasing Transaction ID.

Requirements
============

- PostgreSQL 9.4 or later


Building
========

First, acquire the source code by cloning the git repository.

    $ git clone https://github.com/nminoru/misc

Next, input the following commadns.

    $ cd misc/postgresql/xid_boost
    $ make
    $ make install

Prepare
=======

Add the following line into postgresql.conf.

    shared_preload_libraries = 'xid_boost'

Usage
=====

    CREATE EXTENSION xid_boost;

    SELECT xid_boost(N);
