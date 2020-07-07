# SFML strange line

 SFML in Xcode
 ---
 1) https://www.youtube.com/watch?v=IghUVIL6okk&list=PLskEAOuS4gQMoAE68-71KRxQegr8rSw54
 2) https://www.youtube.com/watch?v=mtEiyDbYMxQ
 3) https://stackoverflow.com/questions/43119058/xcode-c-and-sfml-library-issues
```
Build Settings > Search Paths > Header Search Paths > + > /usr/local/Cellar/sfml/2.5.1/include
Build Settings > Search Paths > Library Search Paths > + > /usr/local/Cellar/sfml/2.5.1/include

Build Phases > Link Binary With Libraries > + > Add Files... > cmd + G > /usr/local/Cellar/sfml/2.5.1/lib > get .dylibs files > Open
```
 
thick line
 1) https://www.sfml-dev.org/tutorials/2.0/graphics-shape.php
 2) https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader#23286000001269127
 3) http://codeflow.org/entries/2012/aug/05/webgl-rendering-of-solid-trails/
 4) https://discourse.libcinder.org/t/smooth-efficient-perfect-curves/925
 5) https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php
