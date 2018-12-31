#include <stdlib.h>
#include <stdio.h>
#define NUMSTATE 2
#define NUMPARTS 8
#define NUMOBS 32 

float cdf[NUMPARTS];
/*aMod[ x_t-1*NUM + x_t ] = p(x_t | x_t-1)
  activation state change model 
  probability of moving to x_t if we were just at x_t-1
  aMod = [ 0->0, 0->1, 1->0, 1->1 ]
  Currently only works for 1-coord state 
*/ 
float aMod[NUMSTATE * NUMSTATE] = {4.0/5.0, 1.0/5.0, 1.0/3.0, 2.0/3.0};

/*oMod[ x_t*NUM + y_t ] = p(y_t | x_t)
  probability of observing y_t if the actual state is x_t
*/
float oMod[NUMSTATE * NUMSTATE] = {9.5/10.0, 0.5/10.0, 1.0/5.0, 4.0/5.0};

typedef size_t state;
/*Actual observations made of the environment*/
state obs[NUMOBS] = {0,1,0,0,0,1,0,0,1,0,1,0,0,1,1,0,0,1};

typedef struct {
  float c[NUMSTATE];
} distribution;


typedef struct {
  
  state st;
  float wt;

} particle;

particle particles[2][NUMPARTS];


float uniformRand(){
  return rand()/RAND_MAX;
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
   
}

size_t evalute(particle p){

   
   
}


int main(int argc, char *argv[]){

  /*Initialization of particles*/
  for(int i = 0; i < NUMPARTS; i++){

    particles[0][i].st = 0;
    particles[0][i].wt = 1.0;

    particles[1][i].st = 0;
    particles[1][i].wt = 1.0;

  }  

  size_t cur = 0, prev = 1;
  for(int i = 0; i < NUMOBS; i++){
 
    state s = obs[i]; 
    
    cur = (cur == 0) ? 1 : 0;
    prev = !cur;

    float wSum = 0.0;
    /*Update all particles*/
    for(int p = 0; p < NUMPARTS; p++){   

      /*Draw particles according to importance*/  
      size_t pIdx = resample();

      /*Update particle using the activation model aMod*/ 
      particles[cur][i] = propagate( particles[prev][pIdx] );
 
      float eval = evaluate(particles[cur][i]); 
      wSum += eval; 
      particles[cur][i].wt = eval;

    }
    
    if( wSum == 0.0 ){
      /*reinitialize all particles*/ 
    }

    cdf[0] = 0.0;
    for(int c = 0; c < NUMPARTS; c++){

      particles[cur][c].wt /= wSum;
      cdf[c+1] = cdf[c] + particles[cur][c].wt;

    }
    
  }
  
  return 0;
}

