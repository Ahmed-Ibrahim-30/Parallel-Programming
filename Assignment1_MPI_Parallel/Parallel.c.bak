#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc , char * argv[])
{
	int Rank;		/* rank of process	*/
	int Process_Count;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for 	*/
				/* recieve		*/

	/* Start up MPI */
	MPI_Init( &argc , &argv );
	
	

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &Process_Count);
	
	 
    
	if( Rank != 0)
	{
		int Student_Count=0; 
		int id_grade=0;
		source=0;
		MPI_Recv(message, 100, MPI_INT, source, tag, MPI_COMM_WORLD, &status );
		sscanf(message, "%d", &Student_Count);
		
		int All_Students[Student_Count][2];
		char Students_information[100][Student_Count];
	    int Passed_Students=0;
		
		int i=0;
		for( ;i<Student_Count;i++)
		{
			MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status );
			char *split;
			int index=0;
			int g;
			char Students_id[100] ;
			for (g = 1, split = strtok(message," "); split != NULL; split = strtok(NULL,","), g++) {
                sscanf(split, "%d", &id_grade);
				All_Students[i][index]=id_grade;
				if(All_Students[i][index]>=60 && index==1){
					snprintf(Students_id, sizeof(Students_id), "%d", All_Students[i][0]);
					strcpy(Students_information[i], Students_id); 
                    strcat(Students_information[i], " Passed the exam "); 
					Passed_Students++;
					sprintf(message,"%s",Students_information[i]);
		            MPI_Send( message, strlen(message)+1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
		 		}
				else if(index==1) {
					snprintf(Students_id, sizeof(Students_id), "%d", All_Students[i][0]);
					strcpy(Students_information[i], Students_id); 
                    strcat(Students_information[i], " Failed .Please Repeat the exam  "); 
					sprintf(message,"%s",Students_information[i]);
		           MPI_Send( message, strlen(message)+1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
				}
				index++;
				
            }	
	
			}
			sprintf(message,"%d",Passed_Students);
		    MPI_Send( message, strlen(message)+1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		
	}else
	{
		FILE* My_File;
		int Student_count=0;
		char File_data[100][100];
		My_File = fopen("/shared/student.txt", "r");
		if (NULL == My_File) { printf("file can't be opened \n");}
        while( fgets (File_data[Student_count++], 100, My_File) ) {}//to get number of students 
		fclose(My_File);
		
		int Process_Students=Student_count/(Process_Count-1),remain=0; //n=31;  count=10  p1=10; p2=10; p3=10; 
        remain=(Student_count-1)-(Process_Students*(Process_Count-1));	
		int w=1;
		for(;w<Process_Count;w++){
			if(w==Process_Count-1 && remain!=0){
				remain+=Process_Students;
				sprintf(message,"%d",remain);
			    MPI_Send( message, strlen(message)+1, MPI_INT, w, tag, MPI_COMM_WORLD);
			}
			else{
				sprintf(message,"%d",Process_Students);
			    MPI_Send( message, strlen(message)+1, MPI_INT, w, tag, MPI_COMM_WORLD);
			}
		}
		int i=0;
		int dest=1,Convert_Between_Cores=Process_Students;
		for (; i<Student_count-1; i++)
        {
           sprintf(message,"%s",File_data[i]);
		   MPI_Send( message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
		   if(i==Convert_Between_Cores-1){
			   dest+=1;
			   if(dest==Process_Count-1 && remain!=0)Convert_Between_Cores+=remain;
			   else Convert_Between_Cores+=Process_Students;
		   }
        }
		int Passed_Students=0,intnum=0;
		for( source = 1; source < Process_Count ; source++)
		{  
			int j=0;
			if(source==Process_Count-1 && remain!=0) Process_Students=remain;
	        for(; j<Process_Students ;j++){
				
				MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status );
				printf("%s\n" , message);
			}
			
			MPI_Recv(message, 100, MPI_INT, source, tag, MPI_COMM_WORLD, &status );
			
			printf("%s" , "process");
			printf(" %d  " , source);
			printf("%s" , "Passed students number: ");
			
			
			 sscanf(message, "%d", &intnum);
			 printf("%d\n" , intnum);
		
			  Passed_Students +=intnum;	
		}
		printf(" Total number of students passed in the exam = %d" , Passed_Students);
		printf(" out of %d\n" ,Student_count-1 );
	}
	/* shutdown MPI */
	MPI_Finalize();
	return 0;
}