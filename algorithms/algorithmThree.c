#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>


const int MAX_FEATURE=16; //Maximum size of a single feature (for memory buffer reasons)
const int MAX_FEATURE_PER_IMAGE = 8; //Maximum number of features that can be stored per image
const int BLANK_GAP_SIZE_IN_FEATURE = 0; //Maximum number of blank spaces that can apear between light spots in a single feature.
const int SPEED_CHANGE_PER_IMAGE = 24;


void sortArray(int *array,int size){
	int i;
	int j;
	for(i=0;i<size;i++){
		for(j=i;j<size;j++){
			if(array[i]>array[j]){
				int temp = array[i];
				array[i]=array[j];
				array[j]=temp;
			}
		}
	}
}

/*
* loops through the previous image's array of features and compares the current feature to each, returning the displacement
* of matching features if it is positive.
*/
int compareFeatures(int *features,int distances[],int *fdcount,
	int *previousFeatures,int previousDistances[],int *previousfdcount,int currentDisp){
	int j;
	for(j=0;j<(*previousfdcount);j++){
		int jDisplacement = distances[*fdcount-1]-previousDistances[j];
		if((jDisplacement-currentDisp < SPEED_CHANGE_PER_IMAGE) && (jDisplacement-currentDisp) > -SPEED_CHANGE_PER_IMAGE){
			if(features[*fdcount-1]>((previousFeatures[j]-10)) && 
				(features[*fdcount-1]<((previousFeatures[j]+10)))) {
				return distances[*fdcount-1]-previousDistances[j];
			}
		}
		// if( (features[*fdcount-1]>((previousFeatures[j]-3))) && (features[*fdcount-1]<((previousFeatures[j]+3))) ){
		// 	previousFeatures[j]=-3;
	}
	return NULL;
}

/*
* stores the current feature sum in the features array for this image then resets the variables to collect a new feature.
*/
void endFeature(int *currentFeature,int *fcount,int *features,int distances[],int *fdcount, int count,
	int *previousFeatures,int previousDistances[],int *previousfdcount, int displacements[], int *dispCount,int currentDisp){
	if(*fdcount<MAX_FEATURE_PER_IMAGE){
		features[*fdcount]=*currentFeature;
		distances[*fdcount]=(count+1)-(*fcount);
		// printf("feature: %i\n",*currentFeature);
		*currentFeature=0;
		*fdcount+=1;
		*fcount=MAX_FEATURE-1;
		int result =compareFeatures(features,distances,fdcount,previousFeatures,previousDistances,previousfdcount,currentDisp);
		if(result!=NULL){
			if(*dispCount<MAX_FEATURE_PER_IMAGE) 
				// printf("result = %i\n",result);
				displacements[*dispCount]=result;
				*dispCount+=1;
		}
	}
}

/*
* Adds argument i to the current feature unless the feature is full in which case it ends the feature.
*/
void addToFeature(int i,int *currentFeature,int *fcount,int *features,int distances[],int *fdcount,
	int count,int *previousFeatures,int previousDistances[],int *previousfdcount, int displacements[], int *dispCount,int currentDisp){
	if((*fcount)>0){
		*currentFeature+=i;
		*fcount=*fcount-1;
	}
	else{
		endFeature(currentFeature,fcount,features,distances,fdcount,count,previousFeatures,previousDistances,previousfdcount,displacements,dispCount,currentDisp);
	}
}

int main(int argc, char *argv[]){
	//Read in file with bit stream in it
	char *path = "/home/timothy/workspace/TDP4/vid_3.txt";
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
	int currentFeature = 20;
	int i =0;
	int fcount = MAX_FEATURE-1;
	int currentDisp = 0;
	int displacements[MAX_FEATURE_PER_IMAGE];
	int dispCount = 0;

	while(fgets(buffer,33,inFile)!=NULL){
		if((count++)==128){
			if(dispCount>0){
				sortArray(displacements,dispCount);
				int c;
				int newDisp = 0;
				int trimmed = (int)((dispCount-1)/5);
				for(c=trimmed;c<dispCount-trimmed;c++){
					newDisp+=displacements[c];
				}
				newDisp=newDisp/(dispCount-(2*trimmed));

				printf("%i\n",currentDisp);
				currentDisp=newDisp;
				dispCount=0;
			}

			// printf("\n-------------------\n\n");
			count=0;
			// i=0;
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
		if(i>13000000){
			i=4;
		}
		else if(i>11000000){
			i=3;
		}
		else if(i>10000000){
			i=2;
		}
		else if(i>9000000){
			i=1;
		}
		else{
			i=0;
		}

		if(i>0){
			if(currentFeature==0){
				currentFeature = i;
				gap=BLANK_GAP_SIZE_IN_FEATURE;
				fcount--;
			}
			else{
				if(aOrB==0){
					addToFeature(i,&currentFeature,&fcount,featuresA,distancesA,&acount,count,featuresB,distancesB,&bcount,displacements,&dispCount,currentDisp);
					gap=BLANK_GAP_SIZE_IN_FEATURE;
				}
				else{
					addToFeature(i,&currentFeature,&fcount,featuresB,distancesB,&bcount,count,featuresA,distancesA,&acount,displacements,&dispCount,currentDisp);
					gap=BLANK_GAP_SIZE_IN_FEATURE;
				}
			}
		}
		else{
			if(currentFeature!=0){
				if((gap--)<=0){
					if(aOrB==0){
						endFeature(&currentFeature,&fcount,featuresA,distancesA,&acount,count,featuresB,distancesB,&bcount,displacements,&dispCount,currentDisp);
					}
					else{
						endFeature(&currentFeature,&fcount,featuresB,distancesB,&bcount,count,featuresA,distancesA,&acount,displacements,&dispCount,currentDisp);
					}
				}
			}
		}
	}
	return 0;
}