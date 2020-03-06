/* 
  Parallel computing ___ Homework#1
  Done by: Seham Nasr & Mais Tawalbeh  
 ---------------------------------------------------------------------
 General information:    
 -This program aims to calculate simple Matrices multiplication .
 -Our simple equation is: C = AxB .
 -A,B & C are (128x128) Matrices .
 */

#include "mpi.h"            /* Needed for MPI libraries */
#include <stdio.h>          /* Needed for C libraries */
#include <stdlib.h>         /* Needed for C libraries */
#include "time.h"

/* Control Matrices size */ 
#define Arow  512
#define Acol  512
#define Brow  512
#define Bcol  512
int  A[Arow][Acol], B[Brow][Bcol], C[Arow][Bcol];   // Matrices decleration  
double Sclock,Pclock; // For sequential & parallel wall clock time 

int main(int argc , char* argv[]){

double start, finish; clock_t end1,begin1; /* Used for measure time algorithm, for free :P */ 
int ca, cm, sp, col, rp;   /* Shown below */

/* Initializing all elements of resulted matrix (C) to 0 */
int c1, c2;
    for(c1 = 0; c1< Arow; c1++)
        for(c2 = 0; c2 < Bcol; c2++)
            C[c1][c2] = 0;    

MPI_Status s;   /* Struct used to get information about MPI_Recv */ 
MPI_Init(NULL , NULL);   /* Initialize MPI */

/* Return # of processes in this program */
int nop;
MPI_Comm_size(MPI_COMM_WORLD , &nop);

/* Return process rank */
int pr;
MPI_Comm_rank(MPI_COMM_WORLD , &pr);
 
/* Test the Number of entered processes */
if (nop < 2) {
  printf("Need at least two MPI processes, one master & one worker.\nQuitting...\n");
  MPI_Abort(MPI_COMM_WORLD, 0);
  exit(1);
  }
else if (nop > 8){
printf("Maxmum number of process you can use is 8 processes.\nQuitting...\n");
  MPI_Abort(MPI_COMM_WORLD, 0);
  exit(1);
}

/* Determine master process which rank is 0 */
if(pr == 0){  /* Starting master process tasks */  
   printf("Hello...\nAll processes in this program = %d\nIam Master process with rank 0\nNumber of worker processes = %d\n\n",nop,nop-1);

     /* First task: Filling matrix A: each row from 0 - 127 */ 
        int r,c;
        for ( r = 0; r < Arow; r++){    
             for (c = 0; c < Acol; c++){
                  A[r][c]=r;
             }
        }
     /* Second task: Filling matrix B: each row from 0 - 127 */
        for (r = 0; r < Brow; r++)
             for (c = 0; c < Bcol; c++)
                   B[r][c] = c;

     /* Third task: Divided matrix B between processes */
        ca = Bcol/(nop-1);  /* Average colunm */          
        cm = Bcol%(nop-1);  /* Additional colunm */ 
        sp = 0;             /* Starting point of matrix block for each process */       
 
     /* Fourth task: Sending data for workers */ 
        for (rp = 1; rp <= (nop-1); rp++){ /* starting master sending loop */
            /* Determine # of colunms for process, in case we have additional colunms */
              if (cm != 0){       
                  col = ca+1; 
                  cm--;
              }                         
            /* Determine # of colunms for process, in case we dont have additional colunms */
         else 
            col = ca;

        int  bs [Brow][col]; /* This buffer used to store divided part from B for each process */
        int r1, c1;  
        /* store each worker process data from B into bs */
           for (r1 = 0 ; r1 < Brow; r1++)
                for(c1 = 0; c1 < col; c1++ )
                     bs[r1][c1] = B[r1][sp+c1];

        //MPI_Wtime():This function return the time in seconds 
         start = MPI_Wtime();  /* Measure starting time */
        
        /* 0: tag for messages sent by master process to worker processes */
           MPI_Send(&col, 1, MPI_INT, rp, 0, MPI_COMM_WORLD);         /* Send # of colunms */
           MPI_Send(&sp, 1, MPI_INT, rp, 0, MPI_COMM_WORLD);          /* Send starting point */
           MPI_Send(&bs, Brow*col, MPI_INT, rp, 0, MPI_COMM_WORLD);   /* Send actual divided data */
           MPI_Send(&A, Arow*Acol, MPI_INT, rp, 0, MPI_COMM_WORLD);   /* Send Matrix A as is */


         sp = sp + col;         /* Increment starting point for next worker by previous worker # of colunms */    
         }/* End master sending loop */
     

      /* Fifth task: Waiting multiplication result from workers */
         for (rp = 1; rp <= (nop-1); rp++){     /* Starting master recived result loop */

        /* 1: tag for messages sent by master process to worker processes */
         MPI_Recv(&col, 1, MPI_INT, rp, 1, MPI_COMM_WORLD, &s);          /* Recive # of colunms */
         int Crecv[Brow][col];                                           /* This buffer used to store each process result */
         MPI_Recv(&sp, 1, MPI_INT, rp, 1, MPI_COMM_WORLD, &s);           /* Recive starting point */
         MPI_Recv(&Crecv, Brow*col, MPI_INT, rp, 1, MPI_COMM_WORLD, &s); /* Recive actual data */ 

         /* Store each worker process result in C */
         int r2, c2;
         for (r2 = 0 ; r2 < Brow; r2++)
             for(c2 = 0; c2 < col; c2++ )
                 C[r2][sp+c2] = Crecv[r2][c2];
         } /* End of master recived result loop */
     
    
  
finish = MPI_Wtime();   /* Measure finishing time */

int checkSum = 0; 
      /* Sixth task: Printing the Result (C) */ 
         printf ("AxB = C ...\nThe content of C:\n ");
         for (r = 0 ; r< Arow; r++){
              for(c = 0; c < Bcol; c++){
                   printf("%d  ",C[r][c]);
                   checkSum = checkSum ^ C[r][c] ;   /* Just for doctor check ^_^ */
              } 
              printf("\n");
         }


//measuring wall time
Pclock=finish - start;
printf("\n\n\n Details : \n");
printf("----------------------------------------------------------------------------\n");
printf("* Parallel elaplsed time = %f seconds.\n", Pclock);

/* ***** Sequential calculating ******* */
//______________________________________//

//multiplying matrecies
int sum=0,checksum1=0,CS[Arow][Bcol],i,j,k;
double ss=MPI_Wtime();
   for( i=0;i<Arow;i++){
     for( j=0;j<Bcol;j++){
	    for( k=0;k<Brow;k++){
		   sum=sum+A[i][k]*B[k][j];
                   
	}
	CS[i][j]=sum;
        checksum1=checksum1^sum;
	sum=0;
     }}
double f = MPI_Wtime();   /* Measure finishing time */
Sclock=f-ss;
printf("* Serial Elapsed time= %f seconds\n", Sclock);
printf("\n* parallel CheckSum value = %d\n",checkSum);
printf("* Serial CheckSum value = %d\n",checksum1);

if(checkSum==checksum1)
{
   printf("The checksum in sequential & parallel is equal ... Successfully checked\n");
}
else
   printf("The checksum in sequential & parallel is not equal ... Failed\n");

double speedup= Sclock/Pclock;  // Compute how much the improvement in speed, in parallel system  compared with sequential system  
double efficiency = Sclock/(Pclock*nop); //the improvement we get per contributing process . (speedup / nop)

printf("__________________________________________________________________________\n");
printf("Absoulte Speedup = %f \n",speedup);
printf("Absoulte efficiency = %f \n",efficiency);
printf("__________________________________________________________________________\n");
/*
When the terms efficiency and speedup are used without qualifiers,
they usually refer to absolute efficiency and absolute speedup, respectively.  
*/
} /* End master process tasks */ 


/* Determine worker process */
 if(pr > 0){  /* Starting master process tasks */
     /* First task: Recive data from master */  
        MPI_Recv(&col, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &s);        /* Recive # of colunms */     
        int br[Brow][col];                                           /* This buffer used to store each worker process part of B */
        MPI_Recv(&sp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &s);         /* Recive starting point */
        MPI_Recv(&br, Brow*col, MPI_INT, 0, 0, MPI_COMM_WORLD, &s);  /* Recive actual data from B */ 
        MPI_Recv(&A, Arow*Acol, MPI_INT, 0, 0, MPI_COMM_WORLD, &s);  /* Recive Matrix A */ 

       /* Initializing all elements of result matrix to 0 */
       int Cpart[Brow][col];   /* This part used to store each worker process result */
       int i, j, k;
       for(i = 0; i< Brow; i++)
           for(j= 0; j< col; j++)
               Cpart[i][j] = 0;      

     /* Second task: Compute matrix C */
        for (i = 0; i< Arow; i++)
             for(j = 0; j < col; j++ )
                 for (k= 0; k < Brow; k++)
                     Cpart[i][j] = Cpart[i][j] + (A[i][k] * br[k][j]);

     /* Third task: Send result to the master */
        /* 1: tag for messages sent by worker process */
        MPI_Send (&col, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);           /* Send # of colunms */
        MPI_Send(&sp, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);             /* Send starting point */
        MPI_Send(&Cpart, Brow*col, MPI_INT, 0, 1, MPI_COMM_WORLD);   /* Send part of result */
}/* End workers tasks */ 


MPI_Finalize(); 
} /* End of main */ 
 


