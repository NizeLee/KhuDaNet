#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "../KhuDaNetApp/KhuDaNetLayer.h"
#include "../KhuDaNetApp/KhuDaNet.h"

#define MAX_PATH	260

void ReadFile();
void LoadMnistTrain();
void LoadMnistTest();

CKhuDaNet gNetwork;

char gExePath[MAX_PATH];
char gTrainImagePath[MAX_PATH];
char gTrainLabelPath[MAX_PATH];
char gTestImagePath[MAX_PATH];
char gTestLabelPath[MAX_PATH];

int gMnistTrainCnt = 60000;
int gMnistTestCnt = 10000;
double **gMnistTrainInput = NULL;
int *gMnistTrainOutput = NULL;
double **gMnistTestInput = NULL;
int *gMnistTestOutput = NULL;

int main(int argc, char* argv[])
{
	strcpy(gExePath, argv[0]);

	int i, j;
	int LastBackSlash = -1;
	int nLen = strlen(gExePath);
	for(i = nLen-1 ; i >= 0 ; i--)
	{
		if(gExePath[i] == '\\') {
			LastBackSlash = i;
			break;
		}
	}

	if(LastBackSlash >= 0)
		gExePath[LastBackSlash] = 0;

	ReadFile();

	gNetwork.AddLayer(CKhuDaNetLayerOption(KDN_LT_INPUT | KDN_LT_FC, 0, 28*28, 0, 0, 0, 0, 0.15));
	gNetwork.AddLayer(CKhuDaNetLayerOption(KDN_LT_FC, 0, 512, 0, 0, 0, KDN_AF_RELU, 0.15));
	gNetwork.AddLayer(CKhuDaNetLayerOption(KDN_LT_FC, 0, 100, 0, 0, 0, KDN_AF_RELU, 0.15));
	gNetwork.AddLayer(CKhuDaNetLayerOption(KDN_LT_FC | KDN_LT_OUTPUT, 0, 10, 0, 0, 0, KDN_AF_SOFTMAX, 0.15));

	gNetwork.InitWeight();

	printf("%s\n", gNetwork.GetInformation());

	int nBatchCnt = 0;
	int nEpochCnt = 0;
	int nBatch = 100;


	while(1)
	{
		int nIndex = (nBatchCnt*nBatch)%gMnistTrainCnt;

		if(nIndex+nBatch >= gMnistTrainCnt)
			nIndex = gMnistTrainCnt-nBatch;

		int nOutputCnt = 10;

		double **OutputList = new double*[nBatch];
		for(i = 0 ; i < nBatch ; ++i)
			OutputList[i] = new double[nOutputCnt];

		for(i = 0 ; i < nBatch ; ++i)
		{
			for(j = 0 ; j < nOutputCnt ; ++j)
			{
				OutputList[i][j] = 0;
				if(j == gMnistTrainOutput[nIndex+i]) OutputList[i][j] = 1;
			}
		}

		double Loss;
		int nTP = gNetwork.TrainBatch(gMnistTrainInput+nIndex, OutputList, nBatch, &Loss);


		for(i = 0 ; i < nBatch ; ++i)
			delete [] OutputList[i];
		delete [] OutputList;

		nBatchCnt++;

		printf("Train accuracy: %6.2lf, %5.3lf (batch index: %5d, total : %6d(%5.1lf), epoch: %2d\n", (double)nTP/(double)nBatch*100, Loss, nBatchCnt, nIndex+nBatch, (double)(nIndex+nBatch)/gMnistTrainCnt*100, nEpochCnt+1);

		if(nIndex+nBatch == gMnistTrainCnt)
		{
			nEpochCnt++;

			int nTP = 0;
			int i;

			for(i = 0 ; i < gMnistTestCnt ; i++)
			{
				int nResult = gNetwork.Forward(gMnistTestInput[i]);
				if(gMnistTestOutput[i] == nResult) nTP++;
			}

			printf("Test accuracy: %7.3lf\n", (double)nTP/(double)gMnistTestCnt*100.);

			break;
		}
	}

	if(gMnistTrainInput){
		for(int i = 0 ; i < gMnistTrainCnt ; ++i)
			delete [] gMnistTrainInput[i];

		delete [] gMnistTrainInput;
	}

	if(gMnistTrainOutput)
		delete [] gMnistTrainOutput;

	if(gMnistTestInput){
		for(int i = 0 ; i < gMnistTestCnt ; ++i)
			delete [] gMnistTestInput[i];

		delete [] gMnistTestInput;
	}

	if(gMnistTestOutput)
		delete [] gMnistTestOutput;

	return 0;
}

