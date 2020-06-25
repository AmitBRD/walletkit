//
//  File.c
//  
//
//  Created by Amit on 07/06/2020.
//
#include "BRTezosCodec.h"
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include "support/BRArray.h"



#ifdef __cplusplus
extern "C" {
#endif

//typedef struct {
//    char* prim;
//    void* args;
//    char* annots[];
//} PrimValue;
//typedef struct  {
//    uint8_t* bytes;
//} BytesValue;
//
//typedef struct  {
//    char* string;
//} StringValue;
//
//typedef struct  {
//    char* value;
//} IntValue;
//
//typedef struct{
//    
//} Zarith;
//
//typedef struct {
//  void ** array;
//  size_t used;
//  size_t size;
//} Operations;
//
//
//typedef struct{
//    char * branch;
//    void * contents[];
//} Operation;
//
//typedef struct  {
//    
//}TezosTransaction;
//
//typedef struct {
//    
//    
//}TezosDelegate;
//
//typedef struct {
//    
//}TezosReveal;

    void
    swapBytesIfLittleEndian (uint8_t *target, uint8_t *source, size_t count) {
        assert (target != source);  // common overlap case, but wholely insufficient.
        for (size_t i = 0; i < count; i++) {
    #if BYTE_ORDER == LITTLE_ENDIAN
            target[i] = source[count - 1 - i];
    #else
            target[i] = source[i];
    #endif
        }
    }
    
    void
    swapBytesIfBigEndian (uint8_t *target, uint8_t *source, size_t count) {
        assert (target != source);  // common overlap case, but wholely insufficient.
        for (size_t i = 0; i < count; i++) {
    #if BYTE_ORDER != LITTLE_ENDIAN
            target[i] = source[count - 1 - i];
    #else
            target[i] = source[i];
    #endif
        }
    }
    
    size_t
      findNonZeroIndex (uint8_t *bytes, size_t bytesCount) {
          for (size_t i = 0; i < bytesCount; i++)
              if (bytes[i] != 0) return i;
          return bytesCount;
      }
    
    void
    convertToBigEndianAndNormalize (uint8_t *target, uint8_t *source, size_t length,
                                    size_t *targetIndex, size_t *targetCount) {
        
        swapBytesIfBigEndian(target, source, length);
        
        *targetIndex = findNonZeroIndex(target, length);
        *targetCount = length - *targetIndex;
        
        if (0 == *targetCount) {
            *targetCount = 1;
            *targetIndex = 0;
        }
    }
    
    /**
     * Fill `targetCount` bytes into `target` using the big-endian formatted `bytesCount` at `bytes`.
     */
    void
    convertFromBigEndian (uint8_t *target, size_t targetCount, uint8_t *bytes, size_t bytesCount) {
        // Bytes represents a number in big-endian              : 04 00
        // Fill out the number with prefix zeros                : 00 00 00 00 00 00 04 00
        // Copy the bytes into target, swap if little endian    : 00 04 00 00 00 00 00 00
        uint8_t value[targetCount];
        memset (value, 0, targetCount);
        memcpy (&value[targetCount - bytesCount], bytes, bytesCount);

        swapBytesIfLittleEndian(target, value, targetCount);
    }

    unsigned char* hexstr_to_char(const char* hexstr)
       {
           size_t len = strlen(hexstr);
           assert(len % 2 != 0);
               return NULL;
           size_t final_len = len / 2;
           unsigned char* chrs = (unsigned char*)malloc((final_len+1) * sizeof(*chrs));
           for (size_t i=0, j=0; j<final_len; i+=2, j++)
               chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i+1] % 32 + 9) % 25;
           chrs[final_len] = '\0';
           return chrs;
       }
       
       char* barray2hexstr (const unsigned char* data, size_t datalen) {
         size_t final_len = datalen * 2;
         char* chrs = (unsigned char *) malloc((final_len + 1) * sizeof(*chrs));
         unsigned int j = 0;
         for(j = 0; j<datalen; j++) {
           chrs[2*j] = (data[j]>>4)+48;
           chrs[2*j+1] = (data[j]&15)+48;
           if (chrs[2*j]>57) chrs[2*j]+=7;
           if (chrs[2*j+1]>57) chrs[2*j+1]+=7;
         }
         chrs[2*j]='\0';
         return chrs;
       }



