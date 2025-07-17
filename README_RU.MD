[English](/README.MD)

## Техническое описание системы аутентификации на основе Json-RPC

В данном репозитории содержится пример реализации системы аутентификации на основе Json-RPC.
Всего здесь представлено 2 примера:

- [`examples/jrpc_auth`](/examples/jrpc_auth/README_RU.MD) &mdash; пример реализации системы аутентификации на основе
  Json-RPC. в качестве токена используется один JWT токен с бесконечным сроком действия.
- [`examples/jrpc_double_token_auth`](/examples/jrpc_double_token_auth/README_RU.MD) &mdash; аналогичная реализация, но
  в качестве токена используется пара JWT токенов(`access` и `refresh`) с ограниченным сроком действия.

Общая функциональность реализована в [`examples/common`](/examples/common).
