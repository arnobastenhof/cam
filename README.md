A literate implementation in C of the Categorical Abstract Machine
==================================================================

Introduction
------------
The present project constitutes a very modest literate implementation of the
Categorical Abstract Machine, proposed by Cousineau, Curien and Mauny in
1985 for the interpretation of functional programming languages. Our aims
having been primarily self-educational, we imposed severe restrictions from the
outset on the results we intended to achieve in order to maintain focus and
allow for completion within a reasonable timeframe. Most notably, our interest
was primarily in the abstract machine itself, and the frontend we wrote for it
was kept as simple as possible to the extent of having become useless beyond
providing access to the CAM's internals. The intent of this project was not to
learn about Hindley-Milner type inference or language design, and it shows.

Requirements
------------
* Noweb
* GCC or Clang
* GNU Make

Installation
------------
If you have Git installed, executing the following command will create a
directory `prolog-jvm/`, clone the entire repository therein and checkout the
master branch:
```
git clone https://github.com/arnobastenhof/cam.git
``` 
Alternatively, Github offers the possibility to download a ZIP file containing
the sources from whichever branch is being viewed. E.g., to download the
master branch, run
```
wget https://github.com/arnobastenhof/cam/archive/master.zip
unzip master.zip
mv cam-master cam
```
Next, navigate to the project root and run a build:
```
cd cam
make all
```
This will result in both a pdf and the source files to be generated, together
with an executable `build/main`.

Usage
-----
`build/main` reads lines from standard input, expecting either `halt` (to quit)
or a closed lambda term, as defined by the following grammar in ISO EBNF:
```
expr  = var | num | sum | app ;
num   = digit, { digit } ;
var   = alpha, { alpha } ;
sum   = "(", "+", expr, { expr }, ")" ;
app   = "(", abs, expr, { expr }, ")" ;
abs   = "(", "lambda", "(", var, { var }, ")", expr, ")" ;
alpha = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J"
      | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T"
      | "U" | "W" | "X" | "Y" | "Z"
      | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j"
      | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t"
      | "u" | "w" | "x" | "y" | "z" ;
digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
```
Note this grammar does not admit the full generality that ordinary lambda
calculus provides, and moreover defines but one operator constant. Little
stands in the reader's way of extending the current codebase to rememdy
the latter limitation, although the first is more serious. Specifically, to
make our life easier, we have restricted to the description of expressions all
whose constituents we are certain denote numbers. This obviates the need for
type checking, but makes it impossible to abstract over functions. For more
information, the reader is referred to section 4.1 of `book.pdf`.
