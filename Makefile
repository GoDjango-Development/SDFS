cflags = -D_FILE_OFFSET_BITS=64 -I ./include 
libs = -lpthread 

#Release profile

obj = release/obj/main_test.o release/obj/sdfs_fslayer.o 
hdr = include/sdfs_fslayer.h include/sdfs_defs.h

release_bin = release/sdfs_test
CCX = gcc -o
CC = gcc -c
release_command = $(CCX) release/sdfs_test $(obj) $(ldflags) $(libs)

release: $(obj) $(hdr)
	$(release_command)

release/obj/main_test.o: src/main_test.c
	$(CC) src/main_test.c -o release/obj/main_test.o $(cflags) 

release/obj/sdfs_fslayer.o: src/sdfs_fslayer.c include/sdfs_fslayer.h
	$(CC) src/sdfs_fslayer.c -o release/obj/sdfs_fslayer.o $(cflags) 
	
run: release 
ifneq ("$(wildcard $(release_bin))","")
	$(release_bin)
else
	$(release_command)
	$(release_bin)
endif

.PHONY: rebuild

rebuild: 
	$(MAKE) clean
	$(MAKE) release
	
# Debug profile

dbg = debug/obj/main_test.o debug/obj/sdfs_fslayer.o

debug_bin = debug/sdfs_test
CCGX = gcc -g -DDEBUG -o
CCG = gcc -g -DDEBUG -c
debug_command = $(CCGX) debug/sdfs_test $(dbg) $(ldflags) $(libs)
	
debug: $(dbg) $(hdr)
	$(debug_command)

debug/obj/main_test.o: src/main_test.c
	$(CCG) src/main_test.c -o debug/obj/main_test.o $(cflags) 

debug/obj/sdfs_fslayer.o: src/sdfs_fslayer.c include/sdfs_fslayer.h
	$(CCG) src/sdfs_fslayer.c -o debug/obj/sdfs_fslayer.o $(cflags) 
	
run_debug: debug
ifneq ("$(wildcard $(debug_bin))","")
	gdb $(debug_bin)
else
	$(debug_command)
	gdb $(debug_bin)
endif

.PHONY: rebuild_debug
	
rebuild_debug: 
	$(MAKE) clean
	$(MAKE) debug

#Others rules

.PHONY: prepare clean

prepare:
	mkdir -p debug release debug/obj release/obj

clean:
	rm -R -f $(obj) $(dbg) release/obj/* debug/obj/* release/sdfs_test debug/sdfs_test

