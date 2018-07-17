.PHONY:clean

open01:open-01.c
	$(CC) -o $@  $< 

open02:open-02.c
	$(CC) -o $@  $< 

close:close.c
	$(CC) -o $@  $< 

read01:read-01.c
	$(CC) -o $@  $< 

write01:write-01.c
	$(CC) -o $@  $< 

lseek01:lseek-01.c
	$(CC) -o $@  $< 

lseek02:lseek-02.c
	$(CC) -o $@  $< 

fstat:fstat.c
	$(CC) -o $@  $< 


mmap:mmap.c
	$(CC) -o $@  $< 

fcntl01:fcntl-01.c
	$(CC) -o $@  $< 

fcntl02:fcntl-02.c
	$(CC) -o $@  $< 


clean:
	-$(RM) *.o
	-$(RM) open01 open02 close read01 write01 lseek01 \
        lseek02 fstat mmap fcntl01 fcntl02
