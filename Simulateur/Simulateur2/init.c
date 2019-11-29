#include "types.h"

void initArch(int num_cores, arch_t *arch, int block_size){
  // création des coeurs
  arch->num_cores = num_cores;
  arch->cores = (core_t *) malloc(num_cores * sizeof(core_t));
  assert(arch->cores);
  for (int i=0 ; i<num_cores; i++)
    arch->cores[i].inst_index = 0;
  // création des caches
  arch->caches = (cache_t *) malloc(num_cores * sizeof(cache_t));
  assert(arch->caches);
  arch->block_size = block_size;
  for (int i=0 ; i<num_cores; i++){
    int num_lines = CACHE_SIZE/arch->block_size;
    arch->caches[i].lines = (cache_line_t *) malloc( num_lines * sizeof(cache_line_t));
    assert(arch->caches[i].lines);
    for (int j=0; j<num_lines; j++)
      arch->caches[i].lines[j].state = 'I';
    arch->caches[i].pending_request.type = NONE;
  }
  // création du bus
  arch->bus.next_served = 0;
  arch->bus.served = -1;
}

// lecture du fichier trace
int readTrace(char * trace_filename, trace_t * trace){
  int num_cores;
  FILE* f = fopen(trace_filename, "r");
  if (f == NULL){
    printf("File %s not found\n", trace_filename);
    exit(1);
  }
  fscanf(f, "%d", &num_cores);
  fscanf(f, "%d\n", &(trace->num_insts));
  trace->insts = (inst_t **) malloc(num_cores * sizeof(inst_t *));
  assert(trace->insts != NULL);
  for (int i=0; i<num_cores; i++){
    trace->insts[i] = (inst_t *) malloc(trace->num_insts * sizeof(inst_t));
    assert(trace->insts[i] != NULL);
    for (int j=0; j<trace->num_insts ; j++)
      fscanf(f, "%c %d\n", &(trace->insts[i][j].type), &(trace->insts[i][j].address));
  }
  fclose(f);
  return num_cores;
}

// initialisation des compteurs
void initStats(stats_t * stats){
  stats->num_cycles = 0;
  stats->num_cache_accesses = 0;
  stats->num_BusRead = 0;
  stats->num_BusReadX = 0;
  stats->num_BusReadXX = 0;
  stats->num_Flush = 0;
  stats->num_invalidations = 0;
}