//export const pad = (num: number, paddingLen: number = 8) => {
//  return num.toString(16).padStart(paddingLen, '0');
//};
uint8_t* padLeft(BRTezosData data, size_t targetSize){
    if(data ==NULL )return NULL;
    size_t paddingLen = targetSize - data->bytesCount;
    uint8_t * buffer = calloc((paddingLen + data->bytesCount),sizeof(uint8_t));
    memcpy(buffer + paddingLen, data->bytes,data->bytesCount*sizeof(uint8_t));
    for(int i=0; i < paddingLen+data->bytesCount; i++){
                   printf("%02x",buffer[i]);
    }
    return buffer;
}
    
    uint8_t* _padLeft(uint8_t * data, size_t dataLength, size_t targetSize){
        size_t paddingLen = targetSize - dataLength;
        uint8_t * buffer = calloc(targetSize, sizeof(uint8_t));
        memcpy(buffer + paddingLen, data,dataLength*sizeof(uint8_t));
        for(int i=0; i < paddingLen+dataLength; i++){
                       printf("%02x",buffer[i]);
        }
        return buffer;
    }
     
    
    

//    BRTezosData bufferConcat(size_t numBuffers, ...){
//        va_list ap;
//        va_start(ap, numBuffers); //Requires the last fixed parameter (to get the address)
//        BRTezosData target = malloc(sizeof(BRTezosData))
//        for(int j=0; j<numBuffers; j++)
//               va_arg(ap,  BRTezosData*); //Requires the type to cast to. Increments ap to the next argument.
//        va_end(ap);
//        return target;
//
//    }
//
//    BRTezosData concat(BRTezosData * a, BRTezosData *b ){
//        va_list ap;
//        va_start(ap, numBuffers); //Requires the last fixed parameter (to get the address)
//        for(int j=0; j<numBuffers; j++)
//               va_arg(ap,  BRTezosData); //Requires the type to cast to. Increments ap to the next argument.
//        va_end(ap);
//
//    }
    

    
//export const bytesEncoder: Encoder<BytesValue> = value => {
//  if (!/^([A-Fa-f0-9]{2})*$/.test(value.bytes)) {
//    throw new Error(`Invalid hex string: ${value.bytes}`);
//  }
//
//  const len = value.bytes.length / 2;
//  return `0a${pad(len)}${value.bytes}`;
//};
    
    /*michelson encoders below, we dont need this if we are not supporting smart contract execution atm*/
    
   static void
    encodeLengthIntoBytes (uint64_t length, uint8_t baseline,
                           uint8_t *bytes9, uint8_t *bytes9Count) {
        size_t lengthSize = sizeof (uint64_t);

               //uint8_t bytes [lengthSize]; // big_endian representation of the bytes in 'length'
               size_t bytesIndex;          // Index of the first non-zero byte
               size_t bytesCount;          // The number of bytes to encode (beyond index)

               convertToBigEndianAndNormalize (bytes9, (uint8_t *) &length, lengthSize, &bytesIndex, &bytesCount);

               // The encoding - a header byte with the bytesCount and then the big_endian bytes themselves.
               uint8_t encoding [1 + bytesCount];
               encoding[0] = baseline +  + bytesCount;
               memcpy (&encoding[1], &bytes9[bytesIndex], bytesCount);

               // Copy back to bytes
               memcpy (bytes9, encoding, 1 + bytesCount);
               *bytes9Count = 1 + bytesCount;
    }

    
    #define BYTE_PREFIX ((const uint8_t) { 0x0a})
    uint8_t * encodeBytes(uint8_t * bytes, uint64_t bytesLen){
        //BRCryptoCoder coder= cryptoCoderCreate(CRYPTO_CODER_HEX);
        //cryptoCoderEncodeLength(coder,)
        uint8_t target[1+4+bytesLen];
        
        uint8_t prefix[1] = {0x0a};
//        uint8_t * target;
//        array_new(target, 1+4+bytesLen);
//        array_insert(target, 0, BYTE_PREFIX);
        //_padLeft(bytesLen,, <#size_t targetSize#>)
        memcpy(&target[0], prefix,1);//copy byte prefix
        convertFromBigEndian(&target[1], sizeof(uint64_t),( uint8_t *) &bytesLen, 4);
        //memcpy(&target[1],( uint8_t *) &bytesLen, 4);
        memcpy(&target[5],bytes, bytesLen);
        
        for(int i=0; i< 5+bytesLen; i++){
            printf("%02x",target[i]);
        }
        return barray2hexstr(&target[0], (size_t)(1+4+bytesLen));
        //return strdup(target);
    }
    
    

