#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <omp.h>
#define N 12
#define CHUNKSIZE 2




int main(int argc, char *argv[])
{
	
	int Rank;		/* rank of process	*/
	int Process_Count;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for 	*/
				/* recieve		*/
	int i;
	int Number_of_bars, Number_of_points,element_per_peocess,THREADS,max_value,Par_length;
	/* Start up MPI */
	MPI_Init( &argc , &argv );
	
	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &Process_Count);
	
	int data[100];
	int *File_data=NULL;
	int *Pars=NULL;
	int *Pars_final=NULL;
	int g,par_length_temp,index=0;
	


	if(Rank==0){
		printf("Enter Number of bars: ");
		scanf("%d", &Number_of_bars);
	    printf("Enter Number of points: ");
        scanf("%d", &Number_of_points);
		//printf("Number of bars: %d\n",Number_of_bars);
		//printf("Number of points: %d\n",Number_of_points);
		//printf("Number of processess: %d\n",Process_Count);
		
		if(Number_of_points%Process_Count!=0){
			element_per_peocess=(Number_of_points/Process_Count)+1;
		}
		else element_per_peocess=(Number_of_points/Process_Count);
		
		
		
		File_data=malloc (element_per_peocess*Process_Count * sizeof(int));
		for(i=0;i<element_per_peocess*Process_Count;i++){
		    File_data[i]=-1;
		}
		
		
		FILE* My_File=fopen("/shared/dataset.txt", "r");;
		char ch;
		int num=0; 
		if (NULL == My_File) { printf("file can't be opened \n");}
        while( fgets (&ch, 100, My_File) ) {
			File_data[num] = atoi(&ch);  
			num++;
		}//to get number of students 		
		fclose(My_File);
		
		max_value=File_data[0];
		for (i = 1; i < Number_of_points; ++i) {
           if (max_value < File_data[i]) {
             max_value = File_data[i];
           }
        }
		
		if(max_value%Number_of_bars!=0){
			Par_length=(max_value/Number_of_bars)+1;
		}
		else Par_length=(max_value/Number_of_bars);
		
	
	}
	
	MPI_Bcast(&element_per_peocess, 1, MPI_INT, 0, MPI_COMM_WORLD);//to inform all process for value of "fre"
	MPI_Bcast(&Number_of_bars, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	#pragma omp parallel
    {
		THREADS = omp_get_num_threads();
    }
	
	int element_per_thread=0;
	if(element_per_peocess%THREADS!=0){
		element_per_thread=(element_per_peocess/THREADS)+1;
	}
	else element_per_thread=(element_per_peocess/THREADS);
	
	int rec_size=0;
	if(element_per_peocess%element_per_thread!=0){
		rec_size=(element_per_peocess/element_per_thread)+1;
	}
	else rec_size=(element_per_peocess/element_per_thread);
	rec_size=rec_size*element_per_thread;
	
	
	int *rec = malloc (rec_size * sizeof(int));
	for( i = 0 ; i < rec_size ; i++){  rec[i] = -1 ;  }
	
	
	MPI_Bcast(&Par_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
	Pars=malloc (Number_of_bars * sizeof(int));
	Pars_final=malloc (Number_of_bars * Process_Count * sizeof(int));
	for(i=0;i<Number_of_bars;i++){  Pars[i]=0;  }
	
	MPI_Scatter(File_data, element_per_peocess, MPI_INT, rec, element_per_peocess, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	
	#pragma omp parallel shared(Pars,Par_length,Number_of_bars,index) private(par_length_temp,g,i)
    {
		for(i=0;i<element_per_thread;i++){
			par_length_temp=Par_length;
			if(rec[index] == -1 ) break;
            for(g=0;g<Number_of_bars;g++){
				if(rec[index]<=par_length_temp) {
					
					Pars[g]++; 
					//if(Rank==1)printf(" data =  %d  \t par_length_temp =  %d\n",rec[index],Pars[g]);
					break;
					
				}
				par_length_temp+=Par_length;
			}			
			index++;
		}
    }
	
	MPI_Gather(Pars, Number_of_bars, MPI_INT, Pars_final, Number_of_bars, MPI_INT, 0, MPI_COMM_WORLD);
	if(Rank==0){
		
		int arr_count=0;
		int final_count[Number_of_bars];
		for( i = 0 ; i < Number_of_bars ; i++){  final_count[i] = 0 ;  }
		
		int d=0,temp=Number_of_bars;
		
		for(g=0;g<Number_of_bars*Process_Count;g++){
		    //printf("  count = %d   \t",Pars_final[g]);
			final_count[g-d]+=Pars_final[g]; //d: 21-20 =index 0 1 2 3 4 5
		    if(g==temp-1) { d+=Number_of_bars; temp+=Number_of_bars;}
		}
		
		int startlen=0,endlen=Par_length;
		
		for(g=0;g<Number_of_bars ;g++){
		    printf("The range starts with %d , end with %d with count = %d   \n",startlen,endlen,final_count[g]);
		    startlen=endlen+1;
			endlen+=Par_length;
	    }
		printf("\n");
		
	}
	
    
	
	/* shutdown MPI */
	MPI_Finalize();
	return 0;

}
