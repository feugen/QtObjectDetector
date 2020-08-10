# QtObjectDetector - OpenCV pipeline creator and object detector

This is a Qt/C++ application to interact with OpenCV. It is possible to create a basic piplene and deploy it to images or videos. Currently you can use 16 OpenCV functions and almost all parameters of these functions. The supported functions are: CvtColor, Threshold, AdaptiveThreshold, MedianBlur, Blur, GaussianBlur, BitwiseNot, Pow, Erode, Sobel, Laplacian, Canny, AddWeighted, Cascade Classifier, Shi-Tomasi, Background substraction. 

![Test Image 1](QtObjectTracker.bmp)


Here is an example for face detection using cascade classifiers. Your own classifiers should be in "data/haarcascades" folder.


![Test Image 1](Facedetection.bmp)


Here is another example for a canny function/pipeline applied to a live cam, the function is used to detect conrers.

![Test Image 1](Canny.bmp)

The workflow should be as follows:

1. You load a picture, select a function, change the parameters and apply it to a loaded picture. Immediatly you can see the changes, if you are satisfied with the results, then you can store the step. Then you select a different function and repeat the steps.
2. When you are done adding the steps, you can name the pipline and store it. If you wish you can create multiple piplines - just repeat the steps.
3. Once you finished your pipeline, you can apply it to any picture, just load a new picture and click on "apply pipeline". If you wish to apply it to a video or webcam, you have to got to a different tab (video tab), start a video or webcam and select and load the pipeline.

Unfortunately you can't create the pipeline on the fly (while the video is playing or web cam is working). I might change it in a distant future, but its unlikely, because first I need to fix many existing bugs. And as you know, time is money, and I dont have money :-)
