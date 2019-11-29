
#include "types.h"

extern void printBusTransaction(FILE *stream, transaction_t *transaction);


// traitement par un cache d'une requête qui lui est faite par son coeur
// la requête peut être une lecture ('R') ou une écriture ('W')
void cacheProcessCoreRequest(arch_t * arch, int core_num, stats_t * stats){
  char type = arch->cores[core_num].inst.type;  // 'R' ou 'W'
  int address = arch->cores[core_num].inst.address; // adresse de la donnée lue ou écrite
  int block_num = address / arch->block_size; // numéro du bloc mémoire contenant la donnée
  char state = arch->caches[core_num].lines[block_num].state; // état du bloc dans le cache
  char req; // requête qui va être envoyée au bus
  stats->num_cache_accesses++;
  switch (state){
    case 'I':
      switch(type){
        case PR_RD: req = BUS_READ;
          break;
        case PR_WR: req = BUS_READX;
          break;
      }
      break;
    case 'E':
      req = NONE; // pas de transaction
      if (type == PR_WR)
        arch->caches[core_num].lines[block_num].state = 'M';
      break;
    case 'S':
      switch(type){
        case PR_RD: req = NONE; // pas de transaction
          break;
        case PR_WR: req = BUS_READXX; // BusReadX*
          break;
      }
      break;
    case 'M':
      req = NONE; // pas de transaction
      break;
  }
  // envoi de la requête au bus
  arch->caches[core_num].pending_request.type = req;
  arch->caches[core_num].pending_request.block_num = block_num;
  if (req == NONE)
    arch->cores[core_num].inst_index++;

  DEBUG(
    fprintf(stderr, "\t[cache %d] @%d is in block %d, state is %c", core_num, address, block_num, state);
    if (req == NONE)
      fprintf(stderr," -> %c (no bus transaction)\n", arch->caches[core_num].lines[block_num].state);
    else{
      fprintf(stderr," -> submitting ");
      printBusTransaction(stderr,&(arch->caches[core_num].pending_request));
      fprintf(stderr,"\n");
    })
}

// cette fonction vérifie si le bloc est présent dans d'autres caches
// cela permet savoir si on doit marquer un bloc nouvellement chargé 'E' ou 'S'
int shared(arch_t * arch, int block_num){
  int n = 0;
  for (int i=0; i < arch->num_cores ; i++)
    if (arch->caches[i].lines[block_num].state == 'S')
      n++;
  return n;
}

// mise à jour de l'état du bloc si la requête d'un cache (BusRd ou BusRdX) a été servie
void cacheEndRequest(arch_t * arch, int core_num, stats_t * stats){
  // est-ce que ma requête a été servie ?
  if (arch->bus.served == core_num){
    int block_num = arch->caches[core_num].pending_request.block_num;
    char type = arch->caches[core_num].pending_request.type;
    switch(type){
      case BUS_READ:
        stats->num_BusRead++;
        if (shared(arch, block_num))
          arch->caches[core_num].lines[block_num].state = 'S';
        else
          arch->caches[core_num].lines[block_num].state = 'E';
        break;
      case BUS_READX:
          arch->caches[core_num].lines[block_num].state = 'M';
          stats->num_BusReadX++;
        break;
      case BUS_READXX:
          arch->caches[core_num].lines[block_num].state = 'M';
          stats->num_BusReadXX++;
        break;
    }
    DEBUG(fprintf(stderr,"[cache %d] request served - state of block %d is now %c\n", core_num, block_num,
      arch->caches[core_num].lines[block_num].state);)
    // la requête est terminée, le coeur peut passer à l'instruction suivante
    arch->caches[core_num].pending_request.type = NONE;
    arch->cores[core_num].inst_index++;
  }
}
