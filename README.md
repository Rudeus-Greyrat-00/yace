# yace
Yet another CLI editor, a simple command line interface text editor written in C

Yace is a simple text editor that I created in a couple of days for fun. This version is still a little bit rough, and lack a lot of functions.

![immagine](https://user-images.githubusercontent.com/96582680/212738286-d1b91f13-29de-4f14-9edd-57ea2c88a1cd.png)

![immagine](https://user-images.githubusercontent.com/96582680/212739283-f54707c5-0391-40f2-b719-e774923f06de.png)

![immagine](https://user-images.githubusercontent.com/96582680/212739397-59daa334-0a66-4ab7-917a-bbb84b87ebe4.png)

# Usage:

Open a file with 
```
$yace filename
```
Or create a new one just by
```
$yace
```
and then saving with CTRL S or CTRL D

When inside the editor:
- CONTROL S -> save
- CONTROL D -> save with name 
- CONTROL G -> open a menu that allow you to go to a specific line in the file
- CONTROL R -> reinitialize the interface
- CONTROL Q -> quit (without saving, no warning are showed)
- You can use the arrow to move around. You can use control + arrow to skip words
- With PAGE UP and PAGE DOWN (or control + arrow up, control + arrow down) you can move up or down by one page (one page is the number of lines that are visible in the screen)

# Compile instructions:
0 - Download the folder "source"

If you want you can change the number of spaces per tab, it is in a #define in the main.h file. It is 4 by default, you can set it to 8 if you prefer

1 - Make sure you have the ncurses libraries installed, otherwise run 
``` 
$ sudo apt-get install libncurses5-dev libncursesw5-dev
```
2 - Go inside the "source" directory, then run the makefile

``` 
$ cd (go inside "source")
$ make
```
3 - Some folder will be generated, the compiled program should be inside projectfolder/bin/Relase

# Know issues:

The code is quite a mess and there are a lot of returning values from function that are not evaluated. 

# Roadmap:

- Add selection support
- Add copy/paste support
- Add some tweaks to improve the GUI, and even a file explorer
- I'd like to have colours and themes at a certain point

# Documentation:

- the yace.c contains the main function and the main loop
- guim.c/h == gui management, it manage the GUI using the ncurses libraries and some modules that I written. The file guim.h contains a stack where the windows are stored. The windows are struct that I called "UserControl". They contains the function to manage themself, to draw themself, and so on. The last window in the stack is the one that is updated
- the files that end with "win" are used by guim.h
- docm.c/h and memm.c/h defines the structures used to store the actual content of the file. memm.c (memory management) define a single line of the document (called DocumentString), docm.c/h define a structure called Document that contains multiple DocumentString
- utils.c/h contains some function that I didn't know where to put
- filem.c/h (file management) contains functions to load and save files

Thats all


 
