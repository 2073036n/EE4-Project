#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>


const int MAX_FEATURE=48; //Maximum size of a single feature (for memory buffer reasons)
const int MAX_FEATURE_PER_IMAGE = 16; //Maximum number of features that can be stored per image
const int BLANK_GAP_SIZE_IN_FEATURE = 8; //Maximum number of blank spaces that can apear between light spots in a single feature.


/*
* loops through the previous image's array of features and compares the current feature to each, returning the displacement
* of matching features if it is positive.
*/
int compareFeatures(int *features,int distances[],int *fdcount,
	int *previousFeatures,int previousDistances[],int *previousfdcount){
	int j;
	for(j=0;j<(*previousfdcount);j++){
		if(features[*fdcount-1]>((previousFeatures[j]-10)) && 
			(features[*fdcount-1]<((previousFeatures[j]+10)))) {
			if(distances[*fdcount-1]-previousDistances[j]>0){
				// printf("matched with %i\n",previousFeatures[j] );
				previousFeatures[j]=-10;
				return distances[*fdcount-1]-previousDistances[j];
			}
		}
		// if( (features[*fdcount-1]>((previousFeatures[j]-3))) && (features[*fdcount-1]<((previousFeatures[j]+3))) ){
		// 	previousFeatures[j]=-3;
	}
	return 0;
}

/*
* stores the current feature sum in the features array for this image then resets the variables to collect a new feature.
*/
void endFeature(int *currentFeature,int *fcount,int *features,int distances[],int *fdcount,
	int count,int *previousFeatures,int previousDistances[],int *previousfdcount){
	features[*fdcount]=*currentFeature;
	distances[*fdcount]=(count+1)-(*fcount);
	// printf("feature: %i\n",*currentFeature);
	*currentFeature=0;
	*fdcount+=1;
	*fcount=MAX_FEATURE-1;
	int result =compareFeatures(features,distances,fdcount,previousFeatures,previousDistances,previousfdcount);
	if(result>0) 
		printf("%i\n",result);
}

/*
* Adds argument i to the current feature unless the feature is full in which case it ends the feature.
*/
void addToFeature(int i,int *currentFeature,int *fcount,int *features,int distances[],int *fdcount,
	int count,int *previousFeatures,int previousDistances[],int *previousfdcount){
	if((*fcount)>0){
		*currentFeature+=i;
		*fcount=*fcount-1;
	}
	else{
		endFeature(currentFeature,fcount,features,distances,fdcount,count,previousFeatures,previousDistances,previousfdcount);
	}
}

int main(int argc, char *argv[]){
	//Read in file with bit stream in it
	char *path = "/home/timothy/workspace/TDP4/image_arrays.txt";
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
	int gap;
	int acount = 0;
	int bcount = 0;
	int aOrB = 0; //0 represents image A and 1 image B
	int currentFeature = 0;
	int smooth =0;
	int fcount = MAX_FEATURE-1;
	while(fgets(buffer,33,inFile)!=NULL){
		if((count++)==768){
			printf("\n-------------------\n\n");
			count=0;
			smooth=0;
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
		int smooth = atoi(buffer);
		if(smooth>12000000){
			smooth=8;
		}
		else if(smooth>11000000){
			smooth=6;
		}
		else if(smooth>10000000){
			smooth=4;
		}
		else if(smooth>9000000){
			smooth=2;
		}
		else{
			smooth=0;
		}
		if(smooth>1){
			if(currentFeature==0){
				currentFeature = smooth;
				gap=BLANK_GAP_SIZE_IN_FEATURE;
				fcount--;
			}
			else{
				if(aOrB==0){
					addToFeature(smooth,&currentFeature,&fcount,featuresA,distancesA,&acount,count,featuresB,distancesB,&bcount);
					gap=BLANK_GAP_SIZE_IN_FEATURE;
				}
				else{
					addToFeature(smooth,&currentFeature,&fcount,featuresB,distancesB,&bcount,count,featuresA,distancesA,&acount);
					gap=BLANK_GAP_SIZE_IN_FEATURE;
				}
			}
		}
		else{
			if(currentFeature!=0){
				if((gap--)<=0){
					if(aOrB==0){
						endFeature(&currentFeature,&fcount,featuresA,distancesA,&acount,count,featuresB,distancesB,&bcount);
					}
					else{
						endFeature(&currentFeature,&fcount,featuresB,distancesB,&bcount,count,featuresA,distancesA,&acount);
					}
				}
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