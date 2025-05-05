FROM jupyter/scipy-notebook:python-3.11.6

USER root

RUN apt update && apt-get install -yq --no-install-recommends \
        gosu \
        supervisor
        
USER $NB_UID

COPY ./jupyter_server_config.py /etc/jupyter/jupyter_server_config.py
COPY --chown=jovyan:users ./entrypoint.sh    /app/entrypoint.sh
COPY --chown=jovyan:users ./supervisord.conf /app/supervisord.conf
RUN chmod u+x /app/entrypoint.sh

USER root

ENV GRANT_SUDO=yes

ENTRYPOINT ["/app/entrypoint.sh"]
