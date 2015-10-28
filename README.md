TouhouC
==============

Introduction
------------
A simple bullet hell(shoot them up) game similar to touhou(東方), written in C.

The name has a `C` at the end because it is a console implementation

Key features

 * Basic bullet curtain game play
 * Cross platform
 * Supports Windows Command Prompt
 * 60FPS
 * Colour
 * Console resize while gameplay
 

Motivation
----------
I couldn't find a text based *Touhou danmaku*(弾幕) game so I thought It might be nice to implement one
I challenged myself to complete this game in 6 hours, and I finished the game in about 4 hours combined.

This game is a coursework written for:

    Week4 extension (due 2015/10/26)
    Programming and Algorithms I (COMS10002)
    University of Bristol, United Kingdom

Work handed in as `wl14928` 

If you're from UoB too, you can contact me with that user name (`name@bristol.ac.uk>`)

How to build
------------
This project uses standard makefile, simply:
    
	make clean all
	
The binary will be located in project root named `main` (same directory with the makefile)

Use `make clean` to clean up(the compiled binary and object files will be removed)


Implementation details
---------------
Cross platform console cursor manipulation is hard but `ncurses` is a bit of a overkill, 
so I found a cute little library that takes care it for me. The library is called 
[rlutil](https://github.com/tapio/rlutil), and license for this library is extremely permissive 
so including the headers in my project does not violate any laws. This library is also exceptionally 
compact, only a single header file is needed.

The bullet dynamic is implemented with function pointers in the structure `Bullet`, this loosely resembles lambda 
expression in functional languages.

The entire game is implemented in a game loop without thread.

Features & Usage
------------

Execute the generated binary in a console, Linux tends to perform console operations smoother.
 

Dodge bullets! some bullets are good so you might want to look for them.

Use the standard [W, S, A, D] for [UP, DOWN, LEFT, RIGHT] and [q] to exit at any point.



Possible improvements
----------------------

 * Better pixel snapping
 * Performance issues with the list, need threads
 * Clear screen doesn't work that well on Windows
 * Sound
 * More ASCII art
 * Keyboard scanning should have it's own thread
 * Save file



## License

    Copyright 2015 WEI CHEN LIN

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
    
Additional notice for `rlutil.h`:

     DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004
    
    Copyright (C) 2010 Tapio Vierros
    
    Everyone is permitted to copy and distribute verbatim or modified
    copies of this license document, and changing it is allowed as long
    as the name is changed.
    
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
    TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
    
    0. You just DO WHAT THE FUCK YOU WANT TO.



 
