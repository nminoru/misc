FROM jupyter/scipy-notebook:python-3.11.6

RUN pip install papermill nbclient

COPY ./nbclient.py /app/nbclient.py

RUN python /app/nbclient.py

