 SRC = src/
 INCLUDE = include/
 CFLAGS = -I$(INCLUDE)
 LDFLAGS = -std=c11 -g -static
 SRCS = codegen.c container.c parse.c rmcc.c
 OBJS = codegen.o container.o parse.o rmcc.o tokenize.o
 
all: rmcc

%.o: $(SRC)%.c
	$(CC) -c -o object/$@ $(CFLAGS) $<

rmcc: $(OBJS)
	$(CC) -o $@ $(addprefix object/, $^) $(LDFLAGS)
	

	

test: rmcc
	./test.sh

clean:
	rm -f rmcc *. *~ tmp* object/*.o
