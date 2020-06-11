//
//  File.c
//  
//
//  Created by Amit on 04/06/2020.
//

#include "BRTezos.h"
#include <assert.h>
#include <stdlib.h>
#include "support/BRKey.h"
#include "support/BRBase58.h"
#include "support/BRBIP32Sequence.h"
#include "support/BRBIP39WordsEn.h"
#include "support/BRCrypto.h"
#include "support/BRBase58.h"
#include "blake2b/blake2b.h"
#include "ed25519/ed25519.h"


#ifdef __cplusplus
extern "C" {
#endif
    
    #define BUFSIZE 1 /* Read buffer size. */
    #define MDSIZE 64 /* Message digest 512 bit. */
    #define BLAKE20 20 /* Message digest 512 bit. */
    #define ED25519_CURVE  "ed25519 seed"
    #define HARDENED_OFFSET  0x80000000
    
    
    //https://github.com/ecadlabs/taquito/blob/master/packages/taquito-utils/src/constants.ts
    const uint8_t EDSK2_PREFIX[4] = {13, 15, 58, 7};
    const uint8_t EDPK_PREFIX[4] = {13, 15, 37, 217};
    const uint8_t TZ1_PREFIX[3]={6, 161, 159};
    
    static void TezosCKDpriv(UInt256 *k, UInt256 *c, uint32_t i);
    static void printHex(UInt512 *k);
    static void printChar(char *k);
    struct BRTezosAccountRecord {
        char * pkh; // The 20 byte blake2b hash of pk
        SigScheme scheme;
        // The public key - needed when sending
        BRKey publicKey;  // BIP44: 'Master Public Key 'M' (264 bits) - 8
        char * publickKey;
        char * privateKey;
        uint32_t index;     // The BIP-44 Index used for this key.
    };

    //https://github.com/satoshilabs/slips/blob/master/slip-0010.md
    void __attribute__((overloadable)) BIP32Ed25519PrivKeyPath(BRKey *key, const void *seed, size_t seedLen, int depth, va_list vlist)
    {
        UInt512 I;
        UInt256 IL, IR;
        
        assert(key != NULL);
        assert(seed != NULL || seedLen == 0);
        assert(depth >= 0);
       
        if (key && (seed || seedLen == 0)) {
            BRHMAC(&I, BRSHA512, sizeof(UInt512), ED25519_CURVE,strlen
                   (ED25519_CURVE), seed, seedLen);
            IL = *(UInt256 *)&I;//key
            IR = *(UInt256 *)&I.u8[sizeof(UInt256)];//chaincode
            var_clean(&I);
           
            for (int i = 0; i < depth; i++) {
                TezosCKDpriv(&IL, &IR,va_arg(vlist, uint32_t));
            }
           
            //https://dune.network/ledger_extract/
            printf("expected secret b1e9eaa8e418370bf9abd524761c306301d7e49e8783b2cd55060214d6ca0b9d, actual: ");
            printHex(&IL.u8);//secret
            
            printf("\r\n chaincode:");
            printHex(&IR.u8);//chaincode
            
            unsigned char publicKey[32]={0};
            unsigned char secretKey[64]={0};//unneeded
            ed25519_create_keypair(publicKey, secretKey,IL.u8);
            printf("expected public key efc82a1445744a87fec55fce35e1b7ec80f9bbed9df2a03bcdde1a346f3d4294, actual:");
            for(int i=0; i < 32; i++){
                printf("%02x",publicKey[i]);
            }
            
            uint8_t pkh[BLAKE20];
            blake2b(pkh, BLAKE20, NULL, 0, &publicKey[0], sizeof(publicKey));
            
            
            printf("\r\n Pkh expected 4cdee21a9180f80956ab8d27fb6abdbd89934052, actual:");
            for(int i=0; i < BLAKE20; i++){
                           printf("%02x",pkh[i]);
            }
            
            uint8_t * buffer = malloc(((sizeof(TZ1_PREFIX)+sizeof(pkh)))*sizeof(uint8_t));
            memcpy(buffer,     TZ1_PREFIX, sizeof(TZ1_PREFIX) * sizeof(uint8_t));
            memcpy(buffer + sizeof(TZ1_PREFIX), pkh, sizeof(pkh) * sizeof(uint8_t));
            printf("\r\n buffer expected 06a19f4cdee21a9180f80956ab8d27fb6abdbd89934052 actual:");
            for(int i=0; i < ((sizeof(TZ1_PREFIX)+sizeof(pkh)))*sizeof(uint8_t) ; i++){
                           printf("%02x",buffer[i]);
            }
            
            uint8_t checkSum[32];
            BRSHA256_2(checkSum, buffer,((sizeof(TZ1_PREFIX)+sizeof(pkh)))*sizeof(uint8_t));
            
            printf("\r\n checksum expected 2669459120fc07bff78f00f343f84297066ad6b5f426e4076952bb0c982a97d8 actual:");
            for(int i=0; i < 32; i++){
                           printf("%02x",checkSum[i]);
            }
            
            
            
            uint8_t payload[((sizeof(TZ1_PREFIX)+sizeof(pkh) + 4))];
                             
            memcpy(&payload, buffer, (sizeof(TZ1_PREFIX)+sizeof(pkh))*sizeof(uint8_t));
            memcpy(&payload[(sizeof(TZ1_PREFIX)+sizeof(pkh))], checkSum,4 * sizeof(uint8_t));
            
            printf("\r\n payload expected 06a19f4cdee21a9180f80956ab8d27fb6abdbd8993405226694591 actual:");
                       for(int i=0; i < sizeof(payload); i++){
                                      printf("%02x",payload[i]);
                       }
//            buffer = malloc((sizeof(pkh)+4)* sizeof(uint8_t));
//            memcpy(buffer,     pkh, sizeof(pkh) * sizeof(uint8_t));
//            memcpy(buffer + sizeof(pkh), checkSum, 4 * sizeof(uint8_t));
            char pk58[BRBase58Encode(NULL, 0, payload, sizeof(payload))];
            BRBase58Encode(pk58, sizeof(pk58), payload,sizeof(payload));
//
            free(buffer);
            //free(payload);
            
            printf("\r\n\r\n pk58 expected tz1SeV3tueHQMTfquZSU7y98otvQTw6GDKaY actual: %s",pk58);
            
            
            printf("\r\nDONE GENERATING ed25519");
            var_clean(&IL, &IR);
        }
    }
    
    
    void  BIP32Ed25519PrivKeyPath(BRKey * key, const void * seed, size_t seedLen, int depth , ... ){
        va_list ap;

        va_start(ap, depth);
        BIP32Ed25519PrivKeyPath(key, seed, seedLen, depth, ap);
        va_end(ap);
    }
    
    
    
    void createAccount()
        {
            //
            
            uint8_t md[BLAKE20];
            blake2b_ctx ctx;

            uint8_t buf[5] ={'h','e','l','l','o'};
            size_t i = 5;
            
           
            blake2b(md,BLAKE20, NULL,0, buf,5);
            
        //    /* Initialize blake2b hashing context. */
        //    if (blake2b_init(&ctx, MDSIZE, NULL, 0)){
        //     return -1;
        //    }
        //
        //    bl
        //    /* Final round. */
        //    blake2b_final(&ctx, md);
            for (i = 0; i < BLAKE20; i++)
            {
                //20 byte: b5531c7037f06c9f2947132a6a77202c308e8939
                //32 byte: 324dcf027dd4a30a932c441f365a25e86b173defa4b8e58948253471b81b72cf
             printf("%02x", md[i]);
            }
            unsigned char sk[32] = {0};
            unsigned char pk[64] = {0};
            ed25519_create_keypair(pk,sk,&md);
            const char * paper_key = "patient doctor olympic frog force glimpse endless antenna online dragon bargain someone";
            
            const char* mnemonic =  "narrow ordinary minimum tennis casual wash soul pretty impulse provide panic donor three long inquiry";
            UInt512 seed = getTezosSeedWithPassword(mnemonic,"nmkkzarm.ywzrrhcl@tezos.example.orgdM4M4eKyFv");
            
            uint8_t edSeed[32]={0};
            //this is 512 bit, we need 256 bit
            UInt512 seed2 = getTezosSeed(paper_key);
            
            
    //        memcpy(edSeed, seed2.u8, 32);
    //        unsigned char publicKey[32]={0};
    //        unsigned char secretKey[64]={0};
    //        ed25519_create_keypair(publicKey, secretKey,edSeed);
    //
    //
    //        uint8_t prefix[5]={'e','d','s','k'};
    //        uint8_t preHash[37];
    //        memcpy(preHash,prefix,sizeof(prefix));
    //        memcpy(preHash+sizeof(prefix),seed.u8,32);
    //        char key[BRBase58Encode(NULL, 0, preHash, BLAKE20)];
    //        BRBase58Encode(key, sizeof(key), preHash, BLAKE20);
    //
    //        SigScheme ss = ed25519;
    //        tezosAccountCreate(ss, paper_key);
    //
    //
            
            printf("seed:");
            printHex(&seed2);
            BRKey brkey;
            
            BIP32Ed25519PrivKeyPath(&brkey,&seed2, sizeof(UInt512), 4, 44 | BIP32_HARD, 1729 | BIP32_HARD, 0 | BIP32_HARD, 0| BIP32_HARD );
            

       }
    
    
    
    static BRKey deriveTezosKeyFromSeed (UInt512 seed, uint32_t index, bool cleanPrivateKey)
       {
           SigScheme scheme = ed25519;
           BRKey key;
           mem_clean(&key, sizeof(BRKey));
           
          
//           switch (scheme) {
//               case ed25519:
//                    unsigned char sk[64] = {0};
//                    unsigned char pk[32] = {0};
//                    ed25519_create_keypair(pk,sk,seed.u8);
//                   break;
//
//               default:
//                   break;
//           }
           
           //seems new tezos uses Bip32 for ed25519
           BRBIP32PrivKeyPath(&key, &seed, 32, 5,
                              44 | BIP32_HARD,          // purpose  : BIP-44
                              1729 | BIP32_HARD,        // coin_type: Tezos
                              0 | BIP32_HARD,          // account  : <n/a>
                              0,                        // change   : not change
                              index);                   // index    :

           // Generate the compressed public key
           key.compressed = 1;
           
           
           
           //if scheme is ecdsa we are done
           //BRKeyPubKey(&key, &key.pubKey, 33);
           
           if (cleanPrivateKey) {
               // In some cases we don't want to wipe the secret: i.e. during signing
               mem_clean(&key.secret, sizeof(key.secret));
           }

           
          
           return key;
       }
    

   
    
    static void TezosCKDpriv(UInt256 *il, UInt256 *ir, uint32_t i)
    {
        //[prefix:1byte, key:32byte, index:4byte] = 37bytes
        uint8_t buf[33 + sizeof(i)];
        UInt512 I;
        
        if (i & BIP32_HARD) {
            buf[0] = 0;
            UInt256Set(&buf[1], *il);
        }else{
            //https://github.com/satoshilabs/slips/blob/master/slip-0010.md
            //ed25519 only supports hardened key paths
            return;
        }
        //else BRSecp256k1PointGen((BRECPoint *)buf, il);
        
        UInt32SetBE(&buf[33], i);
        
        BRHMAC(&I, BRSHA512, sizeof(UInt512), ir, sizeof(*ir), buf, sizeof(buf)); // I = HMAC-SHA512(c, k|P(k) || i)
        //var_clean(k,c);
        //do we not have to do this because its a stack variable so it will fall out of scope
        
        *il = *(UInt256 *)&I;//key
        *ir = *(UInt256 *)&I.u8[sizeof(UInt256)];//chaincode
        var_clean(&I);
        
//        BRSecp256k1ModAdd(k, (UInt256 *)&I); // k = IL + k (mod n)
//        *c = *(UInt256 *)&I.u8[sizeof(UInt256)]; // c = IR
        
        var_clean(&I);
        mem_clean(buf, sizeof(buf));
    }
    
   
    static void printHex(UInt512 *k){
        printf("\r\n");
        for (int i=0;i < (sizeof (k->u8) /sizeof (k->u8[0]));i++) {
            printf("%02x",k->u8[i]);
        }
        printf("\r\n");

    }

    static void printChar(char *s){
        printf("\r\n");
        char * t; // first copy the pointer to not change the original
        for (t = s; s != '\0'; t++) {
            printf("%s",t);
        }
        printf("\r\n");

    }
    
    BRTezosAccount tezosAccountCreate(SigScheme scheme, char* paperKey){
        UInt512 seed = getTezosSeed(paperKey);
        
        printf("\r\n");
        for (int i=0;i < (sizeof (seed.u8) /sizeof (seed.u8[0]));i++) {
            printf("%02x",seed.u8[i]);
        }
        printf("\r\n");
        
        BRKey key = deriveTezosKeyFromSeed (seed, 0, false);
        return createAccountObject(&key);
        
    }
    
    UInt512 getTezosSeed( const char *paperKey)
    {
        // Generate the 512bit private key using a BIP39 paperKey
        UInt512 seed = UINT512_ZERO;
        //it appears we never use a password on our paper wallets
        BRBIP39DeriveKey(seed.u8, paperKey, NULL); // no passphrase
        return seed;
    }
    
    UInt512 getTezosSeedWithPassword( const char *paperKey, const char* passphrase)
    {
        // Generate the 512bit private key using a BIP39 paperKey
        UInt512 seed = UINT512_ZERO;
        //it appears we never use a password on our paper wallets
        BRBIP39DeriveKey(seed.u8, paperKey, passphrase); // no passphrase
        return seed;
    }
    
    
    
    BRTezosAccount createAccountObject(BRKey *key){
         BRTezosAccount account = (BRTezosAccount) calloc (1, sizeof (struct BRTezosAccountRecord));
        return account;
    }

    BRKey deriveTezosKey(SigScheme scheme, UInt512 seed, uint32_t index, bool cleanPrivateKey)
    {
        BRKey key;
        unsigned char sk[32] = {0};
        unsigned char pk[64] = {0};
        //32 byte seed is used to create public private key pair?
        ed25519_create_keypair(pk  , sk, &seed.u8);
        
        https://gitlab.com/dune-network/ledger-extract/-/blob/master/static/index.html
        //m/44'/1729'/0'/0
        
        return key;
    }
    
    void generateTezosAccount(SigScheme scheme){
        switch(scheme){
            case ed25519:
                printf("neg inf");
               
            case scep256k1:
                printf("zero");
               
            case p256:
                printf("pos inf");
               
            default: printf("UNKNOWN SIGNATURE SCHEME");
                break;
        }
    }
    
    char * getPublicKeyHash(char* publicKey){
//        P2HASH_MAGIC = bytes.fromhex('06a1a4')
//        blake2bhash = blake2b(pubkey, digest_size=20).digest()
//        shabytes = sha256(sha256(P2HASH_MAGIC + blake2bhash).digest()).digest()[:4]
//        pkhash = b58encode(P2HASH_MAGIC + blake2bhash + shabytes).decode()
        
        uint8_t md[BLAKE20];
        uint8_t md2[32];
        blake2b(md,BLAKE20, NULL,0, publicKey,64);
        BRSHA256_2(md2, md,BLAKE20);
        char pkh[BRBase58Encode(NULL, 0, md, BLAKE20)];
        BRBase58Encode(pkh, sizeof(pkh), md, BLAKE20);
        return pkh;
    }
    
    

    
    //Reference implementation
    //https://github.com/baking-bad/pytezos/blob/dd1e67bb38c42382f0e156b72e0d3f39a4cccfde/pytezos/crypto.py#L319
    
//    extern void generate(){
//    mnemonic = Mnemonic(language).generate(strength)
//    key = cls.from_mnemonic(mnemonic, passphrase, curve=curve)
//
//    if export:
//        pkh = key.public_key_hash()
//        def public_key_hash(self):
//        """
//        Creates base58 encoded public key hash for this key.
//        :return: the public key hash for this key
//        """
//        pkh = blake2b(data=self.public_point, digest_size=20).digest()
//        prefix = {b'ed': b'tz1', b'sp': b'tz2', b'p2': b'tz3'}[self.curve]
//        return base58_encode(pkh, prefix).decode()
//        data = {
//            'mnemonic': mnemonic.split(),
//            'pkh': pkh,
//            'password': passphrase
//        }
//        with open(abspath(f'./{pkh}.json'), 'w+') as f:
//            f.write(json.dumps(data))
//
//    return key
//
//    }
//    }
    
    
    
    
    //generating test keys for different signing algos:
    //./tezos-signer gen keys alice -s ed25519|scep256k1|p256
    //cat ~/.tezos-signer/public_key_hashs
    
    /*{
      "mnemonic": [
        "twelve",
        "manual",
        "april",
        "glare",
        "typical",
        "business",
        "stove",
        "journey",
        "room",
        "assist",
        "arm",
        "install",
        "ugly",
        "million",
        "decide"
      ],
      "secret": "4e6ceec74a95916230dc4d47e3da4ad7bf856308",
      "amount": "314152183",
      "pkh": "tz1YwpXP4jrua7r1DJZeij6ZyDLKmJttpiNZ",
      "password": "rRZN2oseLU",
      "email": "ktservrz.oybekpsc@tezos.example.org"
    }*/
    
#ifdef __cplusplus
}
#endif
