#include "bank.h"
#include "memory.h"

#define ALIGN(VALUE, BLOCK_SIZE)  ((VALUE) + ((BLOCK_SIZE) - 1)) & ~((BLOCK_SIZE) - 1)

struct __Bank_t
{
  u16     size;
  u16     count;
  void  **free;
  void  **used;
  void   *data[];
};

static inline u32 getTrailingZeroCount(u32 _value)
{
  u32 u32Result;

  /* Uses intrinsic */
  u32Result = (u32) __builtin_ctz(_value);

  /* Done! */
  return u32Result;
}

void BANK_clear(Bank *_bank)
{
  memset(_bank->data, 0, _bank->count * _bank->size);
  for(u16 i = 1, offset = _bank->size / sizeof(void *), count = (_bank->count - 1) * offset;
    i < count;
    i += offset)
  {
    *(_bank->data + i) = _bank->data + i + offset + 1;
  }
  _bank->free = _bank->data + 2;
  _bank->used = NULL;
}

Bank *BANK_create(u16 _count, u16 _size)
{
  Bank *result;

  _size = ALIGN(_size + 2 * sizeof(void *), sizeof(void *));
  result = (Bank *)malloc(sizeof(Bank) + _count * _size);
  if(result != NULL)
  {
    result->size = _size;
    result->count = _count;
    BANK_clear(result);
  }

  return result;
}

void BANK_delete(Bank *_bank)
{
  free(_bank);
}

void *BANK_allocate(Bank *_bank)
{
  void **result;

  result = _bank->free;

  if(result)
  {
    _bank->free = (void **)*(result - 1);
    if(_bank->used)
    {
      *(_bank->used - 2) = (void *)result;
    }
    *(result - 1) = (void *)_bank->used;
    _bank->used = result;
  }

  return (void *)result;
}

void BANK_free(Bank *_bank, void *_cell)
{
  void **current = (void **)_cell;
  void **next = (void **)*(current - 1);
  if(_bank->used == current)
  {
    _bank->used = next;
  }
  else
  {
    void **previous = (void **)*(current - 2);
    *(previous - 1) = (void *)next;
    if(next)
    {
      *(next - 2) = (void *)previous;
    }
  }
  *(current - 1) = (void *)_bank->free;
  _bank->free = current;
}

void *BANK_getFirst(const Bank *_bank)
{
  return (void *)_bank->used;
}

void *BANK_getNext(const Bank *_bank, const void *_cell)
{
  return *((void **)_cell - 1);
}
