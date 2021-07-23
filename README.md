# MatrixIM
## About
MatrixIM is [Matrix](https://www.matrix.org) client that aims to be fast, reliable and have low resource usage. MatrixIM is written in C, and uses GTK+ 3
as GUI library.
## Development status
At the moment MatrixIM is able to login into Matrix homeserver via HTTP API.
## Building
### Windows
Windows is unsupported at the moment.
### Linux
MatrixIM requires following libraries: [GTK+ 3](https://www.gtk.org), [cJSON](https://github.com/DaveGamble/cJSON)\
You also need C compiler for example GCC.\
After you install needed libraries enter MatrixIM folder and run command `make -f Makefile_Linux`
### MacOS X
MatrixIM should work on MacOS X but it is untested. Feedback is welcome.
