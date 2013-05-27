[Camara Lucida](http://www.camara-lucida.com.ar/) is an open-source software toolkit for artists, programmers and designers to make ludic interfaces and digital artifacts based on real-time projection mapping on physical objects.

It uses a 3d Camera-Projector system with a RGBD camera (kinect) to make a 3d representation of the scene and project graphics on top of physical objects in real-time. 

It has two modules: a [calibration module](https://github.com/chparsons/rgbdemo) and a [render module](https://github.com/chparsons/ofxCamaraLucida) /[OpenFrameworks](http://www.openframeworks.cc/) addon. The calibration module is based on the open-source projects [RGBDemo](https://github.com/nburrus/rgbdemo) by [Nicolas Burrus](https://github.com/nburrus). It calibrates the projector-camera system and saves the data for further usage. The render module takes care of: loading the calibration data, setting up the graphics framework to render from the projector point of view, making a 3d mesh from a depth map (the depth map is fed by the custom application, so camera capture is decoupled) and rendering to a texture on the 3d mesh (the texture contains the custom application graphics). The desired mapping is achieved by projecting the graphics render from the projector viewpoint.

[Calibration tutorial](http://www.camara-lucida.com.ar/tutorials/calibration)

Credits:  
[Christian Parsons](http://www.chparsons.com.ar/)  
[Mariano Tepper](http://www.tc.umn.edu/~mhtepper/)  

[![Donate Bitcoins](http://christianparsons.com.ar/img/donate_bitcoins.png)](http://chparsons.com.ar/#donate)   

<!--[![Flattr this git repo](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=chparsons&url=https://github.com/chparsons/ofxCamaraLucida&title=Camara Lucida&language=en_GB&tags=github&category=software)   -->


dependencies:  

openframeworks 0073   

example  
chparsons/ofxKinect  
https://github.com/chparsons/ofxKinect

example_openni  
ofxOpenNI  
https://github.com/gameoverhack/ofxOpenNI  

