# See http://www.throwtheswitch.org/build/make

# Reset suffix list
.SUFFIXES:
.SUFFIXES: .nw .defs .tex .c .h .o

# Use spaces instead of tabs
.RECIPEPREFIX != ps

# Paths
PATHN = noweb/
PATHS = src/
PATHB = build/
PATHD = build/defs/
PATHT = build/tex/
PATHO = build/obj/

# Keep source files
.PRECIOUS: $(PATHS)%.c
.PRECIOUS: $(PATHS)%.h
.PRECIOUS: $(PATHO)%.o
.PRECIOUS: $(PATHT)%.tex

# Commands and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -g3
ALL_CFLAGS = -std=c99 -I$(PATHS) $(CFLAGS)
NOWEAVE = noweave -n -indexfrom $(PATHD)all.defs
LATEX = latex -output-directory=$(PATHT)

# File lists
BUILD_PATHS = $(PATHS) $(PATHB) $(PATHO) $(PATHD) $(PATHT)

DEFS = $(PATHD)intro.defs $(PATHD)node.defs $(PATHD)pool.defs \
      $(PATHD)except.defs $(PATHD)ast.defs $(PATHD)env.defs \
      $(PATHD)cam.defs $(PATHD)optim.defs $(PATHD)lexer.defs \
      $(PATHD)parser.defs $(PATHD)main.defs

TEX = $(PATHT)intro.tex $(PATHT)node.tex $(PATHT)pool.tex $(PATHT)except.tex \
      $(PATHT)ast.tex $(PATHT)env.tex $(PATHT)cam.tex $(PATHT)optim.tex \
      $(PATHT)lexer.tex $(PATHT)parser.tex $(PATHT)main.tex

SOURCES = $(PATHS)ast.h $(PATHS)cam.h $(PATHS)except.h $(PATHS)lexer.h \
      $(PATHS)node.h $(PATHS)optim.h $(PATHS)parser.h $(PATHS)pool.h \
      $(PATHS)env.h $(PATHS)ast.c $(PATHS)cam.c $(PATHS)lexer.c \
      $(PATHS)main.c $(PATHS)node.c $(PATHS)optim.c $(PATHS)parser.c \
      $(PATHS)pool.c $(PATHS)env.c

OBJECTS = $(PATHO)ast.o $(PATHO)cam.o $(PATHO)lexer.o $(PATHO)main.o \
      $(PATHO)node.o $(PATHO)optim.o $(PATHO)parser.o $(PATHO)pool.o \
      $(PATHO)env.o

# Phony targets

.PHONY: all pdf clean

all : pdf $(SOURCES) $(PATHB)main

pdf : $(TEX)
  $(LATEX) book
  noindex book
  cd $(PATHT); bibtex book
  $(LATEX) book
  $(LATEX) book
  dvipdf $(PATHT)book.dvi book.pdf

clean:
  -rm -rf $(BUILD_PATHS)
  -rm -f book.pdf book.nwi

# build directories

$(PATHS) :
  -mkdir $(PATHS)

$(PATHB) : 
  -mkdir $(PATHB)

$(PATHD) : $(PATHB)
  -mkdir $(PATHD)

$(PATHT) : $(PATHB)
  -mkdir $(PATHT)

$(PATHO) : $(PATHB)
  -mkdir $(PATHO)

# noweb definitions

$(PATHD)all.defs : $(DEFS)
  sort -u $(DEFS) | cpif $(PATHD)all.defs

$(PATHD)%.defs : $(PATHD)
  nodefs $(PATHN)$(notdir $(basename $@)).nw > $@

# LaTeX

$(PATHT)%.tex : $(PATHD)all.defs $(PATHT)
  $(NOWEAVE) $(PATHD)all.defs $(PATHN)$(notdir $(basename $@)).nw > $@

# Source files and headers

$(PATHS)%.h : $(PATHN)%.nw $(PATHS)
  notangle -R$(notdir $@) $< > $@

$(PATHS)%.c : $(PATHN)%.nw $(PATHS)
  notangle -R$(notdir $@) $< > $@

# Object files

-include $(OBJECTS:.o=.d)

$(PATHO)%.o : $(PATHS)%.c $(PATHO)
  $(CC) $(ALL_CFLAGS) -c $< -o $@
  $(CC) $(ALL_CFLAGS) $< -MM -MF $(basename $@).d

# Executable

$(PATHB)main: $(OBJECTS)
  $(CC) -o $@ $^
