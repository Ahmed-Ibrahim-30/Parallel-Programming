#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <omp.h>
#define dataCount 22

//Make a Point Struct that have the (x,y) POint.
struct Point
{
   double x,y;
};

//Read FRom File
void readFromFile(char*fileName,struct Point originalData[dataCount]){

  int it=0;
  char line[100];
  FILE *fp=fopen(fileName,"r");
  //read Line BY Line untill we reach to the dataCount.
  //take double  to make all casses possible of data Type.
  while(fgets(line,100,fp)&&it!=dataCount){
    sscanf(line,"%lf %lf",&originalData[it].x,&originalData[it].y);
    it=it+1;
  }

}


bool check_ditinct_random_value(int randomvalue,int *array,int arr_size){
	int i=0;
	for(;i<arr_size;i++){
		if(randomvalue==array[i])return false;
	}
	return true;
}
//Start Program.
int main(){


  int ThreadsNum,clusterCount,i,j;

  //make first parallel to get the number of thread only.
  #pragma omp parallel
  {
   ThreadsNum=omp_get_num_threads();
  }

  //the number of clusters = number of Thread Count.
  clusterCount=ThreadsNum;


  //originalData-->DataFromFile, cluster_Center-->contain the cluster Centroid Points with number of clusters
  struct Point originalData[dataCount];
  struct Point cluster_Center[clusterCount];

  //clusterGroup--> is the contain each cluster what is the nearst cluster points.
  //In clusterGroup-->Ex: the row0 -->is represent the all nearset points of cluster0
  struct Point clusterGroup[clusterCount][dataCount];

  //each thread will take one Center Cluster.
  struct Point ownCentroidPoint;

  //all Distance will save the all distance between the any clusterCenter with any point.
  //collect the distence from the forked THreads
  int threadId,allDistance[clusterCount][dataCount];


  //read from file
  readFromFile("/shared/Points.txt",originalData);



  //GetRandom -->the intial cluster_Center POints will be take from the originalData (Data FRom File).
//  int Randomvalues[clusterCount]; int random_count=0;
//  for(i=0;i<clusterCount;i++)
//  {
//	int GetRandom = rand() % dataCount;
//	while(!check_ditinct_random_value(GetRandom,&Randomvalues,random_count)){
//		 GetRandom = rand() % dataCount;
//	}
//	Randomvalues[random_count++]=GetRandom;
//	cluster_Center[i]=originalData[GetRandom];
//
//  }


   //get max value in x and y
   int max_x=originalData[0].x,max_y=originalData[0].x;
   
   for (i = 1; i < dataCount; ++i) {
    if (max_x < originalData[i].x) {
      max_x = originalData[i].x;
    }
	if (max_y < originalData[i].y) {
      max_y = originalData[i].y;
    }
  }

  //getrandom out the file
  for(i=0;i<clusterCount;i++)
  {
	int Random_x = rand() % 40+1;
	int  Random_y = rand() % 40+1;


	cluster_Center[i].x=Random_x;
	cluster_Center[i].y=Random_y;

  }




  //declare a  Max Iteration of the Loop to stop.that is in serial region.
  int max=(dataCount/2)+2;



for(i=0;i<max;i++){

  //make a parallel forked threads
  //each thread have ownCentroidPoint ,and threadId

  //-------------------------------------------Step 3 ---------------------------------------------------
  #pragma omp parallel private(ownCentroidPoint,threadId) shared(cluster_Center,allDistance,originalData)
  {

    int it;
    double distince;
    threadId=omp_get_thread_num();

    //each thread will enter --> will take the own Cluster Center by using shared cluster_Center to access.
    ownCentroidPoint.x=cluster_Center[threadId].x;
    ownCentroidPoint.y=cluster_Center[threadId].y;

    //GEt Distinces from own thread clusterCenter to all data(original)

    for(it=0;it<dataCount;it++){
        struct Point point1=ownCentroidPoint;
        struct Point point2=originalData[it];
        //calculate the distince between two points
        distince= sqrt( (point1.x-point2.x)(point1.x-point2.x) + (point1.y-point2.y)(point1.y-point2.y) );
        //update the allDistance with new distence.
        allDistance[threadId][it]=distince;
    }

  }



  //-----------------------------------serial program make Filter Step 4-----------------------------------


  //make inizaliztion of points in clusterGroup
  int k;
  for(k=0;k<clusterCount;k++){
	  for(j=0;j<dataCount;j++){
	     clusterGroup[k][j].x=-5000;
	     clusterGroup[k][j].y=-5000;
	  }
  }

  //get the Minimum value -->Filter each point and put the cluster their points.
  for(k=0;k<dataCount;k++){

    long int min=5555555,index=0;
    for(j=0;j<clusterCount;j++){
       if(min>=allDistance[j][k]) {min=allDistance[j][k];index=j;}
    }
    clusterGroup[index][k]=originalData[k];

  }



  //---------------------------------------------------Step 5----------------------------------------------
  //----------calculate the mean with each cluster Points
  struct Point mean;int sumX,sumY;
  //Parallel Part (Forked Thread)
  //each thread have mean.
  #pragma omp parallel private(mean,threadId,sumX,sumY) shared(clusterGroup,cluster_Center)
  {
    threadId=omp_get_thread_num();
    int i,count=0;
    sumX=0,sumY=0;
    for(i=0;i<dataCount;i++){

     if(clusterGroup[threadId][i].x!=-5000 && clusterGroup[threadId][i].y!=-5000){
      sumX+=clusterGroup[threadId][i].x;
      sumY+=clusterGroup[threadId][i].y;
      count++;
     }
    }

    mean.x=(double)sumX/count;
    mean.y=(double)sumY/count;
    cluster_Center[threadId]=mean;
  }
  //end of the iterartion loop.
}


//-------------------Print FInal Result-------------------------------------------------------
for(i=0;i<clusterCount;i++){
    printf("Cluster Points of clusterId: % d\n",i+1);
    for(j=0;j<dataCount;j++){
      if(clusterGroup[i][j].x!=-5000){printf("( %lf , %lf)\n",clusterGroup[i][j].x,clusterGroup[i][j].y);}
    }
    printf("\n");
}

}