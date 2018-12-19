Various quick hacks to deal with fress and fress/imlac
files of various sorts. Source and makefile live in src.
edit the makefile as you wish, then runn it.

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
