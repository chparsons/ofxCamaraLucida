[Camara Lucida](http://chparsons.com.ar/#camara_lucida) is an open-source software toolkit for artists, programmers and designers to make ludic interfaces and digital artifacts based on real-time projection mapping on physical objects.

It uses a 3d Camera-Projector system with a RGBD camera (a kinect) to make a 3d representation of the scene and project graphics on top of physical objects in real-time. 

It has two modules: a [calibration module](https://github.com/chparsons/rgbdemo) and a [render module](https://github.com/chparsons/ofxCamaraLucida) / [OpenFrameworks](http://www.openframeworks.cc/) addon. The calibration module is made on top of [RGBDemo](https://github.com/nburrus/rgbdemo) by [Nicolas Burrus](https://github.com/nburrus). It calibrates the projector-camera system and saves the calibration data. The render module takes care of loading the data, set up the graphics pipeline to render from the projector viewpoint, make a 3d mesh from the depth map and render custom graphics to a texture applied on the 3d mesh.   

[Calibration tutorial](http://chparsons.com.ar/#camara_lucida/tutorials/calib)

Credits:  
[Christian Parsons](http://chparsons.com.ar/)  
[Mariano Tepper](http://www.tc.umn.edu/~mhtepper/)  

<!--[![Donate Bitcoins](http://christianparsons.com.ar/img/donate_bitcoins.png)](http://chparsons.com.ar/#donate)   -->

<!--[![Flattr this git repo](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=chparsons&url=https://github.com/chparsons/ofxCamaraLucida&title=Camara Lucida&language=en_GB&tags=github&category=software)  -->


dependencies:  

openframeworks 0.8.0  

example_openni  
ofxOpenNI v1.0 
https://github.com/gameoverhack/ofxOpenNI/tree/v1.0  

