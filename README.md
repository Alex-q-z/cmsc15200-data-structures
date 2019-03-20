# uchicago-cs152-project

# hw4
This is a C file for realizing various traversal operations. 

# new format for ppm file compression: p152

byte 0:5\
The first six bytes of a P152v1 file contain the characters "P152v1".\

byte 6:7\
The two bytes are reserved for future expansion. For the time being,\
they can contain anything. The same is true of any other bits and\
bytes identified below as reserved for future expansion.\

byte 8:11\
These bytes contain the current date as a YYYYMMDD integer (for\
example, 20190228). This should be the date at which the conversion \
to P152 is performed. Type "man localtime" to read about the various\
ways to read the current date from the system. This number is written\
into the file as a 4-byte big-endian integer.\

byte 12:13\
The current time is given as an HHMM integer. For example: 0 is\
midnight, 930 is 9:30am, 1345 is 1:45pm. The time should be the\
minute at which the P152 conversion is performed. This is written \
into the file as a two-byte big-endian integer.\

byte 14:15\
The image width is written as a 2-byte big-endian integer.\

byte 16:17\
The image height is given as a 2-byte big-endian integer.\

byte 18:21\
These bytes are reserved for future expansion. (They will store the\
color table size.)\

byte 22\

The leftmost bit -- the highest bit -- of byte 22 is 1 for run-length\
encoded color data, 0 for raw data. For project 1, this bit is always\
set to 1.\

The next-to-leftmost bit of byte 22 is 1 for a grayscale image, 0 for\
color.\

The next 6 bits in byte 22 are reserved for future expansion.\

byte 23:25\
Reserved for future expansion.\

starting at byte 26:\
A C string describing the image, terminated with a '\0'. Because the\
file includes a terminating character, the string can be any length.\

following the string:\
The color table. This is unused in the current project. Write four\
zero bytes in the color table section for now.\

following the color table:\
First, a 4-byte big-endian integer giving the number of runs.\
Then, the run lengths in order, each run length a 4-byte big-endian\
integer.\
Then, the colors in order and in lockstep with the runs. If the image\
is a grayscale image, these colors are one byte each; otherwise, they\
are three bytes each in rgb order.\