//export const stringEncoder: Encoder<StringValue> = value => {
//  const str = Buffer.from(value.string, 'utf8').toString('hex');
//  const hexLength = str.length / 2;
//  return `01${pad(hexLength)}${str}`;
//};
    
    uint8_t * encodeString(char * value, uint64_t strLength){
        uint8_t target[1+4+strLength];
        uint8_t prefix[1] = {0x01};
        memcpy(&target[0], prefix,1);//copy byte prefix
        convertFromBigEndian(&target[1], sizeof(uint64_t),( uint8_t *) &strLength, 4);
        memcpy(&target[5], value, strLength);
       return barray2hexstr(&target[0], (size_t)(1+4+strLength));
        
    }

    
    

//export const intEncoder: Encoder<IntValue> = ({ int }) => {
//  const num = new BigNumber(int, 10);
//  const positiveMark = num.toString(2)[0] === '-' ? '1' : '0';
//  const binary = num.toString(2).replace('-', '');
//
//  const pad =
//    binary.length <= 6
//      ? 6
//      : (binary.length - 6) % 7
//        ? binary.length + 7 - ((binary.length - 6) % 7)
//        : binary.length;
//
//  const splitted = binary.padStart(pad, '0').match(/\d{6,7}/g);
//
//  const reversed = splitted!.reverse();
//
//  reversed[0] = positiveMark + reversed[0];
//
//  const numHex = reversed.map((x: string, i: number) =>
//    // Add one to the last chunk
//    parseInt((i === reversed.length - 1 ? '0' : '1') + x, 2)
//      .toString(16)
//      .padStart(2, '0')
//  );
//
//  return `00${numHex.join('')}`;
//};


//export const primEncoder: Encoder<PrimValue> = value => {
//  const hasAnnot = +Array.isArray(value.annots);
//  const argsCount = Array.isArray(value.args) ? value.args.length : 0;
//
//  // Specify the number of args max is 3 without annotation
//  const preamble = pad(Math.min(2 * argsCount + hasAnnot + 0x03, 9), 2);
//
//  const op = opMappingReverse[value.prim];
//
//  let encodedArgs = (value.args || []).map(arg => valueEncoder(arg)).join('');
//  const encodedAnnots = Array.isArray(value.annots) ? encodeAnnots(value.annots) : '';
//
//  if (value.prim === 'LAMBDA' && argsCount) {
//    encodedArgs = pad(encodedArgs.length / 2) + encodedArgs + pad(0);
//  }
//
//  return `${preamble}${op}${encodedArgs}${encodedAnnots}`;
//};

//export const encodeAnnots: Encoder<string[]> = (value: string[]) => {
//  const mergedAnnot = value
//    .map(x => {
//      return Buffer.from(x, 'utf8').toString('hex');
//    })
//    .join('20');
//
//  const len = mergedAnnot.length / 2;
//  return `${pad(len)}${mergedAnnot}`;
//};


