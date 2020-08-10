# QtObjectDetector - OpenCV pipeline creator and object detector

This is a Qt/C++ application to interact with OpenCV. It is possible to create a basic piplene and deploy it to images or videos. Currently you can use 16 OpenCV functions and almost all parameters of these functions. The supported functions are: CvtColor, Threshold, AdaptiveThreshold, MedianBlur, Blur, GaussianBlur, BitwiseNot, Pow, Erode, Sobel, Laplacian, Canny, AddWeighted, Cascade Classifier, Shi-Tomasi, Background substraction. 

![Test Image 1](QtObjectTracker.bmp)


Here is an example for face detection using cascade classifiers. Your own classifiers should be in "data/haarcascades" folder.


![Test Image 1](Facedetection.bmp)


Here is another example for a canny function/pipeline applied to a live cam, the function is used to detect conrers.

![Test Image 1](Canny.bmp)
