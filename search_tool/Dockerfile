### FLASK APP ###
FROM python:3
LABEL maintainer="Alexander Lercher"

RUN apt-get update

EXPOSE 5000

WORKDIR /app

COPY requirements.txt /app/
RUN pip install -r requirements.txt

ENV CONTENT_SEARCH_DB_HOSTNAME='host.docker.internal'

COPY static/ /app/static/
COPY flask_main.py /app/
COPY repo.py /app/
COPY templates/ /app/templates/
RUN chmod a+x flask_main.py

CMD ["python", "./flask_main.py"]
