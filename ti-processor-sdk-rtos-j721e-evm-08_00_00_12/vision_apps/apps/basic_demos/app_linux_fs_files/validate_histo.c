/**************************************************************************
*  Copyright 2021   Neither this file nor any part of it may be used for 
*     any purpose without the written consent of D3 Engineering, LLC.
*     All rights reserved.
*				
*				D 3   E N G I N E E R I N G
*
* File Name   :	validate_histo.c
* Release     :	1.0
* Author      :	D3 Engineering Technical Staff
* Created     :	09/21/2021
* Revision    :	1.0
* Description :	To be compiled on the target using gcc to validate histogram output
* 
*		
***************************************************************************
* Revision History
* Ver	Date			Author		Description 
* 1.0	09/29/2021		VC		Initial version	
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define ABS(a)  ((a) > 0 ? (a) : (-(a)) )

void calcHisto(unsigned short *imData, unsigned int *histoData, unsigned int *pMean, int numPixels, int numBins, int shift) {
    
    unsigned long accum;

    accum= 0;

    for (int i=0; i < numBins; i++)
    {
        histoData[i]= 0;
    }

    for (int i = 0; i < numPixels; i++)
    {
        histoData[(imData[i]>>shift)]++;
        accum +=  imData[i];
    }

    *pMean= (unsigned int)(accum/numPixels);
}

unsigned int calcSdFromHisto(unsigned int *histoData, unsigned int mean, int numPixels, int numBins, int shift) {
    
    unsigned long accum;
    unsigned int sd;

    accum= 0;

    for (int i = 0; i < numBins; i++)
    {
        int d= (i<<shift) - mean;
        accum +=  histoData[i]*d*d;
    }

    sd= (unsigned int)sqrt((double)(accum/numPixels));
    return sd;
}

unsigned int calcSdFromImage(unsigned short *imData, unsigned int mean, int numPixels) {
    
    unsigned long accum;
    unsigned int sd;

    accum= 0;

    for (int i = 0; i < numPixels; i++)
    {
        int d= imData[i] - mean;
        accum += d*d;
    }

    sd= (unsigned int)sqrt((double)(accum/numPixels));
    return sd;
}

int main(int argc, char *argv[])
{
    int imSize, histoSize;
    unsigned short *imData;
    unsigned int *histoData, *histoDataNatC;
    unsigned int meanNatC;
    int numBins, numErrors, range;
    int shift= 0;

    FILE *imFile;
    FILE *histoFile;

    if (argc != 4){
        fprintf(stderr,"Usage: validate_histo [range] [path_to_raw_image_file] [path_to_histogram_file] \n");
        exit(-1);
    }

    range= atoi(argv[1]);

    imFile= fopen(argv[2],"rb");
    if (imFile == NULL)
    {
        fprintf(stderr, "Error opening %s\n", argv[2]);
        exit(-1);
    }

    fseek(imFile, 0L, SEEK_END);
    imSize= ftell(imFile);
    rewind(imFile);

    imData= (unsigned short*)malloc(imSize);
    if (imData== NULL)
    {
        fprintf(stderr, "Unable to allocate buffer of size %d\n", imSize);
        exit(-1);
    }

    if(fread(imData, 1, imSize, imFile)!=imSize)
    {
        fprintf(stderr, "Unable to read %d bytes from %s\n", imSize, argv[2]);
        exit(-1);
    }

    fclose(imFile);

    histoFile= fopen(argv[3],"rb");
    if (histoFile == NULL)
    {
        fprintf(stderr, "Error opening %s\n", argv[3]);
        exit(-1);
    }

    fseek(histoFile, 0L, SEEK_END);
    histoSize= ftell(histoFile);
    rewind(histoFile);

    numBins= histoSize/4 - 2; /* last 2 elements are mean and variance */

    while (range > numBins)
    {
        range >>= 1;
        shift++;
    }

    printf("Range is %d and Histogram has %d bins\n", range, numBins);

    histoData= (unsigned int*)malloc(histoSize);
    if (histoData== NULL)
    {
        fprintf(stderr, "Unable to allocate histo buffer of size %d\n", histoSize);
        exit(-1);
    }

    if(fread(histoData, 1, histoSize, histoFile)!=histoSize)
    {
        fprintf(stderr, "Unable to read %d bytes from %s\n", histoSize, argv[3]);
        exit(-1);
    }

    fclose(histoFile);

    histoDataNatC= (unsigned int*)malloc(histoSize);
    if (histoDataNatC== NULL)
    {
        fprintf(stderr, "Unable to allocate histo buffer of size %d\n", histoSize);
        exit(-1);
    }

    calcHisto(imData, histoDataNatC, &meanNatC, imSize/2, numBins, shift);
    unsigned int sdHistoNatC= calcSdFromHisto(histoDataNatC, meanNatC, imSize/2, numBins, shift);
    unsigned int sdImageNatC= calcSdFromImage(imData, meanNatC, imSize/2);

    numErrors= 0;
    unsigned int mean= histoData[numBins];

    if (mean != meanNatC)
    {
        printf("\nError, mean mismatch: %d found but %d expected!\n", mean, meanNatC);
        getchar();
        numErrors++;
    }
    else
    {
        printf("Mean matches !\n");
    }

    unsigned int sd = histoData[numBins+1];

    if (sd != sdImageNatC)
    {
        if (ABS(100 * (sd - sdImageNatC) / sdImageNatC) < 1)
        {
            printf("Warning, small sd mismatch: %d found but %d expected!\n", sd, sdImageNatC);
        }
        else
        {
            printf("Error, sd mismatch: %d found but %d expected!\n", sd, sdImageNatC);
        }
        printf("Press enter to continue validation. Next mismatch will cause the validation to pause again\n");
        getchar();
        numErrors++;
    }
    else
    {
        printf("Sd matches !\n");
    }

    for (int i=0; i < numBins; i++)
    {
        if (histoDataNatC[i] != histoData[i])
        {
            printf("\nError, histograms mismatch @%d, %d found but %d expected!\n", i, histoData[i], histoDataNatC[i]);
            getchar();
            numErrors++;
        }
        else
        {
            printf("%d:%d ", i, histoDataNatC[i]);
        }
    }
    printf("\n");
    if (numErrors != 0)
    {
        printf("Total number of mismatches:%d\n", numErrors);
    }
    else
    {
        printf("Histogram validation successful !\n");
    }
    printf("Mean=%d, Sd=%d\n", mean, sd);
    free(imData);
    free(histoData);
    free(histoDataNatC);
    return 0;
}

