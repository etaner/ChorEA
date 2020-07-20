#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define PC 1.0
#define PM 0.1
#define POPSIZE 10
#define MAXGNR 500

#define LINES 12
#define COLUMNS 10
#define CHRMLEN LINES*COLUMNS
#define MAXDEPO 4

#define DANCERS 7 /* must be 1 for solo, 3 for duo, 7 for trio, etc */
#define LOCATION 8
#define DIRECTION 8
#define REPETITION 3

/* POSECOUNT and POSES together correspond to PHRASE in non-50LOOKS version */
#define POSECOUNT 5  /* must be between 2 and 5. 1 is not allowed */
#define POSES 50   /* no of poses in 50 LOOKS */  

/* returns 1 with probability p and 0 with probability (1-p)*/
#define flip(p) ((((float) random() / RAND_MAX) <= p) ? 1 : 0)

/* Chromosome ---- Dncr Loc Dir Rep PCnt Poses */

typedef struct {
		int chromosome[CHRMLEN];
        int fitness;
} individual_t;

individual_t pop[POPSIZE+1];
individual_t depo[MAXDEPO];
int valid_depo[MAXDEPO];

float pc = PC;
int chrmlen = CHRMLEN;
float pm = PM;
int popsize = POPSIZE;
int gnrcnt = MAXGNR; 

int lines = LINES;
int columns = COLUMNS;

int p1, p2, c, deleted, selected, save, save_index;

void display_population(void);
void display_three(int i1, int i2, int i3);
void display_two(int i1,int i2);
void display_saved(void);
void display_final_selection(int selected);


void initialize_population(void)
{ 
    int i, j, k, t;
    int pcnt;

	for (i=0; i<popsize; i++) 
	{
		for (j=0; j<lines; j++) 
		{   
			pop[i].chromosome[j*columns+0] = 1 + rand()%DANCERS;
			pop[i].chromosome[j*columns+1] = 1 + rand()%LOCATION;
			pop[i].chromosome[j*columns+2] = 1 + rand()%DIRECTION;	
			pop[i].chromosome[j*columns+3] = 1 + rand()%REPETITION;	
			pop[i].chromosome[j*columns+4] = 2 + rand()%(POSECOUNT-1);	
            pcnt = pop[i].chromosome[j*columns+4];
            for (k=0; k<pcnt; k++)
            {
    			pop[i].chromosome[j*columns+5+k] = 1 + rand()%POSES;
                t = pop[i].chromosome[j*columns+5+k];
                if ((t==5)||(t==9)||(t==10)||(t==16)||(t==24)||(t==25))	
                    pop[i].chromosome[j*columns+5+k]++;
            }            
            for (k=pcnt; k<POSECOUNT; k++)
    			pop[i].chromosome[j*columns+5+k] = -1;	

		}	
        pop[i].fitness = 0;
	}
}


int select_parent(void)
{
    int i;

    i = rand()%POPSIZE;
    return(i);
}


int crossover(int i1, int i2) //uniform crossover for 50 Looks
{   
    int i, j;

     for (i=0; i< LINES; i++)
        {
		    for (j=0; j< 4; j++)
            {
                if (flip(0.5))
                    pop[POPSIZE].chromosome[i*COLUMNS+j] = pop[i1].chromosome[i*COLUMNS+j];
                else
                    pop[POPSIZE].chromosome[i*COLUMNS+j] = pop[i2].chromosome[i*COLUMNS+j];                   
            }   
            if (flip(0.5))   // for the Pose Count and Poses together
            {
                for (j=4; j<COLUMNS; j++)
                    pop[POPSIZE].chromosome[i*COLUMNS+j] = pop[i1].chromosome[i*COLUMNS+j];
            }
            else
            {
               for (j=4; j<COLUMNS; j++)
                   pop[POPSIZE].chromosome[i*COLUMNS+j] = pop[i2].chromosome[i*COLUMNS+j];
            }
        }
     pop[POPSIZE].fitness=0;
     return(POPSIZE);  //child always at last element of pop
}


