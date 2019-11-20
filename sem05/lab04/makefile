app%: main%.o display.o
	gcc -o $@ $^

%.o: %.c %.h
	gcc -c -o $@ $<

%.o: %.c
	gcc -c -o $@ $<

clean:
	rm app* *.o 2>/dev/null
