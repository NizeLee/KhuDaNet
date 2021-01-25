# KhuDaNet(Perceptron, MLP, CNN)
* KhuDaNet: Open source platform for neural networks (Kyung Hee University Data Analysis & Vision Intelligence Neural Networks)
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
* Network I
  + Input: 28*28 pixel values ranging [0, 1]
  + Output: 1 node for binary decision (class 0: digits 0, 1, 2, 3, 4, class 1: digits 5, 6, 7, 8, 9)
  <br><br> <img src="1.KhuDaNet-Perceptron/Perceptron.png"></img>  
* Network II
  + Input layer: 28*28 pixel values ranging [0, 1]
  + Hidden layers: 2 hidden layers
  + Output layer: 10 node for 10 digits
  <br><br> <img src="2.KhuDaNet-MLP-DNN/KhuDaNet(DNN).gif"></img>  
* Network III (LeNet5-like CNN)
<br><br> <img src="3.KhuDaNet-CNN/KhuDaNet(CNN).gif"></img>  
  
``` C++
struct CKhuDaNetLayerOption{
	CKhuDaNetLayerOption(unsigned int nLayerTypeIntput, int nImageCntInput, int nNodeCntIput, int nWidthInput, int nHeightInput, int nKernelSizeInput, int nActicationFnInput, double dLearningRateInput);
	unsigned int nLayerType;
	int nImageCnt;
	int nNodeCnt;
	int nW, nH;
	int nKernelSize;
	int nActicationFn;

	double dLearningRate;
};

void CKhuDaNet::AddLayer(CKhuDaNetLayerOption LayerOptionInput);
``` 

``` C++
CKhuDaNet m_Network;  // LeNet5-like CNN
m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_INPUT | KDN_LT_IMG, 1, 0, 32, 32, 0, 0, 0.15));
m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_CON, 6, 0, 28, 28, 5, KDN_AF_RELU, 0.15));
m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_POOL, 6, 0, 14, 14, 2, 0, 0.5));
m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_CON, 16, 0, 10, 10, 5, KDN_AF_RELU, 0.15));
m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_POOL, 16, 0, 5, 5, 2, 0, 0.5));
m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_CON, 120, 0, 1, 1, 5, KDN_AF_RELU, 0.15));
m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_FC, 0, 84, 0, 0, 0, KDN_AF_RELU, 0.15));
m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_FC | KDN_LT_OUTPUT, 0, 10, 0, 0, 0, KDN_AF_SOFTMAX, 0.15));
```

``` C++
m_Network.InitWeight(); // weight initialization
m_Network.TrainBatch(InputList, OutputList, nBatchSize, &Loss); // training
int nResult = m_Network.Forward(TestInput); // test
```

* Solution files for Visual Studio
  + KhuDaNetApp.sln: Windows application using MFC
  + KhuDaNetConsole.sln: console application

