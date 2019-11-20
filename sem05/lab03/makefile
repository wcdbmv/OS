all:
	gcc main.c -o main

hard-link:
	ln main.c main_hl.c

soft-link:
	ln -s main.c main_sl.c

# pipe:
#	-- shell 1 --
#	mknod pipe p
#	echo Info >pipe
#	-- shell 2 --
#	tee file.txt <pipe

# renice:
#	renice 10 <PID>

ps:
	ps -al

ls:
	ls -ial

clean:
	rm main main_hl.c main_sl.c 2>/dev/null