int mutation (int m)
{ 
    int i, k, t;
    int pcnt;

    for (i=0; i<chrmlen; i++)
    {
        if (flip(pm))
        {
            if (i%COLUMNS == 0) //dancers
                pop[m].chromosome[i] = 1 + rand()%DANCERS;

            if (i%COLUMNS == 1) //location
                pop[m].chromosome[i] = 1 + rand()%LOCATION;

            if (i%COLUMNS == 2) //direction
                pop[m].chromosome[i] = 1 + rand()%DIRECTION;

            if (i%COLUMNS == 3) //repetition
                pop[m].chromosome[i] = 1 + rand()%REPETITION;	           

            if (i%COLUMNS == 4) //pose count
            {
                pop[m].chromosome[i] = 2 + rand()%(POSECOUNT-1);
                pcnt = pop[m].chromosome[i];
                for (k=0; k<pcnt; k++)
                {    			    
                    pop[m].chromosome[i+1+k] = 1 + rand()%POSES;
                    t = pop[m].chromosome[i+1+k];
                    if ((t==5)||(t==9)||(t==10)||(t==16)||(t==24)||(t==25))	
                        pop[m].chromosome[i+1+k]++;	
      	        }
                for (k=pcnt; k<POSECOUNT; k++)
    			    pop[m].chromosome[i+1+k] = -1;	
                i=i+POSES;
            }
        }
    }
    return (m);
}


int main(void)
{

	srand(time(NULL));
    printf("\n\n");
    int i,j;

   	initialize_population();

    save_index=0;	
    for (i=0; i<MAXDEPO; i++)
        valid_depo[i]=0; 	

   i=0;
    while (i<MAXGNR)
    {   
        j=0;
        do{
    		p1 = select_parent();
	    	p2 = select_parent();
            j++;
        } while ((j<10)&&(p1==p2));
        if (p1==p2)
            p2=(p1+1) % popsize;
		c = crossover(p1,p2);
		c= mutation(c);
        display_three(p1,p2,c);
        do {
            printf("  Which one will be DELETED? (0/1/2/3) ... ");
            scanf("%d", &deleted);
        } while (deleted!=0 && deleted!=1 && deleted!=2 && deleted!=3);
 
        if (deleted == 0)
        {
          printf("  Do you want to SAVE a choreography? (0/1) ... ");
          scanf("%d", &save);
          if (save)
          {
             do {
                printf("  Which one will be SAVED? (0/1/2/3) ...   ");
                scanf("%d", &save);
             } while (save!=0 && save!=1 && save!=2 && save!=3);
             if (save!=0)
             {
                if (save==1)
                {
                    for (j=0; j<chrmlen; j++)
                        depo[save_index].chromosome[j] = pop[p1].chromosome[j];
                    depo[save_index].fitness = pop[p1].fitness;
                    valid_depo[save_index]=1; 	
                    save_index = (save_index+1) % MAXDEPO;
                }
                else if (save==2)
                {
                    for (j=0; j<chrmlen; j++)
                        depo[save_index].chromosome[j] = pop[p2].chromosome[j];
                    depo[save_index].fitness = pop[p2].fitness;
                    valid_depo[save_index]=1; 	
                    save_index = (save_index+1) % MAXDEPO;
                }
                else if (save==3)
                {
                   for (j=0; j<chrmlen; j++)
                        depo[save_index].chromosome[j] = pop[POPSIZE].chromosome[j];
                    depo[save_index].fitness = pop[POPSIZE].fitness;
                    valid_depo[save_index]=1; 	
                    save_index = (save_index+1) % MAXDEPO;
                }
             }
          }    
          break;            
        }

        if (deleted !=3)  //if deleted is child do nothing
        {
            if (deleted ==1)
               deleted = p1;
            else if (deleted ==2)
               deleted = p2;
            for (j=0; j<chrmlen; j++)
                pop[deleted].chromosome[j] = pop[c].chromosome[j];
            pop[deleted].fitness = pop[c].fitness;
        }
        pop[p1].fitness ++; //increase fitness
        pop[p2].fitness ++; //increase fitness

        printf("  Do you want to SAVE a choreography? (0/1) ... ");
        scanf("%d", &save);

        if (save)
        {
            display_two(p1,p2);
            do {
                 printf("  Which one will be SAVED? (0/1/2) ... ");
                 scanf("%d", &save);
            } while (save!=0 && save!=1 && save!=2);
            if (save!=0)
            {
                if (save==1)
                {        
                    for (j=0; j<chrmlen; j++)
                        depo[save_index].chromosome[j] = pop[p1].chromosome[j];
                    depo[save_index].fitness = pop[p1].fitness;
                    valid_depo[save_index]=1; 	
                    save_index = (save_index+1) % MAXDEPO;
                }
                else
                {
                    for (j=0; j<chrmlen; j++)
                        depo[save_index].chromosome[j] = pop[p2].chromosome[j];
                    depo[save_index].fitness = pop[p2].fitness;
                    valid_depo[save_index]=1; 	
                    save_index = (save_index+1) % MAXDEPO;
                }
            }
        }    
        i++;
	}	   
    display_saved();
    printf("\n");
    printf("SELECT a Choreography ... ");
    scanf("%d", &selected);

    if ((selected<1) && (selected>3))
    {
        printf("\n\n ERROR: Incorrect input. Program exiting...      \n");
        exit(1);
    } 
    display_final_selection(selected);
	return 0;
}	


