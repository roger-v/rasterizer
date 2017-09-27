# rasterizer

![alt text](http://people.tamu.edu/~rogervillanueva/img/cow.png)

This is a polygon rasterizer than I wrote for Computer Graphics. The input is a .obj file, which is a text file format containing x, y, and z values for vertices between -1.0 and 1.0 (along with other info), and the output is a model rendered through the usage of only one OpenGL tool, which is to set the frame buffer of pixels on the window. The steps in the program include but are not limited to taking the faces, calculating lighting, converting the polygons from world space to screen space, and scan converting them with the appropriate lighting. The cow on the left is shaded using flat shading, and the one on the right is shaded using Phong smooth shading. They are the exact same model.
