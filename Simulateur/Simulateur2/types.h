#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define DEBUG(x) x
//#define DEBUG(x)

#define CACHE_SIZE (1 << 24)
#define BLOCK_SIZE 8

#define PR_RD 'R'
#define PR_WR 'W'
#define BUS_READ 'R'
#define BUS_READX 'X'
#define BUS_READXX '*'  // BusReadX*
#define NONE '-'

// une instruction
typedef struct inst_s inst_t;
struct inst_s{
  char type;  // 'R'=read=PrRd, 'W'=write=PrWr, '-'=pas d'accès mémoire
  int address;  // adresse de l'accès si le type est 'R' ou 'W'
};

// un coeur d'exécution
typedef struct core_s core_t;
struct core_s {
  inst_t inst;  // instruction en cours d'exécution
  int inst_index; // index de l'instruction à exécuter dans la trace
};

// une transaction sur le bus
typedef struct transaction_s transaction_t;
struct transaction_s{
  int block_num;  // numéro de bloc mémoire
  char type;   // type (BUS_READ, BUS_READX, BUS_READXX, NONE)
};

// un bus
typedef struct bus_s bus_t;
struct bus_s{
  int next_served;  // prochain cache à servir (utilisé pour l'arbitrage round robin)
  transaction_t current_transaction;  // transaction en cours
  int served; // cache servi (résultat de l'arbitrage)
};

// une ligne de cache
typedef struct cache_line_s cache_line_t;
struct cache_line_s{
  char state;   // état de la ligne ('M', 'E', 'S' ou 'I')
};

// un cache
typedef struct cache_s cache_t;
struct cache_s{
  cache_line_t * lines; // tableau des lignes (répertoire)
  transaction_t pending_request;  // requête en cours (transaction soumise au bus)
};

// une architecture
typedef struct arch_s arch_t;
struct arch_s {
  int num_cores;  // nombre de coeurs
  core_t * cores; // tableau de coeurs
  cache_t * caches; // tableau de caches - le cache i est associé au coeur i
  bus_t bus;
  int block_size; // taille d'un bloc mémoire
};

// une trace d'exécution : liste des accès mémoire réalisés par chacun des threads
typedef struct trace_s trace_t;
struct trace_s {
  int num_insts;  // nombre d'instructions par thread
  inst_t ** insts;  // tableau d'instructions [numéro_thread][index_instruction]
};

// compteurs pour la génération de stats
typedef struct stats_s stats_t;
struct stats_s{
  int num_cycles;
  int num_cache_accesses;
  int num_BusRead;
  int num_BusReadX;
  int num_BusReadXX;  // BusReadX*
  int num_Flush;
  int num_invalidations; // nombre de blocs invalidés dans un cache pour assurer la cohérence
};

#endif
