// Student ID: 0516022
// Name      : 吳禹璇
// Date      : 2018.12.19
#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0

int imgWidth, imgHeight;
int MEAN_FILTER_SIZE=9;
int SOBEL_FILTER_SIZE;
int FILTER_SCALE;
int *filter_gx, *filter_gy;
sem_t *tmp;
sem_t g;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputMed_name[5] = {
	"output1.bmp",
	"output2.bmp",
	"output3.bmp",
	"output4.bmp",
	"output5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_mean, *pic_final;

void * apply_mean(void *flag)
{
	int flag_i = *(int*)flag;
	int start, end;
	if(flag_i == 0)
	{
		start = 0;
		end = ceil(imgHeight/2);
	}
	else if(flag_i == 1)
	{
		start = ceil(imgHeight/2);
		end = imgHeight;
	}
	//apply the Mean filter to the image
	for (int j = start; j < end; ++j)
	{
		for (int i = 0; i < imgWidth; ++i)
		{
			int tmp6 = 0;
			int a, b , window[9],k=0,sum=0;
			int ws = (int)sqrt((float)MEAN_FILTER_SIZE);
			for (int jj = 0; jj<ws; ++jj)
			for (int ii = 0; ii<ws; ++ii)
			{
				a = i + ii - (ws / 2);
				b = j + jj - (ws / 2);

				// detect for borders of the image
				if (a<0 || b<0 || a>=imgWidth || b>=imgHeight)continue;

				sum=sum+pic_grey[b*imgWidth + a];
			}

			tmp6=sum/MEAN_FILTER_SIZE;
	
			if (tmp6 < 0) tmp6 = 0;
			if (tmp6 > 255) tmp6 = 255;
			pic_mean[j*imgWidth + i] = tmp6;
			sem_post(&tmp[j*imgWidth + i]);
		}
	}
}

void * apply_sobel(void * )
{
	int start = 0;
	int end = ceil(imgHeight/2);
	//apply the gx_sobel, gy_sobel, sobel filter to the image 
	for (int j = start; j < end; ++j)
	{
		for (int i = 0; i < imgWidth; ++i)
		{
			if(i != imgWidth-1 && j != end-1)
			{
				sem_wait(&tmp[(j+1)*imgWidth + (i+1)]);
				sem_post(&tmp[(j+1)*imgWidth + (i+1)]);
			}
			else if(i == imgWidth-1 && j != end-1)
			{
				sem_wait(&tmp[(j+1)*imgWidth + i]);
				sem_post(&tmp[(j+1)*imgWidth + i]);
			}
			else if(i != imgWidth-1 && j == end-1)
			{
				sem_wait(&tmp[j*imgWidth + (i+1)]);
				sem_wait(&tmp[(j+1)*imgWidth + (i+1)]);
				sem_post(&tmp[j*imgWidth + (i+1)]);
				sem_post(&tmp[(j+1)*imgWidth + (i+1)]);
			}
			else if(i == imgWidth-1 && j == end-1)
			{
				sem_wait(&tmp[j*imgWidth + i]);
				sem_wait(&tmp[(j+1)*imgWidth + i]);
				sem_post(&tmp[j*imgWidth + i]);
				sem_post(&tmp[(j+1)*imgWidth + i]);
			}
			int tmp1 = 0, tmp2 = 0; //gx and gy
			int a, b;
			int ws = (int)sqrt((float)SOBEL_FILTER_SIZE);
			for (int jj = 0; jj<ws; ++jj)
			for (int ii = 0; ii<ws; ++ii)
			{
				a = i + ii - (ws / 2);
				b = j + jj - (ws / 2);

				// detect for borders of the image
				if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

				tmp1 += filter_gx[jj*ws + ii] * pic_mean[b*imgWidth + a];
				tmp2 += filter_gy[jj*ws + ii] * pic_mean[b*imgWidth + a];
			};
			if (tmp1 < 0) tmp1 = 0;
			if (tmp1 > 255) tmp1 = 255;
			if (tmp2 < 0) tmp2 = 0;
			if (tmp2 > 255) tmp2 = 255;

			int tmp3 = 0; //sobel
			tmp3 = sqrt(tmp1*tmp1 + tmp2*tmp2);
			if (tmp3 < 0) tmp3 = 0;
			if (tmp3 > 255) tmp3 = 255;

			//extend the size form WxHx1 to WxHx3
			pic_final[3 * (j*imgWidth + i) + MYRED] = tmp3;
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = tmp3;
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = tmp3;
		}
	}
}

void * apply_sobel2(void * )
{
	int start = ceil(imgHeight/2);
	int end = imgHeight;
	//apply the gx_sobel, gy_sobel, sobel filter to the image 
	for (int j = start; j < end; ++j)
	{
		for (int i = 0; i < imgWidth; ++i)
		{
			if(i != imgWidth-1 && j == start)
			{
				sem_wait(&tmp[(j-1)*imgWidth + (i+1)]);
				sem_wait(&tmp[(j+1)*imgWidth + (i+1)]);
				sem_post(&tmp[(j-1)*imgWidth + (i+1)]);
				sem_post(&tmp[(j+1)*imgWidth + (i+1)]);
			}
			if(i == imgWidth-1 && j == start)
			{
				sem_wait(&tmp[(j-1)*imgWidth + i]);
				sem_wait(&tmp[(j+1)*imgWidth + i]);
				sem_post(&tmp[(j-1)*imgWidth + i]);
				sem_post(&tmp[(j+1)*imgWidth + i]);
			}
			if(i != imgWidth-1 && j != end-1 && j != start)
			{
				sem_wait(&tmp[(j+1)*imgWidth + (i+1)]);
				sem_post(&tmp[(j+1)*imgWidth + (i+1)]);
			}
			else if(i == imgWidth-1 && j != end-1 && j != start)
			{
				sem_wait(&tmp[(j+1)*imgWidth + i]);
				sem_post(&tmp[(j+1)*imgWidth + i]);
			}
			else if(i != imgWidth-1 && j == end-1)
			{
				sem_wait(&tmp[j*imgWidth + (i+1)]);
				sem_post(&tmp[j*imgWidth + (i+1)]);
			}
			else if(i == imgWidth-1 && j == end-1)
			{
				sem_wait(&tmp[j*imgWidth + i]);
				sem_post(&tmp[j*imgWidth + i]);
			}
			int tmp1 = 0, tmp2 = 0; //gx and gy
			int a, b;
			int ws = (int)sqrt((float)SOBEL_FILTER_SIZE);
			for (int jj = 0; jj<ws; ++jj)
			for (int ii = 0; ii<ws; ++ii)
			{
				a = i + ii - (ws / 2);
				b = j + jj - (ws / 2);

				// detect for borders of the image
				if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

				tmp1 += filter_gx[jj*ws + ii] * pic_mean[b*imgWidth + a];
				tmp2 += filter_gy[jj*ws + ii] * pic_mean[b*imgWidth + a];
			};
			if (tmp1 < 0) tmp1 = 0;
			if (tmp1 > 255) tmp1 = 255;
			if (tmp2 < 0) tmp2 = 0;
			if (tmp2 > 255) tmp2 = 255;

			int tmp3 = 0; //sobel
			tmp3 = sqrt(tmp1*tmp1 + tmp2*tmp2);
			if (tmp3 < 0) tmp3 = 0;
			if (tmp3 > 255) tmp3 = 255;

			//extend the size form WxHx1 to WxHx3
			pic_final[3 * (j*imgWidth + i) + MYRED] = tmp3;
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = tmp3;
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = tmp3;
		}
	}
}

void * grey1(void * )
{
	for (int j = 0; j<imgHeight/2+1; ++j)
	{
		for (int i = 0; i<imgWidth; ++i)
		{
			int tmp5 = (
					pic_in[3 * (j*imgWidth + i) + MYRED] +
					pic_in[3 * (j*imgWidth + i) + MYGREEN] +
					pic_in[3 * (j*imgWidth + i) + MYBLUE] )/3;

			if (tmp5 < 0) tmp5 = 0;
			if (tmp5 > 255) tmp5 = 255;
			pic_grey[j*imgWidth + i] = tmp5;

			sem_init(&tmp[j*imgWidth + i], 0, 0);
		}
	}
}

void * grey2(void * )
{
	for (int j = imgHeight/2+1; j<imgHeight; ++j)
	{
		for (int i = 0; i<imgWidth; ++i)
		{
			int tmp5 = (
					pic_in[3 * (j*imgWidth + i) + MYRED] +
					pic_in[3 * (j*imgWidth + i) + MYGREEN] +
					pic_in[3 * (j*imgWidth + i) + MYBLUE] )/3;

			if (tmp5 < 0) tmp5 = 0;
			if (tmp5 > 255) tmp5 = 255;
			pic_grey[j*imgWidth + i] = tmp5;

			sem_init(&tmp[j*imgWidth + i], 0, 0);
		}
	}
	sem_post(&g);
}

int main()
{
	// read mask file
	FILE* mask;

	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &SOBEL_FILTER_SIZE);

	filter_gx = new int[SOBEL_FILTER_SIZE];
	filter_gy = new int[SOBEL_FILTER_SIZE];

	for (int i = 0; i<SOBEL_FILTER_SIZE; ++i)
		fscanf(mask, "%d", &filter_gx[i]);
	for (int i = 0; i<SOBEL_FILTER_SIZE; ++i)
		fscanf(mask, "%d", &filter_gy[i]);

	fclose(mask);
	
	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k < 5; ++k)
	{
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		
		//define semaphore array and initial
		tmp = new sem_t[imgWidth*imgHeight];

		// allocate space for output image
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_mean = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));

		sem_init(&g, 0, 0);
	
		pthread_t grey1_t, grey2_t;
		pthread_create(&grey1_t, NULL, grey1, NULL);
		pthread_create(&grey2_t, NULL, grey2, NULL);

		pthread_join(grey1_t, NULL);
		pthread_join(grey2_t, NULL);

		sem_wait(&g);

		pthread_t mean_thread1, mean_thread2, sobel_thread1, sobel_thread2;
		int up = 0, down = 1;
        pthread_create(&mean_thread1, NULL, apply_mean, &up);
        pthread_create(&sobel_thread1, NULL, apply_sobel, NULL);
        pthread_create(&mean_thread2, NULL, apply_mean, &down);
        pthread_create(&sobel_thread2, NULL, apply_sobel2, NULL);
        
        pthread_join(mean_thread1, NULL);
        pthread_join(sobel_thread1, NULL);
        pthread_join(mean_thread2, NULL);
        pthread_join(sobel_thread2, NULL);

		bmpReader->WriteBMP(outputMed_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_mean);
		free(pic_final);
	}

	return 0;
}