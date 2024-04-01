#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "pav_analysis.h"
#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N) {

  Features feat;
  feat.p = compute_power(x, N);
  feat.am = compute_am(x,N);
  feat.zcr = compute_zcr(x,N,16000);
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate, float alfa1, float alfa2) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->alfa1 = alfa1;
  vad_data->alfa2 = alfa2;
  vad_data->last_state = ST_UNDEF;
  vad_data->k0 = 0;
  vad_data->Ninit = 1;
  vad_data->contador = 0;

  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state;
  if(vad_data->state == ST_SILENCE || vad_data->state == ST_VOICE){
    state = vad_data->state;
  } else {
    state = ST_SILENCE;
  }
  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x) {

  /* 
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  switch (vad_data->state) {
  case ST_INIT:
    vad_data->p0 = f.p;   //guardamos la potencia inicial.
    //vad_data->k0=10*log10(vad_data->k0+pow(10,(vad_data->p0/10))/vad_data->Ninit); //lo óptimo es usar una trama: Ninit = 1. Por lo tanto, k0 = p0.
    vad_data->k1 = vad_data->p0 + vad_data->alfa1;
    vad_data->k2 = vad_data->k1 + vad_data->alfa2;
    vad_data->state = ST_SILENCE;
  break;

  case ST_SILENCE:
    if (f.p > vad_data->k1){
      vad_data->state = ST_MVOICE;
      }
    vad_data->last_state = ST_SILENCE;
  break;

  case ST_VOICE:
    if (f.p < vad_data->k1){
      vad_data->state = ST_MSILENCE;
    } 
    vad_data->last_state = ST_VOICE;
  break;

  case ST_MVOICE:
    if(f.p > vad_data->k2){
      vad_data->state = ST_VOICE;
      vad_data->contador = 0;
    } else if(vad_data->contador >= 70){
      vad_data->state = ST_SILENCE;
      vad_data->contador = 0;
    } else{
      vad_data->contador ++;
    }
    vad_data->last_state = ST_MVOICE;
  break;

  case ST_MSILENCE:
    if (f.p > vad_data->k2){
      vad_data->state = ST_VOICE;
      vad_data->contador = 0;
    } else if((f.p < vad_data->k1) && (vad_data->contador >=8)){
      vad_data->state = ST_SILENCE;
      vad_data->contador = 0;
    } else {
      vad_data->contador++;
    }
    vad_data->last_state = ST_MSILENCE;
  break;

  case ST_UNDEF:
  break;
  }

if ((vad_data->state == ST_SILENCE) || (vad_data->state == ST_MVOICE)){
  return ST_SILENCE;
} else if ((vad_data->state == ST_VOICE) || (vad_data->state == ST_MSILENCE)){
  return ST_VOICE;
} else
  return ST_UNDEF;
}

/*if (vad_data->state == ST_SILENCE ||vad_data->state == ST_VOICE){
    return vad_data->state;
  } else if (vad_data->state == ST_INIT){
    return ST_SILENCE;
  } else {
    return ST_UNDEF;
  }*/

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
