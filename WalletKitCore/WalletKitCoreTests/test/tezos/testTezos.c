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

void printBuffer(struct Data buffer){
    for(int i=0; i < buffer.length; i++){
        printf("%02x", buffer.buffer[i]);
    }
}
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
    
    struct Data zarithData = encodeNumber(uint256("00000000000000000000000000000000000000000000000000000000000003e8"));
          printf("\r\n Zarith expected:e807 actual:");
          printBuffer(zarithData);
          free(zarithData.buffer);
    zarithData = encodeNumber(uint256("0000000000000000000000000000000000000000000000000000000000002713"));
             printf("\r\n Zarith expected:934e actual:");
             printBuffer(zarithData);
             free(zarithData.buffer);
    zarithData = encodeNumber(uint256("000000000000000000000000000000000000000000000000001fffffffffffff"));
       printf("\r\n Zarith expected:ffffffffffffff0f actual:");
       printBuffer(zarithData);
       free(zarithData.buffer);
    zarithData = encodeNumber(uint256("0000000000000000000000000000000000000000000000000020000000000000"));
       printf("\r\n Zarith expected:8080808080808010 actual:");
       printBuffer(zarithData);
       free(zarithData.buffer);
    zarithData = encodeNumber(uint256("0000000000000000000000000000000000000000000000000020000000000001"));
       printf("\r\n Zarith expected:8180808080808010 actual:");
       printBuffer(zarithData);
       free(zarithData.buffer);
    zarithData = encodeNumber(uint256("0000000000000000000000000000000000000000000000000020000000000002"));
        printf("\r\n Zarith expected:8280808080808010 actual:");
        printBuffer(zarithData);
        free(zarithData.buffer);

//    struct Data encoded = encodePkh("tz1SeV3tueHQMTfquZSU7y98otvQTw6GDKaY");
//    printBuffer(encoded);
//    free(encoded.buffer);
    
    struct Data encoded = encodePkh("tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn");
    printf("\r\n PKH expected:0035e993d8c7aaa42b5e3ccd86a33390ececc73abd actual:");
    printBuffer(encoded);
    free(encoded.buffer);
    
    encoded = encodeAddress("tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn");
    printf("\r\n Address expected:000035e993d8c7aaa42b5e3ccd86a33390ececc73abd actual:");
    printBuffer(encoded);
    free(encoded.buffer);
    
    encoded = encodeBranch("BLzyjjHKEKMULtvkpSHxuZxx6ei6fpntH2BTkYZiLgs8zLVstvX");
    printf("\r\n Branch expected:a99b946c97ada0f42c1bdeae0383db7893351232a832d00d0cd716eb6f66e561 actual:");
       printBuffer(encoded);
       free(encoded.buffer);
    
    
    uint32_t v = -123654;
    char * encoded2 = encodeInt32(&v);
    free(encoded2);
}
