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

try 29 '3 * 3 + 4*5'
try 30 '(20-5)*2' 

echo OK
