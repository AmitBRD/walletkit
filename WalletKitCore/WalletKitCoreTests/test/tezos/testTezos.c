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
    
    encoded = encodeDelegate("tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn",1);
    printf("\r\n Delegate expected:ff0035e993d8c7aaa42b5e3ccd86a33390ececc73abd actual:");
         printBuffer(encoded);
         free(encoded.buffer);
    
    encoded = encodeDelegate(NULL,0);
    printf("\r\n Delegate expected:00 actual:");
         printBuffer(encoded);
         free(encoded.buffer);
    
    encoded = encodeDelegate2("tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn");
    printf("\r\n Delegate expected:ff0035e993d8c7aaa42b5e3ccd86a33390ececc73abd actual:");
         printBuffer(encoded);
         free(encoded.buffer);
    
    encoded = encodeDelegate2(NULL);
    printf("\r\n Delegate expected:00 actual:");
         printBuffer(encoded);
         free(encoded.buffer);
    
    struct TransactionOperation * tx = malloc(sizeof(struct TransactionOperation));
    tx->operation.op = transaction;
    tx->counter = uint256("0000000000000000000000000000000000000000000000000000000000000001");//1
    tx->amount =uint256("00000000000000000000000000000000000000000000000000000000000003e8");//1000
    tx->fee =uint256("0000000000000000000000000000000000000000000000000000000000002710");//10000
    tx->gasLimit =uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
    tx->storageLimit=uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
    tx->source= "tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn";
    tx->destination="tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn";
    
    struct Data encodedMsg = encodeOperation((struct Operation * )tx);
    printf("\r\n Transaction expected:0035e993d8c7aaa42b5e3ccd86a33390ececc73abd904e010a0ae807000035e993d8c7aaa42b5e3ccd86a33390ececc73abd00 actual:");
    printBuffer(encodedMsg);
    free(encodedMsg.buffer);
    
    struct Operation2 * operations[1];
    
    operations[0] = malloc(sizeof(struct Operation2));
    operations[0]->details.transaction = tx;
    operations[0]->op = transaction;
    
    
    encodedMsg = encode("BLzyjjHKEKMULtvkpSHxuZxx6ei6fpntH2BTkYZiLgs8zLVstvX", &operations[0], 1 );
    printf("\r\n Transaction expected:a99b946c97ada0f42c1bdeae0383db7893351232a832d00d0cd716eb6f66e5616c0035e993d8c7aaa42b5e3ccd86a33390ececc73abd904e010a0ae807000035e993d8c7aaa42b5e3ccd86a33390ececc73abd00 actual:");
    printBuffer(encodedMsg);
    free(encodedMsg.buffer);
    free(operations[0]->details.transaction);
    free(operations[0]);
    
    
    
    struct DelegateOperation * dtx = malloc(sizeof(struct DelegateOperation));
     dtx->counter = uint256("0000000000000000000000000000000000000000000000000000000000000001");//1
     dtx->fee =uint256("0000000000000000000000000000000000000000000000000000000000002710");//10000
     dtx->gasLimit =uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
     dtx->storageLimit=uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
     dtx->source= "tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn";
     dtx->delegate="tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn";
    operations[0] = malloc(sizeof(struct Operation2));
    operations[0]->details.delegation = dtx;
    operations[0]->op = delegation;
    
    encodedMsg = encode("BLzyjjHKEKMULtvkpSHxuZxx6ei6fpntH2BTkYZiLgs8zLVstvX", &operations[0], 1 );
    printf("\r\n Delegation expected:a99b946c97ada0f42c1bdeae0383db7893351232a832d00d0cd716eb6f66e5616e0035e993d8c7aaa42b5e3ccd86a33390ececc73abd904e010a0aff0035e993d8c7aaa42b5e3ccd86a33390ececc73abd actual:");
    printBuffer(encodedMsg);
    free(encodedMsg.buffer);
    free(operations[0]->details.delegation);
    free(operations[0]);
    
    
    
    struct RevealOperation * rtx = malloc(sizeof(struct RevealOperation));
        rtx->counter = uint256("0000000000000000000000000000000000000000000000000000000000000001");//1
        rtx->fee =uint256("0000000000000000000000000000000000000000000000000000000000002710");//10000
        rtx->gasLimit =uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
        rtx->storageLimit=uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
        rtx->source= "tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn";
        rtx->publicKey="edpkvS5QFv7KRGfa3b87gg9DBpxSm3NpSwnjhUjNBQrRUUR66F7C9g";
       operations[0] = malloc(sizeof(struct Operation2));
       operations[0]->details.reveal = rtx;
       operations[0]->op = reveal;
       
       encodedMsg = encode("BLzyjjHKEKMULtvkpSHxuZxx6ei6fpntH2BTkYZiLgs8zLVstvX", &operations[0], 1 );
       printf("\r\n Reveal expected:a99b946c97ada0f42c1bdeae0383db7893351232a832d00d0cd716eb6f66e5616b0035e993d8c7aaa42b5e3ccd86a33390ececc73abd904e010a0a00ebcf82872f4942052704e95dc4bfa0538503dbece27414a39b6650bcecbff896 actual:");
       printBuffer(encodedMsg);
       free(encodedMsg.buffer);
       free(operations[0]->details.reveal);
       free(operations[0]);
    
    
    //TODO: test array of operations e.g. reveal + transaction
    struct Operation2 * operationsMulti[2];
    operationsMulti[0]= malloc(sizeof(struct Operation2));
    operationsMulti[1]= malloc(sizeof(struct Operation2));
    
    rtx = malloc(sizeof(struct RevealOperation));
    rtx->counter = uint256("0000000000000000000000000000000000000000000000000000000000000001");//1
    rtx->fee =uint256("0000000000000000000000000000000000000000000000000000000000002710");//10000
    rtx->gasLimit =uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
    rtx->storageLimit=uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
    rtx->source= "tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn";
    rtx->publicKey="edpkvS5QFv7KRGfa3b87gg9DBpxSm3NpSwnjhUjNBQrRUUR66F7C9g";
    operationsMulti[0]->details.reveal = rtx;
    operationsMulti[0]->op = reveal;
    tx = malloc(sizeof(struct TransactionOperation));
    tx->operation.op = transaction;
    tx->counter = uint256("0000000000000000000000000000000000000000000000000000000000000001");//1
    tx->amount =uint256("00000000000000000000000000000000000000000000000000000000000003e8");//1000
    tx->fee =uint256("0000000000000000000000000000000000000000000000000000000000002710");//10000
    tx->gasLimit =uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
    tx->storageLimit=uint256("000000000000000000000000000000000000000000000000000000000000000a");//10
    tx->source= "tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn";
    tx->destination="tz1QZ6KY7d3BuZDT1d19dUxoQrtFPN2QJ3hn";
    operationsMulti[1]->details.transaction = tx;
    operationsMulti[1]->op = transaction;    //TODO: figure out gas estimation
    
    
    encodedMsg =encode("BLzyjjHKEKMULtvkpSHxuZxx6ei6fpntH2BTkYZiLgs8zLVstvX", operationsMulti, 2 );
    printf("\r\n Multi Operations expected:a99b946c97ada0f42c1bdeae0383db7893351232a832d00d0cd716eb6f66e5616b0035e993d8c7aaa42b5e3ccd86a33390ececc73abd904e010a0a00ebcf82872f4942052704e95dc4bfa0538503dbece27414a39b6650bcecbff8966c0035e993d8c7aaa42b5e3ccd86a33390ececc73abd904e010a0ae807000035e993d8c7aaa42b5e3ccd86a33390ececc73abd00 actual:");
    printBuffer(encodedMsg);
    
    free(operationsMulti[0]->details.reveal);
    free(operationsMulti[1]->details.transaction);
    free(operationsMulti[0]);
    free(operationsMulti[1]);
    //TODO: test sign the transaction
    
    uint32_t v = -123654;
    char * encoded2 = encodeInt32(&v);
    free(encoded2);
}
