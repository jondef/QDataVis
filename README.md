# QDataVis

![GitHub labels](https://img.shields.io/badge/Version-v1.0.0-blue)
![GitHub labels](https://img.shields.io/badge/Language-C++-orange)
![GitHub](https://img.shields.io/github/license/jondef/QDataVis)
![GitHub repo size](https://img.shields.io/github/repo-size/jondef/QDataVis)
![Maintenance](https://img.shields.io/maintenance/yes/2019)
![GitHub labels](https://img.shields.io/badge/Ask%20me-anything-blue)

QDataVis is a simple to use, UI based, interactive plotter powered by <img src="https://cdn2.hubspot.net/hubfs/149513/Qt2017TemporaryImagesWebsite/qt_logo_with_text_green_rgb.png" height="15"/>.
 
  - Includes a mathematical expression parser written by me :)
  - Expect lots of bugs
  - Plot and visualise functions
  - Plot points

# New Features!
  - Plot cursor
  - Stick tick labels to the zero on the axes


You can also:
  - Save your plots as PNG, JPG, etc...
  - Configure plot aesthetics
  - Plot single data points


Dependencies
---------
QDataVis depends on a few libraries to work properly:

* [Qt] - Great UI framework
* [QCustomPlot] - Awesome Qt compatible Plotting widget (included in repo)


Building from source
-------
QDataVis requires [Qt] v5.12.3+ to be installed in included in PATH.

 * Clone the repository `git clone https://github.com/jondef/QDataVis.git`
 * Fetch submodule(s) `git submodule update --init`
 * Load the CMake project and compile! `cmake ./QDataVis && make ./QDataVis/build`
 * To pull submodule(s): `git submodule update --remote`


License
----
Apache 2.0

[//]: # (comment?)
   [Qt]: <https://www.qt.io/>
   [QCustomPlot]: <https://www.qcustomplot.com/>
