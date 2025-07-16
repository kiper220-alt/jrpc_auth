#!/bin/bash

# include .env
set -a
source .env
set +a

function extract_json() {
  local json="$1" path="$2"
  echo "$json" | jq -r "$path"
}

json_config=$(cat "$JRPC_AUTH_CONFIG_PATH")
DATABASE_HOST=$(extract_json "$json_config" '.user.host' | tr -d '\n')
DATABASE_PORT=$(extract_json "$json_config" '.user.port' | tr -d '\n')
DATABASE_SCHEMA=$(extract_json "$json_config" '.user.schema' | tr -d '\n')
DATABASE_NAME=$(extract_json "$json_config" '.user.name' | tr -d '\n')
DATABASE_USER=$(extract_json "$json_config" '.user.user' | tr -d '\n')
DATABASE_PASSWORD=$(extract_json "$json_config" '.user.password' | tr -d '\n')
SOME_PASSWORD_SALT=$(extract_json "$json_config" '.user.salt' | tr -d '\n')

function psql_make_url() {
  echo "postgres://$DATABASE_USER:$DATABASE_PASSWORD@$DATABASE_HOST:$DATABASE_PORT/$DATABASE_NAME"
}

# call psql -c "...", where ... is vaargs of function
function pqsql_make_request() {
  local query="$*"
  psql "$(psql_make_url)" -c "$query"
}

function get_salt() {
  if [ -z "$SOME_PASSWORD_SALT" ]; then
    echo "SOME_PASSWORD_SALT"
  else
    echo "$SOME_PASSWORD_SALT"
  fi
}

function get_schema() {
  if [ -z "$DATABASE_SCHEMA" ]; then
    echo "SCHEMA"
  else
    echo "$DATABASE_SCHEMA"
  fi
}

function calculate_hash() {
  local password salt

  password="$1"
  salt=$(get_salt)

  printf "%s" "$salt$password" | sha256sum | awk '{print $1}'
}

function create_schema() {
  local schema
  schema=$(get_schema)
  pqsql_make_request "CREATE SCHEMA IF NOT EXISTS $schema"
}

function create_users_table() {
  local schema
  schema=$(get_schema)
  pqsql_make_request "CREATE TABLE IF NOT EXISTS $schema.users (
  id int NOT NULL PRIMARY KEY GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 CACHE 1 ),
  username VARCHAR(255),
  password VARCHAR(255)
)"
}

function drop_users_table() {
  local schema
  schema=$(get_schema)
  pqsql_make_request "DROP TABLE IF EXISTS $schema.users"
}

function add_user() {
  local username password
  username="$1"
  password=$(calculate_hash "$2")
  schema=$(get_schema)
  pqsql_make_request "INSERT INTO $schema.users (username, password) VALUES ('$username', '$password')"
}

# make from $1... "[\"$1\", \"$2\", \"$3\", ...]"
to_json_array() {
    local args=("$@")
    local result="["
    local i
    for ((i=0; i<${#args[@]}; i++)); do
        result+="\"${args[i]}\""
        [ $i -lt $((${#args[@]}-1)) ] && result+=", "
    done
    result+="]"
    echo "$result"
}

# Extract token from JSON-RPC response
#
# $1 - JSON string
#
# Returns the token value or empty string if not found
function extract_token() {
  local json="$1"
  echo "$json" | jq -r '.result.token // ""'
}

# Extract token from JSON-RPC response
#
# $1 - JSON string
#
# Returns the token value or empty string if not found
function extract_result() {
  local json="$1"
  echo "$json" | jq -r '.result'
}

function extract_username_identity() {
  local json="$1"
  echo "$json" | jq -r '.result.username'
}

function extract_error() {
  local json="$1"
  echo "$json" | jq -r '.result.error'
}

# Perform a JSON-RPC request to the server
#
# $1 - method name
# $2... - method parameters
#
# Returns a JSON response from the server
function json_rpc_request() {
  local method params request
  method="$1"
  shift
  params=$(to_json_array "$@")
  request="{\"jsonrpc\": \"2.0\", \"method\": \"$method\", \"params\": $params, \"id\": 1}"

  curl -s -X POST \
    -H 'Content-Type: application/json' \
    -H 'Accept: application/json' \
    --max-time 10 \
    --data-raw "$request" \
    'http://127.0.0.1:7777'
}

create_schema
drop_users_table
create_users_table
add_user "admin" "admin"

token=$(extract_token "$(json_rpc_request "auth.login" "admin" "admin")")
echo "Token: $token"
echo "Test checkAuth with valid token: $(extract_result "$(json_rpc_request "auth.checkAuth" "$token")")"
echo "Test checkAuth with invalid token: $(extract_result "$(json_rpc_request "auth.checkAuth" "badtoken")")"
echo "Test getIdentity with valid token: $(extract_username_identity "$(json_rpc_request "auth.getIdentity" "$token")")"
echo "Logout: $(extract_result "$(json_rpc_request "auth.logout" "$token")")"
echo "Test checkAuth after logout: $(extract_result "$(json_rpc_request "auth.checkAuth" "$token")")"
echo "Test getIdentity after logout: $(extract_error "$(json_rpc_request "auth.getIdentity" "$token")")"