//export const valueEncoder: Encoder<MichelsonValue> = (value: MichelsonValue) => {
//  if (Array.isArray(value)) {
//    const encoded = value.map(x => valueEncoder(x)).join('');
//    const len = encoded.length / 2;
//    return `02${pad(len)}${encoded}`;
//  } else if (isPrim(value)) {
//    return primEncoder(value);
//  } else if (isBytes(value)) {
//    return bytesEncoder(value);
//  } else if (isString(value)) {
//    return stringEncoder(value);
//  } else if (isInt(value)) {
//    return intEncoder(value);
//  }
//
//  throw new Error('Unexpected value');
//};

   
    
     void
    convertToLittleEndianAndNormalize (uint8_t *target, uint8_t *source, size_t length,
                                    size_t *targetIndex, size_t *targetCount) {
        
        
        swapBytesIfLittleEndian(target, source, length);
        *targetIndex = findNonZeroIndex(target, length);
        *targetCount = length - *targetIndex;
        
        if (0 == *targetCount) {
            *targetCount = 1;
            *targetIndex = 0;
        }
    }
    
    void tempZEncoder(uint8_t * bytes, size_t bytesLen){
            
            uint8_t v0= bytes[bytesLen-1] & 0x7F;
            if(bytesLen>1){
                v0+=128;
            }
            printf("\r\ntempZEncoder bytes :\r\n");
            printf("val0: %02x",v0 );
            uint8_t overflowBits = 1;
            uint8_t overflow = (bytes[bytesLen-1] & 0x80) >> (8-overflowBits);
            for(int i=1; i<=bytesLen-1; i++){
                uint8_t readBits = 7-i%7;
                uint8_t val = ((bytes[bytesLen-1-i] & (0xff >> (8-readBits))) << overflowBits) | overflow;
                printf("val0: %02x, %d,%d\r\n",val, overflowBits,readBits);
                overflowBits = (8-readBits)%7;
                overflow = (bytes[bytesLen-1-i] & 0x80) >> (8-overflowBits);
                //TODO: val + 128 (0x1000000 set to 1 if there is more bytes in the number)
            }
        printf("\r\n");
        overflowBits =0;
        for(int i=0; i<25; i++){
            uint8_t readBits = 7-i%7;
            if(overflowBits>0){
                printf("bytes[%d] read [%d] & bytes[%d] read [%d]\r\n",i,readBits,i-1,overflowBits);
            }else{
                printf("bytes[%d] read [%d]\r\n",i,readBits);
            }
            
            overflowBits = (8-readBits)%7;
        }
        
        //TODO add final overflow bit;
        
    //        while (true) {
    //          // eslint-disable-line
    //          if (nn.lt(128)) {
    //            if (nn.lt(16)) fn.push('0');
    //            fn.push(nn.toString(16));
    //            break;
    //          } else {
    //            let b = nn.mod(128);
    //            nn = nn.minus(b);
    //            nn = nn.dividedBy(128);
    //            b = b.plus(128);
    //            fn.push(b.toString(16));
    //          }
    //        }
        }
    uint8_t * zarithEnoder(uint8_t * value, size_t count){
      
      uint8_t zarith[count];
      printf("\r\n zarith 7bit little endian encoding");
      uint8_t shifted = 0x00;
      uint8_t overflow = 0x01 & value[0] >> 7;
      for(int i=7; i<count*7; i+=7){
          
          uint8_t index = (i /8);
          uint8_t bits = i%8;
          0xff << bits;
          0xff >> (7-bits) %7;
          printf("\r\index:%d read:%d index%d read:%d", index, bits, index+1, (7-bits) %7);
          printf(" i:%d value:%02x",(i/7)-1, value[(i/7)-1]);
          
      }
      return NULL;
    }

    
    /*
     http://tezos.gitlab.io/api/p2p.html?highlight=zarith
    A variable length sequence of bytes, encoding a Zarith number. Each byte has a running unary size bit: the most significant bit of each byte tells is this is the last byte in the sequence (0) or if there is more to read (1). Size bits ignored, data is then the binary representation of the absolute value of the number in little endian order.*/
    void
    encodeNumber ( uint8_t *source, size_t sourceCount) {
        // Encode a number by converting the number to a big_endian representation and then simply
        // encoding those bytes.
        printf("bytes sent in :\r\n");
        for(int i=0; i < sourceCount; i++){
                              printf("%02x",source[i]);
               }
        uint8_t bytes [sourceCount]; // big_endian representation of the bytes in 'source'
        size_t bytesIndex = findNonZeroIndex(source, sourceCount);
        size_t bytesCount = sourceCount - bytesIndex;           // The number of bytes to encode
        
        //convert the bytes to litle endian format
        for (size_t i = 0; i < sourceCount; i++) {
            bytes[i] = source[sourceCount - 1 - i];
        }

        printf("litle endian bytes :\r\n");
        for(int i=0; i < bytesCount; i++){
                       printf("%02x",bytes[i]);
        }
        
        
        
        //uint8_t test[2] = {0x03,0xe8};//e807
        uint8_t test[2] = {0x27,0x13}; //934e
        tempZEncoder(&test[0], 2);
        //return zarithEnoder(&bytes[0], bytesCount);
        return NULL;
    }
    
    
    
    
    
    
 void
