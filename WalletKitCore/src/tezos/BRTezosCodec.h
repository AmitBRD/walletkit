//
//  BRTezosCodec.h
//  
//
//  Created by Amit on 07/06/2020.
//

#ifndef BRTezosCodec_h
#define BRTezosCodec_h

#include <stdio.h>
#include "support/BRInt.h"

#ifdef __cplusplus
extern "C" {
#endif
//typedef struct PrimValue *_prim;
//typedef struct  BytesValue * _bytes;
//typedef struct  StringValue *_string;
//typedef struct  IntValue *_int;

typedef struct BRTezosDataRecord *BRTezosData;

struct BRTezosDataRecord{
    size_t bytesCount;
    uint8_t * bytes;
    char * hexData;
} ;

struct Data{
    uint8_t * buffer;
    size_t length;
};


enum operation {
    reveal = 0x6b,
    delegation = 0x6e,
    transaction = 0x6c,
    seed_nonce_revalation= 0x01
};

struct Operation{
    enum operation op;
    
};

uint8_t* padLeft(BRTezosData data, size_t targetSize);
uint8_t * encodeBytes(uint8_t * bytes, uint64_t bytesLen);
uint8_t * encodeString(char * value, uint64_t strLength);
struct Data zarithEncoder(uint8_t * bytes, size_t bytesLen);
struct Data encodeNumber ( UInt256 number);
struct Data encodePkh(char * pkh);
uint8_t* encodeInt32(uint32_t u32);
struct Data encodeAddress(char * pk);
struct Data encodeBranch(char * branch);
struct Data encodeDelegate(char * pkh,int delegate);
#ifdef __cplusplus
}
#endif

#endif /* BRTezosCodec_h */
