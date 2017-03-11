#! /bin/sh

if [ ! -f "$memCheckResult.txt" ]; then
  touch "$memCheckResult.txt"
fi

valgrind 
