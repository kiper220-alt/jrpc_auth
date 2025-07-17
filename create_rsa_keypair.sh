#!/usr/bin/bash
ssh-keygen -t rsa -b 4096 -m PEM -E SHA512 -f ./key/jwtRS512.pem -N "" && openssl rsa -in ./key/jwtRS512.pem -pubout -outform PEM -out ./key/jwtRS512.pem.pub
