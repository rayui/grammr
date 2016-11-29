SRC_DIR=src/
BUILD_DIR=build/
LIB_DIR=include/
TEXT_DIR=text/
DATA_DIR=data/
DIST_DIR=dist/
CC65_DIR:=$(shell dirname `which cc65`)/..

all: grammr create_image

create_image:
	c1541 -format "grammr,01" d64 $(DIST_DIR)grammr.d64
	c1541 -attach $(DIST_DIR)grammr.d64 -write $(BUILD_DIR)grammr
	c1541 -attach $(DIST_DIR)grammr.d64 -write $(DATA_DIR)data.pet

grammr: io.o error.o lexer.o parser.o interpreter.o locations.o items.o instruction.o utils.o config_parser_new.o\
	actions.o text.o
	cc65 -O -t c64 --static-locals $(SRC_DIR)main.c -o $(BUILD_DIR)main.s
	ca65 $(BUILD_DIR)main.s -o $(BUILD_DIR)grammr.o
	ld65 -o $(BUILD_DIR)grammr -t c64 $(BUILD_DIR)grammr.o $(BUILD_DIR)error.o $(BUILD_DIR)lexer.o \
		$(BUILD_DIR)parser.o $(BUILD_DIR)interpreter.o $(BUILD_DIR)locations.o $(BUILD_DIR)items.o \
		$(BUILD_DIR)actions.o \
		$(BUILD_DIR)config_parser_new.o $(BUILD_DIR)instruction.o $(BUILD_DIR)text.o \
		$(BUILD_DIR)utils.o $(BUILD_DIR)io.o $(CC65_DIR)/lib/c64.lib

io.o: text.o
	cc65 -O -t c64 --static-locals $(SRC_DIR)io.c $(BUILD_DIR)text.o -o $(BUILD_DIR)io.s
	ca65 $(BUILD_DIR)io.s -o $(BUILD_DIR)io.o

error.o: io.o
	cc65 -O -t c64 --static-locals $(SRC_DIR)error.c $(BUILD_DIR)text.o -o $(BUILD_DIR)error.s
	ca65 $(BUILD_DIR)error.s -o $(BUILD_DIR)error.o

config_parser_new.o: io.o instruction.o
	cc65 -O -t c64 --static-locals $(SRC_DIR)config_parser_new.c -o $(BUILD_DIR)config_parser_new.s
	ca65 $(BUILD_DIR)config_parser_new.s -o $(BUILD_DIR)config_parser_new.o

config_parser.o: io.o instruction.o jsmn.o
	cc65 -O -t c64 --static-locals $(SRC_DIR)config_parser.c -o $(BUILD_DIR)config_parser.s
	ca65 $(BUILD_DIR)config_parser.s -o $(BUILD_DIR)config_parser.o

interpreter.o: parser.o lexer.o utils.o text.o
	cc65 -O -t c64 $(SRC_DIR)interpreter.c $(BUILD_DIR)interpreter.o -o $(BUILD_DIR)interpreter.s
	ca65 $(BUILD_DIR)interpreter.s -o $(BUILD_DIR)interpreter.o

parser.o: lexer.o io.o utils.o text.o
	cc65 -O -t c64 $(SRC_DIR)parser.c $(BUILD_DIR)text.o -o $(BUILD_DIR)parser.s
	ca65 $(BUILD_DIR)parser.s -o $(BUILD_DIR)parser.o

lexer.o: utils.o io.o text.o
	cc65 -O -t c64 $(SRC_DIR)lexer.c $(BUILD_DIR)text.o -o $(BUILD_DIR)lexer.s
	ca65 $(BUILD_DIR)lexer.s -o $(BUILD_DIR)lexer.o

locations.o: items.o
	cc65 -O -t c64 --static-locals $(SRC_DIR)locations.c $(BUILD_DIR)items.o -o $(BUILD_DIR)locations.s
	ca65 $(BUILD_DIR)locations.s -o $(BUILD_DIR)locations.o

items.o: utils.o
	cc65 -O -t c64 --static-locals $(SRC_DIR)items.c $(BUILD_DIR)utils.o -o $(BUILD_DIR)items.s
	ca65 $(BUILD_DIR)items.s -o $(BUILD_DIR)items.o

actions.o:
	cc65 -O -t c64 --static-locals $(SRC_DIR)actions.c $(BUILD_DIR)utils.o -o $(BUILD_DIR)actions.s
	ca65 $(BUILD_DIR)actions.s -o $(BUILD_DIR)actions.o

instruction.o:
	cc65 -O -t c64 --static-locals $(SRC_DIR)instruction.c -o $(BUILD_DIR)instruction.s
	ca65 $(BUILD_DIR)instruction.s -o $(BUILD_DIR)instruction.o

jsmn.o:
	cc65 -O -t c64 --static-locals $(SRC_DIR)jsmn.c $(BUILD_DIR)jsmn.o -o $(BUILD_DIR)jsmn.s
	ca65 $(BUILD_DIR)jsmn.s -o $(BUILD_DIR)jsmn.o

utils.o:
	cc65 -O -t c64 --static-locals $(SRC_DIR)utils.c -o $(BUILD_DIR)utils.s
	ca65 $(BUILD_DIR)utils.s -o $(BUILD_DIR)utils.o

debugmalloc.o:
	cc65 -O -t c64 $(SRC_DIR)debugmalloc.c -o $(BUILD_DIR)debugmalloc.s
	ca65 $(BUILD_DIR)debugmalloc.s -o $(BUILD_DIR)debugmalloc.o

text.o:
	ca65 -t c64 $(TEXT_DIR)text.s -o $(BUILD_DIR)text.o

clean:
	rm $(BUILD_DIR)*
	rm $(DIST_DIR)*
