#include "types.h"

extern void cacheProcessCoreRequest(arch_t * arch, int core_num, stats_t * stats); // cache.c
extern void printBusTransaction(FILE *stream, transaction_t *transaction); // bus.c

void printCoreInst(FILE * stream, inst_t * inst){
  switch(inst->type){
    case PR_RD:
      fprintf(stream, "PrRd(@%d)", inst->address);
      break;
    case PR_WR:
      fprintf(stream, "PrWr(@%d)", inst->address);
      break;
    case NONE:
      fprintf(stream, "-");
      break;
  }
}

void coreProcessInst(arch_t * arch, trace_t * trace, int core_num, stats_t * stats){
  if (arch->cores[core_num].inst_index == trace->num_insts)
    return;
  // s'il n'y a pas de requête cache en cours, traiter une nouvelle instruction
  if (arch->caches[core_num].pending_request.type == '-'){
    // lire une nouvelle instruction
    arch->cores[core_num].inst = trace->insts[core_num][arch->cores[core_num].inst_index];
    DEBUG(
      fprintf(stderr, "[core %d] new instruction (index %d) = ", core_num,
        arch->cores[core_num].inst_index);
      printCoreInst(stderr, &(arch->cores[core_num].inst));
      fprintf(stderr,"\n");
    )
    // si l'instruction est un PrRd ou un PrWr, accéder au cache
    if ((arch->cores[core_num].inst.type == PR_RD)
      || (arch->cores[core_num].inst.type == PR_WR))
        cacheProcessCoreRequest(arch, core_num, stats);  // cache.c
    else
      // sinon, passer à l'instruction suivante
      arch->cores[core_num].inst_index++;
  }
  // sinon (requête cache en cours) attendre
  else{
      DEBUG(
        fprintf(stderr, "[core %d] bus request still pending: ", core_num);
        printBusTransaction(stderr, &(arch->caches[core_num].pending_request));
        fprintf(stderr, "\n"); )
        ;
  }
}
