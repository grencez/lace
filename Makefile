
default: all

CC = gcc

CONFIG += ansi
CONFIG += debug

CxPath = ../cx
BinPath = ../bin
PfxBldPath = ../lace-bld
BldPath = $(PfxBldPath)/lace

ExeList = lace add best-match xpipe void cat1 ssh-all ujoin \
		  godo waitdo \
		  chatty
Deps := $(ExeList)
ExeList := $(addprefix $(BinPath)/,$(ExeList))
Objs = $(addprefix $(BldPath)/,$(addsuffix .o,$(Deps)))

include $(CxPath)/include.mk

all: $(ExeList)

$(BinPath)/lace: $(BldPath)/lace.o \
	$(addprefix $(CxBldPath)/, fileb.o bstree.o rbtree.o syscx.o)
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/add: $(BldPath)/add.o \
	$(addprefix $(CxBldPath)/, fileb.o syscx.o)
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/best-match: $(BldPath)/best-match.o \
	$(addprefix $(CxBldPath)/, fileb.o syscx.o)
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/xpipe: $(BldPath)/xpipe.o
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/void: $(BldPath)/void.o
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/cat1: $(BldPath)/cat1.o
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/ssh-all: $(BldPath)/ssh-all.o \
	$(addprefix $(CxBldPath)/, fileb.o ospc.o syscx.o)
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/ujoin: $(BldPath)/ujoin.o \
	$(addprefix $(CxBldPath)/, bstree.o fileb.o ospc.o rbtree.o syscx.o)
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/godo: $(BldPath)/godo.o \
	$(addprefix $(CxBldPath)/, fileb.o syscx.o)
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/waitdo: $(BldPath)/waitdo.o \
	$(addprefix $(CxBldPath)/, fileb.o syscx.o)
	$(CC) $(CFLAGS) -o $@ $^

$(BinPath)/chatty: $(BldPath)/chatty.o \
	$(addprefix $(CxBldPath)/, fileb.o ospc.o syscx.o)
	$(CC) $(CFLAGS) $^ -o $@ -lrt


$(BldPath)/lace.o: $(BldPath)/lace.c
	$(CC) -c $(CFLAGS) \
		"-DUtilBin=\"$(abspath $(BinPath))\"" \
		$< -o $@

$(BldPath)/chatty.o: $(BldPath)/chatty.c
	$(CC) -c $(filter-out -ansi,$(CFLAGS)) $< -o $@

.PHONY: clean
clean:
	rm -fr $(PfxBldPath)
	rm -f $(ExeList)

