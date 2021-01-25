# KhuDaNet(Perceptron, MLP, CNN)
* KhuDaNet: Open source platform for neural networks (Kyung Hee University, Data Analysis & Vision Intelligence)
* C++, (Perceptron, MLP(DNN), CNN)

## Source file
* KhuDaNetLayer.cpp / KhuDaNetLayer.h
  + Fully connected layer
* KhuDaNet.cpp / KhuDaNet.h
  + Sequential network
  
## MNIST test
* Download train and test data from [http://yann.lecun.com/exdb/mnist/](http://yann.lecun.com/exdb/mnist/)
* Move to decompressed files to [Run] folder
* Run the following files
  + KhuDaNetApp.exe: Windows application using MFC
  + KhuDaNetConsole.exe: console application
* Network I - [1.KhuDaNet-Perceptron](https://github.com/NizeLee/KhuDaNet/tree/main/1.KhuDaNet-Perceptron)
  + Input: 28*28 pixel values ranging [0, 1]
  + Output: 1 node for binary decision (class 0: digits 0, 1, 2, 3, 4, class 1: digits 5, 6, 7, 8, 9)
  <br><br> <img src="1.KhuDaNet-Perceptron/Perceptron.png"></img>  
* Network II - [2.KhuDaNet-MLP-DNN](https://github.com/NizeLee/KhuDaNet/tree/main/2.KhuDaNet-MLP-DNN)
  + Input layer: 28*28 pixel values ranging [0, 1]
  + Hidden layers: 2 hidden layers
  + Output layer: 10 node for 10 digits
  <br><br> <img src="2.KhuDaNet-MLP-DNN/KhuDaNet(DNN).gif"></img>  
* Network III (LeNet5-like CNN) - [3.KhuDaNet-CNN](https://github.com/NizeLee/KhuDaNet/tree/main/3.KhuDaNet-CNN)
  + Input layer: 32*32 pixel values ranging [0, 1]
  + Hidden layers: CNN-POOL-CNN-POOL-CNN-FC
  + Output layer: 10 node for 10 digits
  <br><br> <img src="3.KhuDaNet-CNN/KhuDaNet(CNN).gif"></img>  

* Solution files for Visual Studio
  + KhuDaNetApp.sln: Windows application using MFC
  + KhuDaNetConsole.sln: console application

