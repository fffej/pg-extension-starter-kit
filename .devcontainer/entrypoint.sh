#!/bin/bash
set -e

PG_BIN="/usr/lib/postgresql/17/bin"
PG_DATA="/var/lib/postgresql/data"
PG_LOG="/var/lib/postgresql/logfile"

# Initialize the PostgreSQL data directory if it's empty
if [ -z "$(ls -A $PG_DATA)" ]; then
    echo "Initializing PostgreSQL database..."
    sudo -u postgres $PG_BIN/initdb -D $PG_DATA
fi

# Ensure PostgreSQL socket directory exists and has correct permissions
sudo mkdir -p /var/run/postgresql
sudo chown postgres:postgres /var/run/postgresql

# Start PostgreSQL service
sudo -u postgres $PG_BIN/pg_ctl -D $PG_DATA -l $PG_LOG start

echo "PostgreSQL is ready."

# Keep the container alive
exec "$@"
