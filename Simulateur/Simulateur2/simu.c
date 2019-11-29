#include "types.h"

extern int readTrace(char * trace_filename, trace_t * trace); // init.c
extern void initArch(int num_cores, arch_t *arch, int block_size);    // init.c
extern void initStats(stats_t * stats); // init.c
extern void coreProcessInst(arch_t * arch, trace_t * trace, int core_num, stats_t * stats); // core.c
extern void cacheSnoopBusRequests(arch_t * arch, int core_num, stats_t * stats);  // snoop.c
extern void busArbitrateCacheRequests(arch_t * arch); // bus.c
extern void cacheEndRequest(arch_t * arch, int num_core, stats_t * stats); // cache.c

// simulation d'un cycle "logique"
void simulateCycle(arch_t * arch, trace_t * trace, stats_t * stats){
  DEBUG(fprintf(stderr,"========== Cycle %d ==========\n", stats->num_cycles);)
  // chaque coeur traite une instruction, accède éventuellement à son cache
  // le cache envoie éventuellement une requête au bus
  for (int i=0; i < arch->num_cores; i++)
    coreProcessInst(arch, trace, i, stats);    // core.c
  // le bus arbitre parmi toutes les requêtes qui lui sont soumises
  busArbitrateCacheRequests(arch);    // bus.c
  // s'il y a une transaction sur le bus
  //  les caches espionnent le bus et maintiennent la cohérence des données
  if (arch->bus.served != -1)
    for (int i=0; i < arch->num_cores; i++)
      cacheSnoopBusRequests(arch, i, stats);  // snoop.c
  // le cache dont la requête a été servi met son répertoire à jour
  if (arch->bus.served != -1)
    cacheEndRequest(arch,arch->bus.served, stats);  // cache.c
  // fin de la transaction sur le bus
  arch->bus.current_transaction.type = '-';
}

// fin de la simulation ?
char checkEnd(arch_t * arch, trace_t * trace){
  char done = 1;
  for (int i=0; i<arch->num_cores; i++){
    // tant qu'un des threads n'a pas exécuté toutes ses instructions,
    //   il faut continuer
    if (arch->cores[i].inst_index < trace->num_insts)
      done = 0;
  }
  return done;
}

// affichage des stats
void dumpStats(stats_t *stats, arch_t * arch, trace_t * trace){
  printf("\n====== Résultats ======\n");
  printf("nombre de coeurs : %d\n", arch->num_cores);
  printf("nombre d'instructions par thread : %d\n", trace->num_insts);
  printf("taille d'un bloc mémoire : %d octets\n\n", arch->block_size);
  printf("nombre de cycles logiques : %d\n", stats->num_cycles);
  printf("nombre d'accès au cache : %d\n", stats->num_cache_accesses);
  printf("nombre de BusRead : %d\n", stats->num_BusRead);
  printf("nombre de BusReadX : %d\n", stats->num_BusReadX);
  printf("nombre de BusReadX* : %d\n", stats->num_BusReadXX);
  printf("nombre de Flush : %d\n", stats->num_Flush);
  printf("nombre d'invalidations : %d\n", stats->num_invalidations);
  printf("\n");
}

int main(int argc, char *argv[]){
  // lecture du fichier trace
  if (argc<2){
    printf("Usage: ./simu <trace file name> [<block_size>]\n");
    exit(1);
  }
  char * trace_filename = argv[1];
  trace_t trace;
  int num_cores = readTrace(trace_filename, &trace);  // init.c

  // génération de l'architecture
  arch_t arch;
  int block_size = BLOCK_SIZE;
  if (argc == 3)
    block_size = atoi(argv[2]);
  initArch(num_cores, &arch, block_size);  // init.c

  // initialisation des compteurs de stats
  stats_t stats;
  initStats(&stats);  // init.c

  // tant que la simulation n'est pas terminée, simuler un cycle logique
  char done = 0;
  while (!done){
    simulateCycle(&arch, &trace, &stats);
    done = checkEnd(&arch, &trace);
    if (!done)
      stats.num_cycles++;
  }
  dumpStats(&stats, &arch, &trace);
  return 0;
}