void ReadFile()
{
	gMnistTrainCnt = 60000;
	gMnistTestCnt = 10000;

	gMnistTrainInput = NULL;
	gMnistTrainOutput = NULL;

	gMnistTestInput = NULL;
	gMnistTestOutput = NULL;

	LoadMnistTrain();
	LoadMnistTest();
}

void LoadMnistTrain()
{
	sprintf(gTrainImagePath, "%s\\train-images.idx3-ubyte", gExePath);
	sprintf(gTrainLabelPath, "%s\\train-labels.idx1-ubyte", gExePath);

	int i;

	if(!gMnistTrainInput){
		gMnistTrainInput = new double *[gMnistTrainCnt];

		for(i = 0 ; i < gMnistTrainCnt ; i++)
			gMnistTrainInput[i] = new double[28*28];
	}

	if(!gMnistTrainOutput)
		gMnistTrainOutput = new int [gMnistTrainCnt];


	FILE *fp = fopen(gTrainImagePath, "rb");
	if(fp)
	{
		unsigned char Buf[28*28];
		fread(Buf, 1, 16, fp);

		int nCnt = 0;
		for(i = 0 ; i < gMnistTrainCnt ; ++i)
		{
			fread(Buf, 1, 28*28, fp);

			int k;
			for(k = 0 ; k < 28*28 ; k++)
				gMnistTrainInput[nCnt][k] = (double)Buf[k]/255.;
			nCnt++;
		}
		fclose(fp);
	}

	fp = fopen(gTrainLabelPath, "rb");
	if(fp)
	{
		unsigned char Buf[32];
		fread(Buf, 1, 8, fp);

		int nCnt = 0;
		for(i = 0 ; i < gMnistTrainCnt ; ++i)
		{
			fread(Buf, 1, 1, fp);
			gMnistTrainOutput[nCnt] = Buf[0];
			nCnt++;
		}
		fclose(fp);
	}
}

void LoadMnistTest()
{
	sprintf(gTestImagePath, "%s\\t10k-images.idx3-ubyte", gExePath);
	sprintf(gTestLabelPath, "%s\\t10k-labels.idx1-ubyte", gExePath);

	int i;

	if(!gMnistTestInput){
		gMnistTestInput = new double *[gMnistTestCnt];

		for(i = 0 ; i < gMnistTestCnt ; i++)
			gMnistTestInput[i] = new double[28*28];
	}

	if(!gMnistTestOutput)
		gMnistTestOutput = new int [gMnistTestCnt];

	FILE *fp = fopen(gTestImagePath, "rb");
	if(fp)
	{
		unsigned char Buf[28*28];
		fread(Buf, 1, 16, fp);

		int nCnt = 0;
		for(i = 0 ; i < gMnistTestCnt ; ++i)
		{
			fread(Buf, 1, 28*28, fp);

			int k;
			for(k = 0 ; k < 28*28 ; k++)
				gMnistTestInput[nCnt][k] = (double)Buf[k]/255.;
			nCnt++;
		}
		fclose(fp);
	}

	fp = fopen(gTestLabelPath, "rb");
	if(fp)
	{
		unsigned char Buf[32];
		fread(Buf, 1, 8, fp);

		int nCnt = 0;
		for(i = 0 ; i < gMnistTestCnt ; ++i)
		{
			fread(Buf, 1, 1, fp);
			gMnistTestOutput[nCnt] = Buf[0];
			nCnt++;
		}
		fclose(fp);
	}
}

