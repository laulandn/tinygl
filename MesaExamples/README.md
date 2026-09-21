GLUT apps require certain resources, which is why they are here.
The app building scripts are connivence, and NOT related to SDL directly.

NOTE: These may or may not use the OpenGL and GLUT libs in the parent directory.
These will use whatever has been "installed".

Because of this, building them does not make sense if you are testing OpenGL or GLUT,
without having done an "install" before.