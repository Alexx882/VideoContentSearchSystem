### DATABASE ### 
FROM postgres
LABEL maintainer="Alexander Lercher"

EXPOSE 5432

ENV POSTGRES_PASSWORD=contentsearch
ENV PGPASSWORD=contentsearch

COPY database_dump /db/database_dump

COPY restore_db.sh /docker-entrypoint-initdb.d/restore_db.sh
RUN chmod a+x /docker-entrypoint-initdb.d/restore_db.sh