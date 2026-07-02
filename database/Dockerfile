FROM postgres:17

# Certs
COPY ssl/ca.crt       /etc/ssl/pg/ca.crt
COPY ssl/database.crt /etc/ssl/pg/database.crt
COPY ssl/database.key /etc/ssl/pg/database.key

# Config files
COPY postgresql.conf /etc/postgresql/postgresql.conf
COPY pg_hba.conf     /etc/postgresql/pg_hba.conf

# Init scripts
COPY initdb/ /docker-entrypoint-initdb.d/

# Fix permissions
RUN chmod 600 /etc/ssl/pg/database.key && chown postgres:postgres /etc/ssl/pg/database.key
RUN chown postgres:postgres /etc/postgresql/postgresql.conf && chown postgres:postgres /etc/postgresql/pg_hba.conf

EXPOSE 5432

CMD [ "postgres", "-c", "config_file=/etc/postgresql/postgresql.conf", "-c", "hba_file=/etc/postgresql/pg_hba.conf" ]