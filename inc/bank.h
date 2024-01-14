#ifndef _BANK_H_
#define _BANK_H_

#include "types.h"

typedef struct __Bank_t Bank;

Bank *         BANK_create(u16 _count, u16 _size);

void           BANK_delete(Bank *_bank);

void *         BANK_allocate(Bank *_bank);

void           BANK_free(Bank *_bank, void *_cell);

void           BANK_clear(Bank *_bank);

void *         BANK_getFirst(const Bank *_bank);

void *         BANK_getNext(const Bank *_bank, const void *_cell);

#endif
