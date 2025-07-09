#!/usr/bin/bash

set -o allexport
source .env

export QJSONRPC_DEBUG=1
export QT_ASSUME_STDERR_HAS_CONSOLE=1

function add_user_query() {
    username="$1"
    password="$2"
    passwordsum="$(echo -n "SOME_PASSWORD_SALT$username$password" | sha256sum | cut -d ' ' -f 1)"

    echo "INSERT INTO ${DATABASE_SCHEMA}.Users (username, password) VALUES('$username', '$passwordsum');"
}

function response_with_body() {
    echo $(curl -s -X POST\
        -H "Content-Type: application/json" \
        -H "Accept: application/json" \
        -d "$1" \
        http://localhost:7777/api)
}

function login_query() {
    username="$1"
    password="$2"
    
    REQUEST="
{
    \"jsonrpc\": "2.0",
    \"method\": \"auth.login\",
    \"params\": [\"$username\", \"$password\"],
    \"id\": \"$(date +%s%N)\"
}"

    echo "$REQUEST"

    response_with_body "$REQUEST"
    
    if [ $? -ne 0 ]; then 
        exit 1
    fi
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

./ServerArchJwt & pids=( $! )

sleep 1

# TODO: write test workflow
login_query admin admin

kill ${pids[0]}