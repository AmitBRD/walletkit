//
//  BRTezos.h
//  
//
//  Created by Amit on 04/06/2020.
//

#ifndef BRTezos_h
#define BRTezos_h

#include <stdio.h>
#include <stdbool.h>
#include "support/BRInt.h"
#include "support/BRKey.h"
#include <stdarg.h>
#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct BRTezosAccountRecord *BRTezosAccount;

typedef enum {ed25519, scep256k1, p256} SigScheme;

UInt512 getTezosSeed(const char *paperKey);

UInt512 getTezosSeedWithPassword( const char *paperKey, const char* passphrase);

BRKey deriveTezosKey(SigScheme scheme, UInt512 seed, uint32_t index, bool cleanPrivateKey);

BRTezosAccount createAccountObject(BRKey*key);

char * getPublicKeyHash(char* publicKey);
    
BRTezosAccount tezosAccountCreate(SigScheme scheme, char* paperKey);

void BIP32Ed25519PrivKeyPath(BRKey *key, const void *seed, size_t seedLen, int depth, ...);

void __attribute__((overloadable)) BIP32Ed25519PrivKeyPath(BRKey *key, const *seed, size_t seedLen, int depth, va_list vlist);

void createAccount();

#ifdef __cplusplus
}
#endif

#endif /* BRTezos_h */
