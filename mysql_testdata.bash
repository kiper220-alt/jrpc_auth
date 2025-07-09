#!/usr/bin/bash

if [ -z "$DATABASE_HOST" ]; then
    export DATABASE_HOST=127.0.0.1
fi
if [ -z "$DATABASE_PORT" ]; then
    export DATABASE_PORT=5432
fi
if [ -z "$DATABASE_SCHEMA" ]; then
    export DATABASE_SCHEMA=public
fi
if [ -z "$DATABASE_NAME" ]; then
    export DATABASE_NAME=db
fi 
if [ -z "$DATABASE_USER" ]; then
    export DATABASE_USER=db
fi
if [ -z "$DATABASE_PASSWORD" ]; then
    export DATABASE_PASSWORD=db
fi 
if [ -z "$DATABASE_DRIVER" ]; then
    export DATABASE_DRIVER=QPSQL
fi

function add_user_query() {
    username="$1"
    password="$2"
    passwordsum="$(echo -n "SOME_PASSWORD_SALT$username$password" | sha256sum | cut -d ' ' -f 1)"

    echo "INSERT INTO ${DATABASE_SCHEMA}.Users (username, password) VALUES('$username', '$passwordsum');"
}

function login_query() {
    username="$1"
    password="$2"
    echo {"id": 0, "jsonrpc": "2.0","method": "auth.login", "params": [$username, $password]};
}

function call_psql() {
    psql postgresql://${DATABASE_USER}:${DATABASE_PASSWORD}@${DATABASE_HOST}:${DATABASE_PORT}/${DATABASE_NAME} -c "$1"
}

call_psql \
"CREATE SCHEMA IF NOT EXISTS ${DATABASE_SCHEMA};

DROP TABLE IF EXISTS ${DATABASE_SCHEMA}.Users;

CREATE TABLE ${DATABASE_SCHEMA}.Users (  
    id int NOT NULL PRIMARY KEY GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 CACHE 1 ),
    username VARCHAR(255) NOT NULL,
    password VARCHAR(2048) NOT NULL
);"

call_psql \
"$(add_user_query admin admin)"

call_psql \
"$(add_user_query user user)"

call_psql \
"$(add_user_query user2 user2)"

cd build

./ServerArchJwt &

sleep 1

# TODO: write test workflow