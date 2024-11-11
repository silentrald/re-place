#!/bin/bash

DIR=$(pwd)

checkout_version () {
  printf "\n  > [%s] => %s\n\n" "$1" "$2"
  cd $1
  git checkout $2
  cd $DIR
}

printf "=== git submodule update --recursive --remote ===\n\n"
git submodule update --recursive --remote

printf "\n=== checking out the correct versions ===\n"
checkout_version external/asio tags/asio-1-32-0
checkout_version external/llhttp release/v9.2.1
checkout_version external/rapidjson 858451e5b7d1c56cf8f6d58f88cf958351837e53

