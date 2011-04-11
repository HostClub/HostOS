//Bitset implementation

#define OFFSET (a%(8*4))
#define INDEX (a/(8*4))


//set a bit in the *ptr
static void set_bit(uint32_t flag, uint32_t *ptr){
  uint32_t b = flag/0x1000;
  uint32_t i = INDEX(b);
  uint32_t o = OFFSET(b);
  ptr[i]    |= (0x1 << o);

}

//clear a bit in the *ptr
static void clear_bit(uint32_t flag, uint32_t *ptr){
  uint32_t b = flag/0x1000;
  uint32_t i = INDEX(b);
  uint32_t o = OFFSET(b);
  ptr[i]    &= ~(0x1 << o);
}

//check if bit is set
static uint32_t test_bit(uint32_t flag, uint32_t *ptr){
  uint32_t b = flag/0x1000;
  uint32_t i = INDEX(b);
  uint32_t o = OFFSET(b);
  return (ptr[i] & (0x1 << off));
}


