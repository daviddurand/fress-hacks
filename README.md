# Fress Hacks

Various quick hacks to deal with fress and fress/imlac
files of various sorts. Source and makefile live in src.
edit the makefile as you wish, then runn it.

# What they are and how to build

Various scripts assume that the output binaries are in your
path.

fress.convert:
fress.convert turns a fress binary file into an XML export
containing all the original information. DTD and some test
FRESS files are in fressconv.

trans:
trans translates non-standard characters in an ascii print
file from hercules into UTF-8 with appropriate characters.
the tn-chain file works for mainframs programs like fress that
assumed the TN printer chain would be mounted on the line
printer. At some point we will also implement the ALA character
set, which was known as the ALX print train at Brown.

A separately modified version of the Unix utility paps can be
used to turn the UTF-8 results into a PDF file.

Trans takes a file in a very limited format:
lines starting with # are comments
all other lines start with an 8-bit code point position,
_e.g._ `0xff`, a space, and then a replacement value (which may be multiple
UTF-8 bytes).

If the replacement value starts with `"` then it's a quoted
string, so you enter UTF8 literally in your editor. The string can contain `"`,
but cannot contain whitespace. The closing quote is not validated.

otherwise, any non-whitespace charcters are treated as the replacement.

If the replacement value starts with `0x` then the following string of hexadecimal
digit pairs is copied as literal bytes.

Because hercules won't output EBCDIC directly, trans first reverses the EBCDIC->ASCII
translation, so that the code points can be defined in terms of the EBCDIC code page.
The specific reversible translation is hard-wired in to correspond to our Hercules
setup.

slc:
slc converts .itext files (stored as .punch output from
hercules) into IMLAC memory images, logging a bunch of stuff
along the way. itext directory contains the 3 itext decks
currently avaialable, and a script that creates memory images
by loading these in different orders.

punch output is currently BINARY/EBCDIC. We should use the
approach in trans, and the reversible EBCDIC/ASCII translation
used for the the printer files, and convert these to
ASCII images, so that we can modify the patches to get the
resulting images to function correctly. 

# Data files and contemporary info

## unconverted

This directory contains raw FRESS files suitable for running through
the fress converter to produce XML export files.

## documents

Contains recently generated PDFs of some of the FRESS contemporary documentation
that we have in FRESS files. This is a little easier to read that starting
up vm370 in emulation an reading them with FRESS itself. Some of the files have
artifacts like extra blank pages or Character set problems (I've been having trouble
finding a good-looking Unicode monospace font that contains a few of the more
esoteric EBCDIC bullets.)


# Bonus: directory HES 

This contains the relatively unexamined source of the first
Brown hypertext system as described in van Dam, Carmody, and Nelson's
paper. This was the forerunner of Fress. One thing I never knew but
learned from scanning this briefly is that the Fress formatting
language is a derivative of the text360 formatting language.

Edit/Alter codes, and some of the syntax.

The papers are there in original form as used by HES, which
includes extra stuff for linking, etc.

There are some 2250 Vector Scope PDF documents in the HES directory. There are
a bunch more at bitsavers. At this point, I'm not sure if it makes sense to
copy them all here for a more standalone archive. Many are not really that
relevant for understanding HES itself.

I've left the 2260 docs in place, but I think they are irrelevant,
they are not a later version of the 2250, though they have similar IO
stub libraries.

## documents
There are two word files from Ted and Andy from some HES-related
presentation in the past.

There are PDFs of some comtemporary documents courtesy of Ted Nelson's
collection at archive.org. These include an early manual, as well
as Ted's notes that he made when preparing the Patent database that is
discussed in the HES papers. Also, a research progress report to IBM,
sadly missing page 2, David Rice's masters thesis, which includes a
section on HES, and a manual for a later version of text360
which continued in use on MTS (the Michigan Terminal System), another
370-based timesharing system of the 70's.
