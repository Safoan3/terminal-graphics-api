

Stupidly simple to use.



A frame buffer in your terminal where you can directly manipulate pixels
or draw triangles. Supports triangle rasterization with zbuffer.

Enjoy.

example codes are given.


EXAMPLE1: line rendering 
EXAMPLE2: 3D SPINNING DONUT WITH Z BUFFERING USING THE API 


compilation method: gcc 

donut:
❯ g++ donut.cpp ../graphicsapi.cpp -o donut && ./donut 

line example:
❯ g++ example1.cpp ../graphicsapi.cpp -o e1 && ./e1