/* Disply functions */

void display_population(void)
{
    int i, j;

	for (i=0; i<popsize; i++)
	{	
		printf("\n");
		for (j=0; j< CHRMLEN; j++)
		{
		  if ((j % COLUMNS)==0)  	
            printf("\n");
		  printf("%2d  ", pop[i].chromosome[j]);
        }	
	}
}

void display_three(int i1,int i2,int i3)
{
    int i, j;
        printf("\e[1;1H\e[2J"); //clear screen
		printf("\n\n");
        printf("Choreography 1\t\t\t\tChoreography 2\t\t\t\tChoreography 3\n");

      for (i=0; i< LINES; i++)
        {
            printf("\n");
		    for (j=0; j< COLUMNS; j++)
                    printf("%2d ", pop[i1].chromosome[i*COLUMNS+j]);

    		printf("\t\t");
		    for (j=0; j< COLUMNS; j++)
                printf("%2d ", pop[i2].chromosome[i*COLUMNS+j]);

    		printf("\t\t");
		    for (j=0; j< COLUMNS; j++)
                printf("%2d ", pop[i3].chromosome[i*COLUMNS+j]);
        }
		printf("\n\n");
}

void display_two(int i1,int i2)
{
    int i, j;
        printf("\e[1;1H\e[2J"); //clear screen
		printf("\n\n");
 
        printf("Choreography 1\t\t\tChoreography 2\n");

      for (i=0; i< LINES; i++)
        {
            printf("\n");
		    for (j=0; j< COLUMNS; j++)
                    printf("%2d ", pop[i1].chromosome[i*COLUMNS+j]);

    		printf("\t");
		    for (j=0; j< COLUMNS; j++)
                printf("%2d ", pop[i2].chromosome[i*COLUMNS+j]);
        }
		printf("\n\n");
}

void display_saved(void)
{
    int i, j;
        printf("\e[1;1H\e[2J"); //clear screen
		printf("\n");

    if ((valid_depo[0]!=0)||(valid_depo[1]!=0)||(valid_depo[2]!=0)||(valid_depo[3]!=0)) {
    	printf("\n");
        printf("SAVED Choreographies\n");
      for (i=0; i< LINES; i++)
        {
            printf("\n");
            if (valid_depo[0]!=0) {
		    for (j=0; j< COLUMNS; j++)
                    printf("%2d ", depo[0].chromosome[i*COLUMNS+j]);
          }

            if (valid_depo[1]!=0) {
    		printf("\t");
		    for (j=0; j< COLUMNS; j++)
                    printf("%2d ", depo[1].chromosome[i*COLUMNS+j]);
            }

            if (valid_depo[2]!=0) {
	    	printf("\t");
		    for (j=0; j< COLUMNS; j++)
                printf("%2d ", depo[2].chromosome[i*COLUMNS+j]);
            }

            if (valid_depo[3]!=0) {
	    	printf("\t");
		    for (j=0; j< COLUMNS; j++)
                printf("%2d ", depo[3].chromosome[i*COLUMNS+j]);
            }

        }
		printf("\n");
    }
}

void display_final_selection(int selected)
{
    int i, j;

    if (valid_depo[selected-1]!=0) {
       printf("\e[1;1H\e[2J"); //clear screen
       printf("\n\n");
       printf("ChorEA ...... SELECTED CHOREOGRAPY    \n");
       printf("\n(Dancers, Location, Direction, Repetition, PoseCount, Poses ..... \n");
      for (i=0; i< LINES; i++)
        {
            printf("\n\n\t");
		    for (j=0; j< COLUMNS; j++)
            {
                if (depo[selected-1].chromosome[i*COLUMNS+j] != -1)       
                     printf("%2d   ", depo[selected-1].chromosome[i*COLUMNS+j]);
                else
                     printf("--   ");
            }       
        }
    }
    else
    {
        printf("\n\n ERROR: Incorrect input. Program exiting...     \n");
        exit(1);
    } 
    printf("\n\n");
}

