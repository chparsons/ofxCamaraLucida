Camara Lucida is an open-source software toolkit for artists, programmers and designers to make ludic interfaces and digital artifacts based on real-time projection mapping on physical objects.

It uses a 3d Camera-Projector system with a RGBD camera (a kinect) to make a 3d representation of the scene and project graphics on top of physical objects in real-time. 

It has two modules: a calibration module and a render module. The calibration module is based on the open-source projects RGBDemo by Nicolas Burrus and OpenCV. It calibrates the projector-camera system and saves the data for further usage. The render module is on this repo and it takes care of: loading the calibration data, setting up the graphics framework to render from the projector point of view, making a 3d mesh from a depth map (the depth map is fed by the custom application, so camera capture is decoupled) and rendering to a texture on the 3d mesh (the texture contains the custom application graphics). The desired mapping is achieved by projecting the graphics render from the projector viewpoint. The render module is by now an OpenFrameworks addon and ports to other frameworks (e.g. Cinder, Cing, Quartz Composer) are planned for future work.

[![Flattr this git repo](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=chparsons&url=https://github.com/chparsons/Camara-Lucida&title=Camara Lucida&language=en_GB&tags=github&category=software) 

