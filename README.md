# convolution_kernel
This is an Image Processing program which applies filters to an image with the use of a convolution kernel.

## How it works:
A .ppm image is read and each pixel's RGB values are stored in an array. Using the given kernel, a filter is applied on the image via convolution. The processed image is then saved as an output.ppm file.


### A visual example

When given an input image like this:
![input image](flowers.jpg)

applying an edge detect kernel of:
```
-1 -1 -1
-1  8 -1
-1 -1 -1
```
yields an output of:
![output image](output.jpg)
