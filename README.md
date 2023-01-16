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

When inside the editor:
- CONTROL S -> save
- CONTROL D -> save with name
- CONTROL G -> open a menu that allow you to go to a specific line in the file
- CONTROL R -> reinitialize the interface
- CONTROL Q -> quit (without saving, no warning are showed)
- You can use the arrow to move around. You can use control + arrow to skip words
- With PAGE UP and PAGE DOWN (or control + arrow up, control + arrow down) you can move up or down by one page (one page is the number of lines that are visible in the screen)

# Compile instructions:

1 - Make sure you have the ncurses libraries installed, otherwise run 
``` 
$sudo apt-get install libncurses5-dev libncursesw5-dev
```
2 - Go inside the project directory, then run the makefile

``` 
$cd [where you put the folder]
$makefile
```
3 - Some folder will be generated, the compiled program should be inside /bin/Relase

# Know issues:

The code is quite a mess and there are a lot of returning values from function that are not evaluated. IDK if I'll continue working on this, but the first thing that I'll do is to clean the code

# Roadmap:

- Add selection support
- Add copy/paste support
- Add some tweaks to improve the GUI, and even a file explorer
- I'd like to have colours and themes at a certain point


 
