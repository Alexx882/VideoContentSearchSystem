623.915	Current Topics in Multimedia Systems: Content Search with Deep Learning
Alexander Lercher

The general description of my project was done in "readme.md". The zip folder contains the source code but no datasets used during training, no trained networks and no videos. To run the web app from code please insert the videos into 'search_tool/static/videos/v3c1'.
The web app can be started with docker-compose and is also running on http://143.205.173.117:5000/ (only accessible inside the University/VPN as I used our OpenStack).

First, I used the "VOC2012" dataset but the images had multiple concepts (I only used the first) and I assume this is the reason for bad classifications. Next, I used "IIC" which uses landscapes. These classifications worked better but are still bad sometimes. In my opinion, the "Mixed" dataset works best but still contains bad classifications. The networks had .4, .6 and .7 validation accuracy respectively. 
Many bad classifications for V3C1 can be explained due to the dominant colors in the images. For instance redish dirt is detected as sea because training data contains multiple sand, stone and sunset (redish) images. Bright images are classified as glacier.

With better preprocessing for training the results should get better. Furthermore, the network architecture was taken from Assignment 4 and could also be extended.