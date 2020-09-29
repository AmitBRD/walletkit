from libc.stdint cimport uint8_t


cdef extern from "BRCryptoBase.h":
    ctypedef enum BRCryptoBoolean:
        CRYPTO_FALSE,
        CRYPTO_TRUE


cdef extern from "BRCryptoHasher.h":
    ctypedef enum BRCryptoHasherType:
        CRYPTO_HASHER_SHA1,
        CRYPTO_HASHER_SHA224,
        CRYPTO_HASHER_SHA256,
        CRYPTO_HASHER_SHA256_2,
        CRYPTO_HASHER_SHA384,
        CRYPTO_HASHER_SHA512,
        CRYPTO_HASHER_SHA3,
        CRYPTO_HASHER_RMD160,
        CRYPTO_HASHER_HASH160,
        CRYPTO_HASHER_KECCAK256,
        CRYPTO_HASHER_MD5

    cdef struct BRCryptoHasherRecord:
        pass

    ctypedef BRCryptoHasherRecord *BRCryptoHasher

    BRCryptoHasher cryptoHasherCreate(BRCryptoHasherType type)
    size_t cryptoHasherLength(BRCryptoHasher hasher)
    BRCryptoBoolean cryptoHasherHash(BRCryptoHasher hasher,
                                     uint8_t *dst,
                                     size_t dst_len,
                                     const uint8_t *src,
                                     size_t src_len)
    BRCryptoHasher cryptoHasherTake(BRCryptoHasher instance)
    BRCryptoHasher cryptoHasherTakeWeak(BRCryptoHasher instance)
    void cryptoHasherGive(BRCryptoHasher instance)

cdef extern from "../src/support/BRInt.h":
    ctypedef unsigned int uint32_t
    ctypedef unsigned long uint64_t
    ctypedef unsigned int uint8_t
    ctypedef unsigned int uint16_t

    ctypedef union UInt512: 
        uint8_t u8[64]
        uint16_t u16[32]
        uint32_t u32[16]
        uint64_t u64[8]
    ctypedef union UInt256:
        uint8_t u8[32]
        uint16_t u16[16]
        uint32_t u32[8]
        uint64_t u64[4]


cdef extern from "../src/ethereum/base/BREthereumAddress.h":
    ctypedef unsigned int uint8_t
    ctypedef struct BREthereumAddress:
        uint8_t bytes[20]
    BREthereumAddress ethAddressCreateKey (const BRKey *key)

cdef extern from "BRCryptoAccount.h":
    ctypedef unsigned long uint64_t
    
    ctypedef struct BRCryptoAccountRecord:
       pass
    ctypedef BRCryptoAccountRecord *BRCryptoAccount
    

    BRCryptoAccount cryptoAccountCreate(const char *paperKey, uint64_t timestamp, const char *uids)

cdef extern from "../src/support/BRBip32Sequence.h":
    ctypedef unsigned int uint32_t
    ctypedef unsigned long uint64_t
    ctypedef unsigned int uint8_t
    ctypedef struct BRMasterPubKey:
        uint8_t fingerPrint[4]
        uint8_t chainCode[32]
        uint8_t pubKey[33]

    ctypedef struct BRKey:
        UInt256 secret
        uint8_t pubKey[65]
        int compressed

    BRMasterPubKey BRBIP32MasterPubKey(const void *seed, size_t seedLen)
    void BRBIP32PrivKeyPath(BRKey *key, const void *seed, size_t seedLen, int depth, ...)
    size_t BRKeyPubKey(BRKey *key, void *pubKey, size_t pkLen)


cdef extern from "../src/support/BRBIP39Mnemonic.h":
    void BRBIP39DeriveKey(void *key64, const char *phrase, const char *passphrase)

# cdef extern from "../src/ethereum/ewm/BREthereumAccount.h":
    
   
#     # ctypedef struct BREthereumAddress:
#     #     uint8_t bytes[20]

   

#     # ctypedef struct BREthereumAddressDetailRecord:
#     #     BREthereumAddress raw
#     #     char string[43]
#     #     uint8_t publicKey [64]
#     #     uint32_t index
#     #     uint64_t nonce

#     cdef struct BREthereumAddressDetailRecord:     
#         pass

#     ctypedef BREthereumAddressDetailRecord * BREthereumAddressDetail
    
#     cdef struct BREthereumAccountRecord:
#         BRMasterPubKey masterPubKey
#         BREthereumAddressDetail primaryAddress
#         pass
        
#     ctypedef BREthereumAccountRecord *BREthereumAccount

#     BREthereumAccount ethAccountCreate(const char *paperKey);