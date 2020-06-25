#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "tezos/BRTezos.h"
#include "tezos/BRTezosCodec.h"
#include "support/BRInt.h"

#pragma clang diagnostic pop
#pragma GCC diagnostic pop

extern void
runTezosTest (void /* ... */) {
    //createAccount();
    
    
    BRTezosData data = malloc(sizeof(BRTezosData));
    uint8_t number[2] = {0x0f,0xfa};
    data->bytes = &number[0];
    data->bytesCount = 2;
    data->bytes= padLeft(data, 4);
    //0000 0ffa
    
    free(data->bytes);
    
    
    uint8_t random[12] = {0x0f, 0xde , 0xad,0x0f, 0xde , 0xad,0x0f, 0xde , 0xad,0x0f, 0xde , 0xad};
    uint8_t * value = encodeBytes(&random[0], (uint64_t)12);
    
    free(value);
    char * string = "hello";
    value = encodeString(string, 5);
    
    free(value);
    
    encodeNumber(uint256("0000000000376bb71314321c45de3015fe958543afcbada242a3b1b072498e38").u8,32);
    
    
    
}
