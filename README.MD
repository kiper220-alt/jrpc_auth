[Russian](/README_RU.MD)

## Technical description of the authentication system based on Json-RPC

This repository contains an example of implementing an authentication system based on Json-RPC.
There are 2 examples in total:

- [`examples/jrpc_auth`](/examples/jrpc_auth/README_RU.MD) &mdash; An example of implementing an authentication system
  based on Json-RPC. A single JWT token with an infinite validity period is used as a token.
- [`examples/jrpc_double_token_auth`](/examples/jrpc_double_token_auth/README_RU.MD) &mdash; similar implementation, but
  a pair of JWT tokens (`access` and `refresh`) with a limited validity period are used as a token.

Common functionality is implemented in [`examples/common`](/examples/common).
