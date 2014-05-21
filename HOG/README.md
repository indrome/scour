Histogram of Oriented Gradients (HOG)
=====================================

HOGs transform the silhouette of a human into a desciminative feature vector. 
This library provide functionality to compute these gradients either by sliding 
a window over the image width and height, or by providing a bounding box at which 
a single histogram feature is computed.

The wrapper program `trainer.py` can perform automated SVM training using svm_light. 
You only provide the path to your PASCAL annotation files.

Usage
=====

Run the feature extractor using a sliding window:

``` ./bin/HOG_sliding path_to_image.jpg ```

Perform feature extraction using bounding box:

``` ./bin/HOG_BB path_to_image.jpg Xmin Ymin Xmax Ymax ```

NOTE: these binaries are both build from lib/HOG.o

Perform automated SVM training:

``` python2 wrapper.py path_to_annotations ```

Note
====

The INRIA person dataset images are not backwards compatible with some png-libraries. 
To tix problems reading these images one can use '''optipng''' to fix the files.
For example:

``` optipng --fix --force --quiet IMG_1.png IMG_2.png ... ```

