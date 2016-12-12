
############################################################################
# project
############################################################################
EXEC=fbcamviewer

DESTDIR = /usr/bin

#
#	sub-directories and source files
#
SRC= fb.c

############################################################################
# C compiler & linker flags 
############################################################################
CFLAGS=-Wall -g -O2 -mtune=cortex-a8 -march=armv7-a -ftree-vectorize -fomit-frame-pointer -mfloat-abi=softfp -std=c99
#CFLAGS=-Wall -g -O1 -fomit-frame-pointer 
LDFLAGS=

CC=sb2 gcc
#CC=gcc

############################################################################
# building rules
#
DEPDIR = .deps


OBJS= $(SRC:%.c=%.o)


all:
	mkdir -p $(DEPDIR)
	make $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
  
%.o: %.c
	@$(CC) -MM $(CFLAGS) -o $(DEPDIR)/$*.d $<
	$(CC) -c $(CFLAGS) $<

-include $(SRC:%.c=$(DEPDIR)/%.d)

  
#
#	install
#
install:
	scp $(EXEC) root@192.168.0.2:$(DESTDIR)/

#
#	uninstall
#
uninstall:

#
#	clean up everything
#
clean:
	rm *.o $(DEPDIR)/*.d $(EXEC)
	
############################################################################

