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

typedef struct BRTezosAccountRecord *BRTezosAccount;

typedef enum {ed25519, scep256k1, p256} SigScheme;

extern UInt512 getTezosSeed(const char *paperKey);

extern UInt512 getTezosSeedWithPassword( const char *paperKey, const char* passphrase);

extern BRKey deriveTezosKey(SigScheme scheme, UInt512 seed, uint32_t index, bool cleanPrivateKey);

extern BRTezosAccount createAccountObject(BRKey*key);

extern char * getPublicKeyHash(char* publicKey);
    
extern BRTezosAccount tezosAccountCreate(SigScheme scheme, char* paperKey);

extern void BIP32Ed25519PrivKeyPath(BRKey *key, const void *seed, size_t seedLen, int depth, va_list vlist);

extern void createAccount();



#endif /* BRTezos_h */
