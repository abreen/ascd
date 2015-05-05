CC = cc
OBJECTS = main.o options.o help.o db.o handlers.o logging.o lookup.o
CFLAGS = -g3 -ggdb
LDFLAGS = -g3 -ggdb -L/home/abreen/lib -lascd -lsqlite3 -lfann -lgmp
CLEAN = ascd *.[ch]~ *.o */*.[ch]~ */*.o statements.h ascd.sock

ascd: $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

db.o:
	python3 make_statements.py sql/*.sql
	$(CC) db.c $(CFLAGS) -c -o db.o

%.o: %.c ascd.h libascd.h
	$(CC) $< $(CFLAGS) -c -o $@

.PHONY: all clean

clean:
	$(RM) $(wildcard $(CLEAN))
