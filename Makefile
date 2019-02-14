 rmcc: rmcc.c

test: rmcc
	./rmcc -test
	./test.sh

clean:
	rm -f rmcc *. *~ tmp*
