#!/bin/sh

arg0=$(basename $0 .sh)
error()
{
    echo "$arg0: $@" 1>&2
    exit 1
}

file="engine-output-example.txt"

if [ ! -f $file ]
  then
    error "$file not found"
fi

while read line; do
    echo "$line"
    sleep 0.1
done < $file
