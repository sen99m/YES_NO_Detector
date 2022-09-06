// Assignment2.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include<algorithm>
#include<cstdio>

typedef long double ld;
ld computeDCShift();
ld computeNormalizationFactor(FILE *fp);
void computeEnergyZcr(FILE *ip,FILE *op,ld offset,ld factor);
void detectWord();
int frameSize = 100;


ld computeDCShift() {
	char inputFileName[100];
	ld temp,Energy = 0,sampleNumber=0;
	//printf("Enter input file name for DC shift computation\n");
	//scanf("%s",inputFileName);
	//FILE *fp = fopen(inputFileName, "r");
	FILE *fp = fopen("DC_shift.txt", "r");//recorded this file in silence to capture DC shift

	if(!fp) {
		printf("Sorry input file could not be opened\n");
		return 0;
	}

	while (!feof (fp)) {  
		fscanf (fp, "%Lf", &temp); 
		Energy+=temp;
		sampleNumber++;
	}
	Energy/=sampleNumber; //calculating avg energy as dc shift value
    fclose(fp);
	return Energy;
}

ld computeNormalizationFactor(FILE *fp) {
	ld maxEnergy;
	if(!feof (fp))
		fscanf (fp, "%Lf", &maxEnergy); 
	while (!feof (fp)){
		ld temp;
		fscanf (fp, "%Lf", &temp); 
		maxEnergy = std::max(maxEnergy,abs(temp));
	}
	ld factor = (maxEnergy-5000.0)/(maxEnergy*1.0);  
	if(factor<=0)//this can happen when our data is already below 5000. In this case, as the data is already in range hence no need to normalize
		return 1;
	return factor;
}

void computeEnergyZcr(FILE *ip,FILE *op,ld offset,ld factor) {
	ld frameNumber=0;
	ld Energy =0,zcrCount=0;
	int index = 0;
	short flag = 0;
	//fprintf(op, "Frame Number,Energy,ZCR\n"); //printing the heading
	
	while (!feof (ip)) {  //calculating energy,zcr and frame number till the end of the file.
		frameNumber++;
		Energy = 0;
		zcrCount = 0;
		flag = 0;
		int i=0;
		for(i=0;i<frameSize;i++) {   //calculate energy and ZCR from frame number 3. Ignoring first 300 samples as those are mainly silence.
			if(feof (ip)) break;   //if eof before 100 samples in that frame then leave the last frame
			ld temp;
			fscanf (ip, "%Lf", &temp);
			temp = (temp-offset)*factor;   //data cleaning
			Energy+=temp*temp;
			if(i>0) {
				if((flag==1 && temp<0) || (flag==-1 && temp>0))
					zcrCount++;
			}
			if(temp>0)
				flag=1;
			if(temp<0)
				flag=-1;
		}
		if(i>=frameSize && frameNumber>=4) {//ignoring first 3 frames(300 samples) as they majorly contain silence
			Energy/=frameSize;
			fprintf(op, "%Lf %Lf %Lf\n", frameNumber-3,Energy,zcrCount); 
		}
		

	}
}

void detectWord(ld energyArr[],long totaLframes,int *start,int *end) {//start and end are out parameters
	ld baseEnergy = 0.0;
	for(int i=0;i<5;i++)
		baseEnergy+=energyArr[i];
	baseEnergy/=5;// to detece start and end of the word, we are trying to calculate base energy taking first 5 frames(500 samples)
	int factor = 5;
	for(int i=0;i<=totaLframes-3;i++)
		if(energyArr[i]>=factor*baseEnergy && energyArr[i+1]>=factor*baseEnergy && energyArr[i+2]>=factor*baseEnergy) {//if data is above 3x then base energy that means start of the frame
			*start = i;
			break;
		}
	for(int i=totaLframes-1;i>=2;i--) {
		if(energyArr[i]>=factor*baseEnergy && energyArr[i-1]>=factor*baseEnergy && energyArr[i-2]>=factor*baseEnergy) {//if data is below 3x then base energy that means start of the frame
			*end = i;
			break;
		}
	}
}

bool predictWord(ld zcrArr[],int start,int end) {
	long count = 0;
	for(int i=start;i<=end;i++) {
		if(zcrArr[i]>=15)//if zcr is greater than 15 that implies high zcr
			count++;
	}
	printf("\nNumber of frames having high zcr: %lu",count);	
	if(count>=(end-start)/3)//if 30% of the word has zcr greater than 15 that means it is yes ending with noisy "s"
		return true;
	else
		return false;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//computing DC shift
	ld offset = computeDCShift();
	printf("\nComputed offset: %Lf",offset);

	//computing normalisation factor
	//FILE *fp = fopen("Yes2.txt", "r");
	char inputFileName[100];
	printf("\nEnter input file name:\n");
	scanf("%s",inputFileName);
	FILE *fp = fopen(inputFileName, "r");
	ld factor = computeNormalizationFactor(fp);
	printf("\nComputed normalization factor: %Lf",factor);
	
	//Compute energy and ZCR
	rewind(fp);
	FILE *out = fopen("Out.txt", "w");
	
	computeEnergyZcr(fp,out,offset,factor);//ignoring first 3 frames
	fclose(out);

	//detect starting and ending of word
	out = fopen("Out.txt", "r");

	//for convenience we are copying the energy and zcr values in array
	ld energyArr[10000],zcrArr[10000];
	long totaLframes = 0;
	while(!feof (out)) {
		ld frameNumber,energy,zcr;
		fscanf(out,"%Lf",&frameNumber);//frameNumber
		fscanf(out,"%Lf",&energy);//energy
		energyArr[totaLframes] = energy;
		fscanf(out,"%Lf",&zcr);//zcr
		zcrArr[totaLframes++] = zcr;
	}
	
	int start = 0,end = totaLframes-1;
	detectWord(energyArr,totaLframes,&start,&end);
	printf("\nStart frame number of the word: %d",start);
	printf("\nEnd frame number of the word: %d",end);

	//predicting yes or no
	if(predictWord(zcrArr,start,end))
		printf("\nYES\n");
	else
		printf("\nNO\n");

	//closing files & clearing resources
    fclose(fp);
	fclose(out);
	
	system("pause");

	return 0;
}

