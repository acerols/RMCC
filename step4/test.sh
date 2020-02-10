#!/bin/bash

try(){
	expected="$1"
	input="$2"

	./rmcc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" != "$expected" ]; then
		echo "$input expected, but got $actual"
		exit 1
	fi
}

try 27 '19 + 3 + 2 -1 +4'
try 42 '53-12+1' 

echo OK
