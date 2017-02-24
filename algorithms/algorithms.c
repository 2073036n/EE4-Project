#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

//simulink

const int MAX_FEATURE=24;
const int MAX_FEATURE_PER_IMAGE = 100;

int compareFeatures(int *features,int distances[],int *fdcount,
	int *previousFeatures,int previousDistances[],int *previousfdcount){
	int j;
	int smallest;
	int pos=-1;
	for(j=0;j<(*previousfdcount);j++){
		// printf("if %i is ~ %i\n",features[*fdcount-1],(previousFeatures[j]) );
		if(distances[*fdcount-1]-previousDistances[j]>0 &&
			smallest>(abs(features[*fdcount-1]-previousFeatures[j])) &&
			(features[*fdcount-1]>((previousFeatures[j]-3))) && 
			(features[*fdcount-1]<((previousFeatures[j]+3)))){
			smallest=abs(features[*fdcount-1]-previousFeatures[j]);
			pos=j;
		}
		// if( (features[*fdcount-1]>((previousFeatures[j]-3))) && (features[*fdcount-1]<((previousFeatures[j]+3))) ){
		// 	previousFeatures[j]=-3;
	}
	if(pos==-1) return 0;
	return distances[*fdcount-1]-previousDistances[pos];
}

int newFeature(int i){
	return i;
}

void addToFeature(int i,int *currentFeature,int *fcount,int *features,int distances[],int *fdcount,
	int count,int *previousFeatures,int previousDistances[],int *previousfdcount){
	if((*fcount)>0){
		*currentFeature+=i;
		*fcount=*fcount-1;
	}
	else{
		if(*fdcount<MAX_FEATURE_PER_IMAGE){
			features[*fdcount]=*currentFeature;
			distances[*fdcount]=(count+1)-MAX_FEATURE;
			*currentFeature=0;
			*fdcount+=1;
			*fcount=MAX_FEATURE-1;
			int result =compareFeatures(features,distances,fdcount,previousFeatures,previousDistances,previousfdcount);
			if(result>0) 
				printf("%i\n",result);
		}
	}
}

int main(int argc, char *argv[]){
	char *path = "/home/timothy/workspace/TDP4/test_4.txt";
	FILE *inFile = fopen(path,"r");
	if(inFile==NULL){
		printf("error opening file\n");
		return 1;
	}
	int count = 0;
	char *buffer = malloc(33);
	int featuresA[MAX_FEATURE_PER_IMAGE];
	int distancesA[MAX_FEATURE_PER_IMAGE];
	int featuresB[MAX_FEATURE_PER_IMAGE];
	int distancesB[MAX_FEATURE_PER_IMAGE];
	int acount = 0;
	int bcount = 0;
	int aOrB = 0; //0 represents image A and 1 image B
	int currentFeature = 0;
	int fcount = MAX_FEATURE-1;
	while(fgets(buffer,33,inFile)!=NULL){
		if((count++)==768){
			printf("\n-------------------\n\n");
			count=0;
			fcount=MAX_FEATURE-1;
			if(aOrB==0){
				aOrB=1;
				bcount=0;
			}
			else{
				aOrB=0;
				acount=0;
			}
		}
		int i = atoi(buffer);
		if(i>12000000){
			i=5;
		}
		else if(i>10000000){
			i=4;
		}
		else if(i>8000000){
			i=3;
		}
		else if(i>7000000){
			i=1;
		}
		else{
			i=0;
		}
		if(currentFeature==0){
			currentFeature = newFeature(i);
			fcount--;
		}
		else{
			if(aOrB==0){
				addToFeature(i,&currentFeature,&fcount,featuresA,distancesA,&acount,count,featuresB,distancesB,&bcount);
			}
			else{
				addToFeature(i,&currentFeature,&fcount,featuresB,distancesB,&bcount,count,featuresA,distancesA,&acount);
			}
		}
	}
	// int j;
	// for(j=0;j<pcount;j++){
	// 	printf("%i at %i\n",pointers[j],distances[j]);
	// }
	// close(inFile);
	return 0;
}