hexEncodeTezos (char *target, size_t targetLen, const uint8_t *source, size_t sourceLen) {
    //assert (targetLen == 2 * sourceLen  + 1);
    
//    for (int i = 0; i < sourceLen; i++) {
//        target[2*i + 0] = encodeChar (source[i] >> 4);
//        target[2*i + 1] = encodeChar (source[i]);
//    }
    target[2*sourceLen] = '\0';
    
}

 size_t
hexEncodeLengthTezos(size_t byteArrayLen) {
    return 2 * byteArrayLen + 1;
}

 char *
hexEncodeCreateTezos (size_t *targetLen, const uint8_t *source, size_t sourceLen) {
    size_t length = hexEncodeLength(sourceLen);
    if (NULL != targetLen) *targetLen = length;
    char *target = malloc (length);
    hexEncode(target, length, source, sourceLen);
    return target;
}

 int
hexEncodeValidateTezos (const char *number) {
    // Number contains only hex digits, has an even number and has at least two.
    if (NULL == number || '\0' == *number || 0 != strlen(number) % 2) return 0;

    while (*number)
        if (!isxdigit (*number++)) return 0;
    return 1;
}
    
    
    
    
    
    
    
    //Dumping Ground
    uint8_t * bufferConcatList(uint8_t* buffers[], size_t buffersLength[] ){
        size_t targetLength = 0;
        size_t buffersSize = sizeof buffersLength /sizeof * buffersLength;
        for(size_t i=0; i< buffersSize; i++){
            targetLength+= buffersLength[i];
        }
        uint8_t * target = calloc(targetLength, sizeof(uint8_t));
        for(size_t i=0; i < buffersSize; i++){
            memcpy(target+buffersLength[i], buffers[i], sizeof(uint8_t)*buffersLength[i]);
        }
        return target;
    }

    uint8_t* _bufferConcat(uint8_t * a, size_t aLength, uint8_t * b, size_t bLength){
        uint8_t* target = malloc((aLength+bLength)*sizeof(uint8_t));
        memcpy(target, a, aLength);
        memcpy(target+aLength, b, bLength);
        return target;
    }
    
  

    
    
    
    
    //We only need to support
//    export const ActivationSchema = {
//      pkh: 'tz1',
//      secret: 'secret',
//    };
//
//    export const RevealSchema = {
//      source: 'pkh',
//      fee: 'zarith',
//      counter: 'zarith',
//      gas_limit: 'zarith',
//      storage_limit: 'zarith',
//      public_key: 'public_key',
//    };
//
//    export const DelegationSchema = {
//      source: 'pkh',
//      fee: 'zarith',
//      counter: 'zarith',
//      gas_limit: 'zarith',
//      storage_limit: 'zarith',
//      delegate: 'delegate',
//    };
//
//    export const TransactionSchema = {
//      source: 'pkh',
//      fee: 'zarith',
//      counter: 'zarith',
//      gas_limit: 'zarith',
//      storage_limit: 'zarith',
//      amount: 'zarith',
//      destination: 'address',
//      parameters: 'parameters',
//    };
//    export const SeedNonceRevelationSchema = {
//      level: 'int32',
//      nonce: 'raw',
//    };
#ifdef __cplusplus
}
#endif
