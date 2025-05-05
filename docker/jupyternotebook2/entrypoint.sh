#!/bin/sh

set -e

export GRANT_SUDO=yes
export DOCKER_STACKS_JUPYTER_CMD=notebook

/usr/bin/supervisord -c /app/supervisord.conf
