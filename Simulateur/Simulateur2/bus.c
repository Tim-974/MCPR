#include "types.h"

void printBusTransaction(FILE *stream, transaction_t *transaction){
    switch(transaction->type){
      case BUS_READ:
        fprintf(stream, "BusRead(block %d)", transaction->block_num);
        break;
      case BUS_READX:
        fprintf(stream, "BusReadX(block %d)", transaction->block_num);
        break;
      case BUS_READXX:
        fprintf(stream, "BusReadX*(block %d)", transaction->block_num);
        break;
      case NONE:
        fprintf(stream, "-");
        break;
    }
}

void busArbitrateCacheRequests(arch_t * arch){
  int count = 0;
  int served = -1;
  // scan cache requests
  while (count < arch->num_cores){
    if (arch->caches[arch->bus.next_served].pending_request.type != NONE){
        served = arch->bus.next_served;
        break;
      }
    else{
      arch->bus.next_served++;
      if (arch->bus.next_served == arch->num_cores)
        arch->bus.next_served = 0;
      count++;
    }
  }
  arch->bus.served = served;
  if (served != -1) { // a request has been found
    arch->bus.current_transaction.type = arch->caches[arch->bus.next_served].pending_request.type;
    arch->bus.current_transaction.block_num = arch->caches[arch->bus.next_served].pending_request.block_num;
    arch->bus.next_served++;
    if (arch->bus.next_served == arch->num_cores)
      arch->bus.next_served = 0;
    DEBUG(
      fprintf(stderr, "[bus] selected transaction: ");
      printBusTransaction(stderr, &(arch->bus.current_transaction));
      fprintf(stderr, " from cache %d\n", served);
    )
  }
  else
    DEBUG(fprintf(stderr, "[bus] no transaction\n"));
}
