#!/bin/bash

assert(){
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

assert 29 '3 * 3 + 4*5'
assert 30 '(20-5)*2' 
assert 47 '5+6*7'
assert 15 '5*(9-6)'
assert 4 '(3+5)/2'
assert 10 '-10+20'
assert 10 '- -10'
assert 10 '- - +10'

echo OK
