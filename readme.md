[Camara Lucida](http://www.camara-lucida.com.ar/) is an open-source software toolkit for artists, programmers and designers to make ludic interfaces and digital artifacts based on real-time projection mapping on physical objects.

It uses a 3d Camera-Projector system with a RGBD camera (a kinect) to make a 3d representation of the scene and project graphics on top of physical objects in real-time. 

It has two modules: a [calibration module](https://github.com/chparsons/rgbdemo) and a [render module](https://github.com/chparsons/ofxCamaraLucida) /[OpenFrameworks](http://www.openframeworks.cc/) addon. The calibration module is based on the open-source projects [RGBDemo](https://github.com/nburrus/rgbdemo) by [Nicolas Burrus](https://github.com/nburrus) and [OpenCV](http://opencv.willowgarage.com/wiki/). It calibrates the projector-camera system and saves the data for further usage. The render module is on this repo and it takes care of: loading the calibration data, setting up the graphics framework to render from the projector point of view, making a 3d mesh from a depth map (the depth map is fed by the custom application, so camera capture is decoupled) and rendering to a texture on the 3d mesh (the texture contains the custom application graphics). The desired mapping is achieved by projecting the graphics render from the projector viewpoint.

[Calibration tutorial](http://www.camara-lucida.com.ar/tutorials/calibration)

Credits:
[Christian Parsons](http://www.chparsons.com.ar/)
[Mariano Tepper](http://www.tc.umn.edu/~mhtepper/)

[![Flattr this git repo](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=chparsons&url=https://github.com/chparsons/ofxCamaraLucida&title=Camara Lucida&language=en_GB&tags=github&category=software) 


dependencies:

openframeworks 007
https://github.com/openframeworks/openFrameworks/zipball/007


example_ofxKinect

ofxKinect
stable 007
https://github.com/ofTheo/ofxKinect/zipball/007


example_ofxKinect_opencl

msalibs

https://github.com/memo/msalibs

commit https://github.com/memo/msalibs/commit/8faff7ab35b76d341e9012350dcaafac500a0cd2 


example_ofxOpenNI

ofxOpenNI 
https://github.com/gameoverhack/ofxOpenNI master branch

commit https://github.com/gameoverhack/ofxOpenNI/commit/72dbf239fd8c6e8adbb390ddb4ffbe77a5125965


