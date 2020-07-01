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

uint8_t* padLeft(BRTezosData data, size_t targetSize);
uint8_t * encodeBytes(uint8_t * bytes, uint64_t bytesLen);
uint8_t * encodeString(char * value, uint64_t strLength);
struct Data zarithEncoder(uint8_t * bytes, size_t bytesLen);
struct Data encodeNumber ( UInt256 number);
struct Data encodePkh(char * pkh, size_t length );
uint8_t* encodeInt32(uint32_t u32);
struct Data encodeAddress(char * pk, size_t length);
#ifdef __cplusplus
}
#endif

#endif /* BRTezosCodec_h */
