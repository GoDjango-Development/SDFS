cflags = -D_FILE_OFFSET_BITS=64 -I ./include 
libs = -lpthread 

#Release profile

obj = release/obj/main_test.o 

release_bin = release/sdfs_test
CCX = gcc -o
CC = gcc -c

.PHONY: release run rebuild

run: release 
	$(release_bin)

rebuild: 
	$(MAKE) clean
	$(MAKE) release
	
release: $(obj)
	$(CCX) release/sdfs_test $(obj) $(ldflags) $(libs)

release/obj/main_test.o: src/main_test.c
	$(CC) src/main_test.c -o release/obj/main_test.o $(cflags) 


# Debug profile

dbg = debug/obj/main_test.o

debug_bin = debug/sdfs_test
CCGX = gcc -g -DDEBUG -o
CCG = gcc -g -DDEBUG -c
	
.PHONY: debug run_debug rebuild_debug

run_debug: debug
	gdb $(debug_bin)
	
rebuild_debug: 
	$(MAKE) clean
	$(MAKE) debug
	
debug: $(dbg)
	$(CCGX) debug/sdfs_test $(dbg) $(ldflags) $(libs)

debug/obj/main_test.o: src/main_test.c
	$(CCG) src/main_test.c -o debug/obj/main_test.o $(cflags) 

#Others rules

prepare:
	mkdir -p debug release debug/obj release/obj

clean:
	rm -R -f $(obj) $(dbg) release/obj/* debug/obj/* release/sdfs_test debug/sdfs_test

