#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <math.h>
#define S 3
#define filePath "/shared/table.txt"


double calculateSpeedUp(double parralelTime,int n,int processCount){
	//the speed up is the tserial /tparalel
	//get from the file t serial with the n.
	
	FILE* f = fopen(filePath, "rb");
    
    char line[100];
    
	//read from file
    //The data will be take from the file.
	int Process_Count,nfile;
	double runTime,speedUp,efiicieny;
	double speedUpResult;
	
	
	
	//not make iterate to the first two lines.
    fgets(line, 100, f);
	fgets(line, 100, f);
    while (fgets(line, 100, f))
    {
		 sscanf(line,"%i %i %lf %lf %lf",&Process_Count,&nfile,&runTime,&speedUp,&efiicieny);
		 if(n==nfile&&Process_Count==1) {
			  return (runTime/parralelTime);
		  }
    }
   return processCount;
	
}

void  saveInFile(int Process_Count,int n,double runTime,double speedUp,double efiicieny){
	
	FILE* f = fopen(filePath, "a");
    FILE *fTemp=f;
    while (getc(f)!=EOF){
		fTemp=f;
	}
	fprintf(fTemp,"\n%i %i %lf %lf %lf",Process_Count,n,runTime,speedUp,efiicieny);
	fclose(f);
   
}
int main(int argc, char *argv[])
{
    int Rank;		/* rank of process	*/
	int Process_Count;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;     /* tag for messages	*/
    int n = 0 ; 	
	int i=0;
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for 	*/
	double *data;
	
				

	/* Start up MPI */
	MPI_Init( &argc , &argv );
	
	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &Process_Count);
	
	double startTime,endTime;
	
	
	if(Rank==0){
		printf("Enter n\n");
        scanf("%d", &n);
		startTime=MPI_Wtime();
		
	}
	
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int element_per_process=0;
	if(n%Process_Count!=0){
		element_per_process=(n/Process_Count)+1;
	}
	else element_per_process=(n/Process_Count);
	
	
	int index=0;//index for data array 
	double sum=0.0,result=0.0;
	
	
	for(i=(Rank*element_per_process)+1;i<=(element_per_process)*(Rank+1);i++){
	
		sum+=(1/pow(i,S));
	}
	
	MPI_Reduce(&sum, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 
	
	
	
	//if the
	if(Rank == 0) {
		
		double runTime,speedUp,efficiency;
		double error;
		
		endTime=MPI_Wtime();
		runTime=endTime-startTime;
		if(Process_Count==1){
			speedUp=1,
	        efficiency=1;
		}
		else {
			speedUp=calculateSpeedUp(runTime,n,Process_Count);
	        efficiency=speedUp/Process_Count;
		}
	
	   error = 1.202056903159594 - result;
	   printf("RunTime= %f\n",runTime); 
	   printf("Result = %f\n", result); 
       printf("Error = %f\n", error); 
	   
	   saveInFile(Process_Count,n,runTime,speedUp,efficiency);
	  
    }
	
    MPI_Finalize(); 
    return 0;
}
