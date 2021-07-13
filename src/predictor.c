//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Shangqing Gu, Junxian Qu";
const char *studentID   = ", A59005904";
const char *email       = ", j1qu@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

uint32_t BHR_gshare;
uint8_t* BHT_gshare;

// Tournament
uint32_t *PHT;
uint8_t *BHT_local;
uint8_t *BHT_global;
uint8_t *Predict;
uint32_t BHR_tournament;

// Perceptron (Custom)
int** F;
int* history;
float sum;
uint32_t BHR_custom;
//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  switch (bpType)
  {
    case STATIC:
      break;
    case GSHARE:
      init_GSHARE();
      break;
    case TOURNAMENT:
      init_TOURNAMENT();
      break;
    case CUSTOM:
      init_CUSTOM();
      break;
    default:
      break;
  }
}


int
power_base_2(int exp)
{
  int result = 1;
  for (int i = 0; i < exp; i++) {
    result *= 2;
  }
  return result;
}

uint32_t
lower_n_bits(uint32_t num, int n)
{
  return (num << (32 - n)) >> (32 - n);
}

void
init_GSHARE()
{
  int BHT_size = power_base_2(ghistoryBits);
  BHT_gshare = malloc(BHT_size * sizeof(uint8_t));
  for (int i = 0; i < BHT_size; i++) {
    BHT_gshare[i] = WN;
  }
}


void
init_TOURNAMENT()
{
  int BHT_size = power_base_2(lhistoryBits);
  BHT_local = malloc(BHT_size * sizeof(uint8_t));
  for (int i = 0; i < BHT_size; i++) {
    BHT_local[i] = WN;
  }

  int PHT_size = power_base_2(pcIndexBits);
  PHT = malloc(PHT_size * sizeof(uint8_t));
  for (int i = 0; i < PHT_size; i++) {
    PHT[i] = WN;
  }

  int BHT_global_size = power_base_2(ghistoryBits);
  BHT_global = malloc(BHT_global_size * sizeof(uint8_t));
  for (int i = 0; i < BHT_global_size; i++) {
    BHT_local[i] = WN;
  }

  int Predict_size = power_base_2(ghistoryBits);
  Predict = malloc(Predict_size * sizeof(uint8_t));
  for (int i = 0; i < Predict_size; i++) {
    Predict[i] = WN;
  }
}


void
init_CUSTOM()
{
  //TODO
}


// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType)
  {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return predict_GSHARE(pc);
    case TOURNAMENT:
      return predict_TOURNAMENT(pc);
    case CUSTOM:
      return predict_CUSTOM(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}


uint8_t
predict_GSHARE(uint32_t pc)
{
  uint32_t index = lower_n_bits(pc, ghistoryBits) ^ lower_n_bits(BHR, ghistoryBits);
  if (BHT_gshare[index] == WT || BHT_gshare[index] == ST) {
    return TAKEN;
  }
  return NOTTAKEN;
}



uint8_t
predict_TOURNAMENT_BHR()
{
  uint32_t index_T = lower_n_bits(BHR_tournament, ghistoryBits);
  if (BHT_global[index_T] == WT || BHT_global[index_T] == ST) {
    return TAKEN;
  }
  return NOTTAKEN;
}

uint8_t
predict_TOURNAMENT_PC(uint32_t pc)
{
  uint32_t index_PC = lower_n_bits(pc, pcIndexBits);
  uint32_t index_BHT = PHT[index_PC];
  if (BHT_local[index_BHT] == WT || BHT_local[index_BHT] == ST) {
    return TAKEN;
  }
  return NOTTAKEN;
}

uint8_t
predict_TOURNAMENT(uint32_t pc)
{
  uint32_t index = lower_n_bits(BHR_tournament, ghistoryBits);
  if (Predict[index] == WN || Predict[index] == WT) {
    return predict_TOURNAMENT_BHR();
  }
  return predict_TOURNAMENT_PC(pc);
}


uint8_t
predict_CUSTOM(uint32_t pc)
{
  //TODO
  return NOTTAKEN;
}


// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  switch (bpType)
  {
    case STATIC:
      break;
    case GSHARE:
      train_GSHARE(pc, outcome);
      break;
    case TOURNAMENT:
      train_TOURNAMENT(pc, outcome);
      break;
    case CUSTOM:
      train_CUSTOM(pc, outcome);
      break;
    default:
      break;
  }
}


void
train_GSHARE(uint32_t pc, uint8_t outcome)
{
  uint32_t index = lower_n_bits(pc, ghistoryBits) ^ lower_n_bits(BHR, ghistoryBits);
  if (outcome == TAKEN && BHT_gshare[index] < 3) {
    BHT_gshare[index]++;
  } else if (outcome == NOTTAKEN && BHT_gshare[index] > 0) {
    BHT_gshare[index]--;
  }
}

void
train_TOURNAMENT(uint32_t pc, uint8_t outcome)
{
  uint8_t PHT_size = lower_n_bits(pc, pcIndexBits) ;
  PHT[PHT_size] = outcome;
  
  if(BHT_global[BHR_tournament] < 3 && outcome == TAKEN)
  {
    BHT_global[BHR_tournament]++;
  }
  if(BHT_global[BHR_tournament] > 0 && outcome != TAKEN)
  {
    BHT_global[BHR_tournament]--;
  }
  uint32_t temp = lower_n_bits(BHR_tournament*2, ghistoryBits) ;
  BHR_tournament = temp | outcome;
}

void
train_CUSTOM(uint32_t pc, uint8_t outcome)
{
  //TODO
}