#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define NUMSTATE 2
#define NUMPARTS 8
#define NUMOBS 16 

float cdf[NUMPARTS];
/*
 * cdf = [p1|   p2   |                           p3                        |  p4 | ... ]
 * the "width" of a particles section in the array is its likelihood of being multinomially sampled
 */

/*
  activation state change model 
  probability of moving to x_t if we were just at x_t-1
  aMod = [ 0:stay, 0:switch, 1:stay, 1:switch]
  Currently only works for 1-coord state 
*/ 
float aMod[NUMSTATE * NUMSTATE] = {4.0/5.0, 1.0/5.0, 4.0/5.0, 1.0/5.0};

/*oMod[ x_t*NUM + y_t ] = p(y_t | x_t)
  probability of observing y_t if the actual state is x_t
*/
float oMod[NUMSTATE * NUMSTATE] = {9.5/10.0, 0.5/10.0, 1.0/5.0, 4.0/5.0};

typedef size_t state;
/*Actual observations made of the environment*/
//state obs[NUMOBS] = {0,1,1,1,0,0,1,1,1,0,0,0,1,1,0,0};
state obs[NUMOBS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

typedef struct {
  float c[NUMSTATE];
} distribution;


typedef struct {
  
  state st;
  float wt;

} particle;

particle particles[2][NUMPARTS];

void printCDF(){
  printf("[");
  for(int i = 0; i < NUMPARTS-1; i++){
    printf("%f,",cdf[i]);
  }
  printf("%f]\n",cdf[NUMPARTS-1]);
}

float uniformRand(){
  return (float)rand()/(float)RAND_MAX;
}

void initialize(){

  float wt = 1.0 / (float)NUMPARTS;
  /*Initialization of particles*/
  for(int i = 0; i < NUMPARTS; i++){

    particles[0][i].st = (uniformRand() > 0.5) ? 1 : 0;
    particles[0][i].wt = wt;

    particles[1][i].st = (uniformRand() > 0.5) ? 1 : 0;
    particles[1][i].wt = wt;
   
  }  

}

size_t resample(){

 float r = uniformRand(); 
 for (int i = 0; i < NUMPARTS; i++){
   if(cdf[i] > r){
     return i; 
   }
 }
 return NUMPARTS-1;

}

size_t propagate(particle p){
 
  float r = uniformRand();
  /*Use aMod to figure out what the next particle value should be (0 or 1) and
 * return it.*/
  state curState = p.st;
 
  if( r < aMod[ curState*NUMSTATE ] ){

    /*stay in the same state*/
    return curState;

  }else{

    /*switch states*/
    return !curState;

  }
   
}

float evaluate(particle p, state obs){

  state curState = p.st;
  float prob = oMod[obs*NUMSTATE + curState];
  //printf("The likelihood of %u means %u was %f\n",obs,curState,prob);
  return prob;
   
}


int main(int argc, char *argv[]){

  srand(time(NULL));
  printf("==Particle Filter Demo\n");
  initialize(); 
  printf("Cur particles initially: ");
  for(int i = 0; i < NUMPARTS; i++){
    printf("%u ",particles[0][i].st);
  }
  printf("\n");

  printf("Prev particles initially: ");
  for(int i = 0; i < NUMPARTS; i++){
    printf("%u ",particles[1][i].st);
  }
  printf("\n");

  size_t cur = 0, prev = 1;
  for(int i = 0; i < NUMOBS; i++){
 
    state s = obs[i]; 
    printf("Observation: %u\n",s);
    
    cur = (cur == 0) ? 1 : 0;
    prev = !cur;

    float wSum = 0.0;
    /*Update all particles*/
    for(int p = 0; p < NUMPARTS; p++){   

      /*Draw particles according to importance*/  
      size_t pIdx = resample();
      //printf("Drew particle: %u -> (%u,%f)\n",pIdx,particles[cur][pIdx].st,particles[cur][pIdx].wt);

      /*Update particle using the activation model aMod*/ 
      particles[cur][p].st = propagate( particles[prev][pIdx] );
      //printf("Sampled p%u with state: %u\n",pIdx,particles[cur][p].st);
 
      float eval = evaluate(particles[cur][p],s); 
      //printf("New p%u weight: %f\n",p,eval);
      wSum += eval; 
      particles[cur][p].wt = eval;

    }

    if( wSum == 0.0 ){
      initialize();
    }
   
    size_t num0 = 0;
    float tSum = 0.0;
    /*Normalize particle weights before updating the CDF*/
    for(int p = 0; p < NUMPARTS; p++){
      particles[cur][p].wt /= wSum;
      tSum += particles[cur][p].wt;
      if( particles[cur][p].st == 0 ){ num0++; }
    }

    tSum = 0.0;
    for(int p = 0; p < NUMPARTS; p++){

      cdf[p] = particles[cur][p].wt + tSum;
      tSum += particles[cur][p].wt;

    }
    //printCDF();
    if( num0 >= NUMPARTS/2 ){
     printf("estimate == 0\n");
    } else{
     printf("estimate == 1\n");
    }
    
    
  }
  
  return 0;
}

