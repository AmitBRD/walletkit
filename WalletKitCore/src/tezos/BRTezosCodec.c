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
#include "support/BRBase58.h"
#include "blake2b/blake2b.h"



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
    struct Data  uint8tdup(uint8_t * buffer, size_t length){
        /**TODO:
         return a structure encapsulating buffer and length so we may concat all the DataStructs
         
         DataStruct {
            uint8_t * buffer;
            size_t length;
         }
        **/
        struct Data data;
        data.buffer = malloc(length * sizeof(uint8_t));
        memcpy(data.buffer, buffer, length);
        data.length = length;
        return data;
        //return barray2hexstr(buffer,length);
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
    char * print_byte_as_bits(char val) {
       for (int i = 7; 0 <= i; i--) {
        printf("%c", (val & (1 << i)) ? '1' : '0');
      }
      
    }
    
    struct Data zarithEncoder(uint8_t * bytes, size_t bytesLen){
        uint8_t target[(((bytesLen*8) / 7)) ];
        uint8_t overflowBits= 0;
        uint8_t overflow = 0x0;
        uint8_t readBits = 0x0;
        uint8_t readMask =0x0;
        uint8_t overflowMask = 0x0;
        uint8_t val = 0x0;
        for(int i=0; i<bytesLen; i++){
            readBits = 7-i%7;
            readMask = 0xff >> (8-readBits);
            overflowMask = ~readMask;
            val =(bytes[bytesLen-1-i] & readMask) << overflowBits ;
            val =  val | overflow ;
            val +=128;
//            printf("\r\n overflow mask:");
//            print_byte_as_bits(overflowMask);
//            printf("\r\n read mask:");
//            print_byte_as_bits(readMask);
            
           
            overflowBits = (8-readBits)%7;
            overflow =((bytes[bytesLen-1-i] & overflowMask)) >> (8-overflowBits);
            target[i]=val;
             //printf("val: %02x", val);
        }
//            printf("\r\nLAST EXPRESSION\r\n");
//            print_byte_as_bits(overflowBits);
//            printf("\r\n final bytes");
//            print_byte_as_bits(bytes[0]);
//            readBits = 7-(bytesLen)%7;
//            readMask = 0xff >> (8- readBits);
//            printf("\r\n read mask bits:%d", readBits);
//            print_byte_as_bits(readMask);
        if((bytes[0] & overflowMask) > 0x0){
            readBits = 7-bytesLen%7;
            overflowBits = (8-readBits)%7;
            val =(bytes[0] & overflowMask) >> overflowBits ;
            target[sizeof(target)-1]=val;
            //printf("overflow: %02x", val);
        }else{
            val-= 128;
            target[sizeof(target)-1]=val;
            //printf("last byte replaced: %02x", val);
        }
        return uint8tdup(&target[0], sizeof(target));
    }
        
        


    
    /*
     http://tezos.gitlab.io/api/p2p.html?highlight=zarith
    A variable length sequence of bytes, encoding a Zarith number. Each byte has a running unary size bit: the most significant bit of each byte tells is this is the last byte in the sequence (0) or if there is more to read (1). Size bits ignored, data is then the binary representation of the absolute value of the number in little endian order.*/
    struct Data
    encodeNumber ( UInt256 number) {
        size_t zeroIndex = findNonZeroIndex(number.u8,32);
        return zarithEncoder(&number.u8[zeroIndex], 32-zeroIndex);
        
    }
    
    
    //  taquito-utils.js constants:
    //        const prefixMap = {
    //          [prefix.tz1.toString()]: '0000',
    //          [prefix.tz2.toString()]: '0001',
    //          [prefix.tz3.toString()]: '0002',
    //        };
    //        [Prefix.TZ1]: new Uint8Array([6, 161, 159]),
    //         [Prefix.TZ2]: new Uint8Array([6, 161, 161]),
    //         [Prefix.TZ3]: new Uint8Array([6, 161, 164]),
    //
    //        [Prefix.EDSK]: new Uint8Array([43, 246, 78, 7]),
    //        [Prefix.EDSK2]: new Uint8Array([13, 15, 58, 7]),
    //        [Prefix.SPSK]: new Uint8Array([17, 162, 224, 201]),
    //        [Prefix.P2SK]: new Uint8Array([16, 81, 238, 189]),
    //
    //        [Prefix.EDPK]: new Uint8Array([13, 15, 37, 217]),
    //        [Prefix.SPPK]: new Uint8Array([3, 254, 226, 86]),
    //        [Prefix.P2PK]: new Uint8Array([3, 178, 139, 127]),

    
   
    struct Data encodePkh(char * pkh )
    {
        uint8_t prefix;
        if(memcmp(pkh, "tz1", 3)==0){
            prefix = 0x00;
           //TZ1 support only
        }else if(memcmp(pkh, 'tz2', 3)==0){
            prefix = 0x01;
        }else if(memcmp(pkh, 'tz3', 3)==0){
            prefix = 0x02;
        }else{
            return;
        }
        //decode the pkh and remove the checksum
        size_t len = BRBase58CheckDecode(NULL, 0, pkh);
        uint8_t decoded[len];
        BRBase58CheckDecode(&decoded[0], len, pkh);

//        printf("\r\n decoded pkh:");
//        for(int i=0; i < len;i++){
//            printf("%02x", decoded[i]);
//        }
        //printf("\r\n expected: 06a19f4cdee21a9180f80956ab8d27fb6abdbd8993405226694591");
        uint8_t target[len-2];//remove the prefix
        memcpy(&target[0], &prefix,1);//add in the prefix
        memcpy(&target[1], &decoded[3], len-3);//add in the decoded public key removing the prefix and checksum
//        printf("\r\n decoded target:");
//        for(int i=0; i < len-2;i++){
//            printf("%02x", target[i]);
//        }
//        printf("\r\n expected: 004cdee21a9180f80956ab8d27fb6abdbd8993405226694591");
        //TODO: add unit test from taquito
        return uint8tdup(&target[0], sizeof(target));

        
    }
    
//    [Prefix.EDPK]: new Uint8Array([13, 15, 37, 217]),
//    [Prefix.SPPK]: new Uint8Array([3, 254, 226, 86]),
//    [Prefix.P2PK]: new Uint8Array([3, 178, 139, 127]),
    static uint8_t PREFIX_EDPK[] = {13,15,37, 217};
    static uint8_t PREFIX_SPPK[] = {3, 254, 226, 86};
    static uint8_t PREFIX_P2PK[] = {3, 178, 139, 127};
    
    struct Data encodePublicKey(char * pk){
        uint8_t prefix;
        if(memcmp(PREFIX_EDPK, pk, 4)==0){
                   prefix = 0x00;
                  //TZ1 support only
        }else if(memcmp(pk, PREFIX_SPPK, 3)==0){
                   prefix = 0x01;
        }else if(memcmp(pk, PREFIX_P2PK, 3)==0){
                   prefix = 0x02;
        }else{
                   return;
        }
        
        size_t len = BRBase58Decode(NULL, 0, pk);
        uint8_t decoded[len];
        BRBase58Decode(&decoded[0], len, pk);

//        printf("\r\n decoded pkh:");
//        for(int i=0; i < len;i++){
//            printf("%02x", decoded[i]);
//        }
        uint8_t target[len-3];//remove the prefix
        memcpy(&target[0], &prefix,1);//add in the prefix
        memcpy(&target[1], &decoded[4], len-4);//add in the decoded public key removing the prefix
//        printf("\r\n decoded public key:");
//        for(int i=0; i < len-3;i++){
//            printf("%02x", target[i]);
//        }
        //printf("\r\n expected: ");
        //TODO: add unit test from taquito
        return uint8tdup(target, len-3);
    }
    

   
    struct Data encodeParams(){
        //TODO: we do not support michelson contract execution
        uint8_t buffer[1]={0x00};
        return uint8tdup(&buffer[0],1);
    }
    
    //    export const addressEncoder = (val: string): string => {
    //      const pubkeyPrefix = val.substr(0, 3);
    //      switch (pubkeyPrefix) {
    //        case Prefix.TZ1:
    //        case Prefix.TZ2:
    //        case Prefix.TZ3:
    //          return '00' + pkhEncoder(val);
    //        case Prefix.KT1:
    //          return '01' + prefixEncoder(Prefix.KT1)(val) + '00';
    //        default:
    //          throw new Error('Invalid address');
    //      }
    //    };
    
    struct Data encodeAddress(char * address){
        uint8_t prefix;
        if(memcmp(address, "tz1", 3)==0 || memcmp(address, "tz2", 3)==0 || memcmp(address, "tz3", 3)==0){
            prefix = 0x00;
        }else{
            //TODO: we do not support KT1
            return;
        }
        
        struct Data encoded = encodePkh(address);
       
        uint8_t target[encoded.length + 1];
        memcpy(&target[0], &prefix, 1);
        memcpy(&target[1], encoded.buffer, encoded.length);
        free(encoded.buffer);
        
        return uint8tdup(&target[0], sizeof(target));
    }
    

    
//    export const int32Encoder = (val: number | string): string => {
//      const num = parseInt(String(val), 10);
//      const byte = [];
//      for (let i = 0; i < 4; i++) {
//        const shiftBy = (4 - (i + 1)) * 8;
//        byte.push((num & (0xff << shiftBy)) >> shiftBy);
//      }
//      return Buffer.from(byte).toString('hex');
//    };
    
    uint8_t * encodeInt32(uint32_t u32){
        uint8_t target[4];
        target[3] = (uint8_t)u32;
        target[2] = (uint8_t)(u32>>=8);
        target[1] = (uint8_t)(u32>>=8);
        target[0] = (uint8_t)(u32>>=8);
        return strdup(target);
    }
    
    uint8_t encodeBool(int v){
        if(v>0){ return 0xff;}else{return 0x00;}
    }
    
    struct Data encodeDelegate(char * pkh,int delegate){
        uint8_t bool =  encodeBool(delegate);
        size_t len = 1;
        if(delegate>0){
            struct Data encodedPkh = encodePkh(pkh);
             len += encodedPkh.length;
            uint8_t buffer[len];
            memcpy(&buffer[1], encodedPkh.buffer, encodedPkh.length);
            free(encodedPkh.buffer);
            memcpy(&buffer[0], &bool, 1);
            return uint8tdup(buffer, len);
        }else{
            uint8_t buffer[1]={bool};
            return uint8tdup(buffer, len);
        }
            
    }
    
    struct Data concatAndFreeDataBuffers(struct Data * parameters, size_t params){
        size_t totalLength = 0;
        for(int i=0; i < params; i++){
            totalLength+=parameters[i].length;
        }
        uint8_t buffer[totalLength];
        totalLength = 0;
        for(int i=0; i < params; i++){
            memcpy(&buffer[totalLength], parameters[i].buffer, parameters[i].length);
            free(parameters[i].buffer);
            totalLength+=parameters[i].length;
        }
        return  uint8tdup(buffer, totalLength);
        
    }
    
    
    struct Data encodeTransaction(char * source, UInt256 fee, UInt256 counter, UInt256 gasLimit, UInt256 storageLimit, UInt256 amount, char * destination){
        size_t params = 8;
        struct Data target[params];
        target[0]=encodePkh(source);
        target[1]=encodeNumber(fee);
        target[2]=encodeNumber(counter);
        target[3]=encodeNumber(gasLimit);
        target[4]=encodeNumber(storageLimit);
        target[5]=encodeNumber(amount);
        target[6]=encodeAddress(destination);
        target[7]=encodeParams();
        return concatAndFreeDataBuffers(target, params);
    }
    
    
    
    
//    // See https://tezos.gitlab.io/api/p2p.html
//    export const kindMapping: { [key: number]: string } = {
//      0x04: 'activate_account',
//      0x6b: 'reveal',
//      0x6e: 'delegation',
//      0x6c: 'transaction',
//      0x6d: 'origination',
//      0x06: 'ballot',
//      0x00: 'endorsement',
//      0x01: 'seed_nonce_revelation',
//      0x05: 'proposals',
//    };

    
    struct Data encodeOperation(struct Operation * operation){
        
        switch (operation->op) {
            case reveal:
                break;
            case delegation:
                break;
            case transaction:
            {
                struct TransactionOperation * tx = (struct TransactionOperation *)operation;
                return encodeTransaction(tx->source,
                                  tx->fee,
                                  tx->counter,
                                  tx->gasLimit,
                                  tx->storageLimit,
                                  tx->amount,
                                  tx->destination);
                //TODO: pass on varargs to encodeTransaction(...);
            }
            default:
                //ERROR
                break;
        }
    }
    
    
    struct Data encodeOperation2(struct Operation2 * operation){
        
        switch (operation->op) {
            case reveal:
                break;
            case delegation:
                break;
            case transaction:
            {
                struct TransactionOperation * tx = operation->details.transaction;
                struct Data txData =  encodeTransaction(tx->source,
                                  tx->fee,
                                  tx->counter,
                                  tx->gasLimit,
                                  tx->storageLimit,
                                  tx->amount,
                                  tx->destination);
                  uint8_t buffer[txData.length+1];
                              buffer[0]= transaction;
                              memcpy(&buffer[1], txData.buffer, txData.length);
                              free(txData.buffer);
                              return uint8tdup(buffer, sizeof(buffer));
                break;
            }
            default:
                //ERROR
                break;
        }
    }
    //[Prefix.B]: new Uint8Array([1, 52]),
    struct Data encodeBranch(char * branch){
        
        size_t len = BRBase58CheckDecode(NULL, 0, branch);
        uint8_t decoded[len];
        BRBase58CheckDecode(&decoded[0], len, branch);
        uint8_t prefix[2] = {1, 52};
        //we want to remove the prefix from the decoded branch
        return uint8tdup(&decoded[sizeof(prefix)],
                         sizeof(decoded)-sizeof(prefix));
    }
    
    struct Data encode(char * branch, struct Operation2 ** operations, size_t opSize){
        struct Data buffer[opSize+2];
        buffer[0] = encodeBranch(branch);
       
        for(int i=0; i < opSize; i ++){
            buffer[i+1]=encodeOperation2(operations[i]);
        }
        
        struct Data result = concatAndFreeDataBuffers(&buffer[0], opSize+1);
        return result;
    }
    
    
    uint8_t * secretEncoder(){
        //UNNEEDED in V0
        //TODO:
    }
    
    
    uint8_t * rawEncoder(){
        //UNNEEDED in V0
        //TODO:
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
