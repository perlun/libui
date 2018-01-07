# libui

`libui` is a simple console UI library for Linux terminals. It is written in C++ and runs on text-mode; does not support X Windows or SSH etc. at the moment. It's an abstraction layer on top of the actual console subsystem, so that you can write your application in a generic, less-platform-centric way.

(There was once a port to `ncurses` done by [Bartek Kania](https://bk.gnarf.org/) but unfortunately I have lost the code to that version since it was made. If I recover it, I will add it to this repository as well.)

Caveat: the code has not been compiled since 1999 or so. It's quite likely it will need some work to be useful with modern `g++` versions.
