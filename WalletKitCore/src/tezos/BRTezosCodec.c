//
//  File.c
//  
//
//  Created by Amit on 07/06/2020.
//

#include "BRTezosCodec.h"

struct PrimValue{
    char* prim;
    void* args;
    char* annots[];
};
struct BytesValue  {
    uint8_t* bytes;
};

struct StringValue  {
    char* string
};

struct IntValue  {
    char* value;
};
