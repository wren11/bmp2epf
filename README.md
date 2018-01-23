# bmp2epf
A simple tool for generation new images for Dark Ages


This program is pretty straight forward.

it takes --frame as multiple input.

so if you want to create a new epf file with 3 animations,
you should do bmp2epf --frame a.png --frame b.png --frame c.png output.epf
and it will output the following: 

output.epf that contain's your frame and stencil data.
it will also output a output.tbl and an output.pal file.

now what? (Disclaimer this information below may not be correct, I have just recently researched this stuff)

well, now you can do what you want with them. but for them to have impacting effects.
you will need to define the tbl files.


if you want to make a brand new animation,

you first need to define a new ID for your animation. this is the same animation number you would send via an animation packet.
it can be any number, but it must be not already used.
so determine the next logical ID to use in your roh.dat and use the next number.

for example, if the last file in your roh.dat is efct340.epf then your new epf should be called efct341.epf
and your pallette file also must be in palette order. so if your last eff###.pal file is eff029.pal, then you should use eff030.pal.
so it's index ordered.

now, once you have this setup, the next thing to do is update your effect.tbl file.

inside this file you will see the following:
339
0 1 2 3
0 1
0 1 2 3 4 5
0 1 2 3 4 5 6 7
0 1 2 3 4
0 1 2 3 4 5 6
0 1 2 3 4
0 1 2 3 4 5
0 1 2 3 4 5 6 7 8 9 10
0 1 2 3
0 1 2 3 4
0 1 2 3 4 5
0 1 2 1 2 3
0 1 2 3
0 1 2 1 2 3
0 1 2 1 2
0 1 2 1 2
0 1 2 3 4
0 1 2
0 1 2 3 4 5 6 7
0 1 0 1
0 1
0 1 2 1 2
0 1 2 3 4 5 6 7
0 1 2 3
0 1 2
0 1 2 3
0 1 2 3
0 1 2 3


ect....


what this is doing is defining what frame indexes are used by your animation.
so because we are adding a new animation called 340.epf, we first must increment the first frame out number from
339 to 341.

then you must go to line 341 in the file and insert the frame indices.
that means you need to add 340 0 1 2 3 .... for however many frames indicies necessary.

anyways i hope that makes sense.
now onto effpal.tbl

this file order does not matter.
so you can just insert anywhere the line 340 30
because it's telling the client for effect id 340, use palette at index 30.
(the one we just made.)

repack the roh.dat and now you can use animation number 340 via packet, ect.
if you want to use this on the USDA live server to change assets. then you need to replace the existing ID of an existing image.
as the packet the server sends will never be sent to the client. only private servers really can have brand new assets.



