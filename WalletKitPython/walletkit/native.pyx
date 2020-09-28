from core cimport *
from enum import Enum
from libc.stdio cimport printf
import binascii


class HasherType(Enum):
    SHA1 = CRYPTO_HASHER_SHA1
    SHA224 = CRYPTO_HASHER_SHA224
    SHA256 = CRYPTO_HASHER_SHA256
    SHA256_2 = CRYPTO_HASHER_SHA256_2
    SHA384 = CRYPTO_HASHER_SHA384
    SHA512 = CRYPTO_HASHER_SHA512
    SHA3 = CRYPTO_HASHER_SHA3
    RMD160 = CRYPTO_HASHER_RMD160
    HASH160 = CRYPTO_HASHER_HASH160
    KECCAK256 = CRYPTO_HASHER_KECCAK256
    MD5 = CRYPTO_HASHER_MD5


cdef class Hasher:
    cdef BRCryptoHasher _hasher

    def __init__(self, hasher_type: HasherType):
        pass

    def __cinit__(self, kind: HasherType):
        self._hasher = cryptoHasherCreate(kind.value)
        if self._hasher is NULL:
            raise MemoryError

    def __dealloc__(self):
        if self._hasher is not NULL:
            cryptoHasherGive(self._hasher)

    def hash(self, data: bytes) -> bytes:
        hashlen = cryptoHasherLength(self._hasher)
        out = bytes(1) * hashlen
        cryptoHasherHash(self._hasher, out, hashlen, data, len(data))
        return out


# cdef class Account:
#     cdef BRCryptoAccount _account
    

#     def __init__(self):
#         pass

#     def __cinit__(self):
#         pass

#     def __dealloc__(self):
#         pass

#     def createAccount(self):
#         _account = cryptoAccountCreate("hello world", 12321L, "eth")

       

cdef class Account:
    
    def __init__(self):
        pass

    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    def createAccount(self, _seed):

        #cdef unsigned long[64] ZERO = [1L] * 64 
        #cpdef void * seed = <const void *> ZERO[0]
        #print(ZERO)
        cdef UInt512 ss
        ss.u8 = _seed
        key = BRBIP32MasterPubKey( &ss, sizeof(ss))
        print (key.pubKey)
        return key.pubKey

    def test(self,_seed, _phrase):
        #cdef char * paperKey = "ginger settle marine tissue robot crane night number ramp coast roast critic"
        cdef UInt512 ss;
        ss.u8= _seed
        #print(ss.u8)
        BRBIP39DeriveKey (ss.u8, _phrase, NULL);
        print(binascii.hexlify(bytearray(ss.u8)))
        key= BRBIP32MasterPubKey(&ss, sizeof (ss));
        return key.pubKey
        #return ZERO[0]

        #cdef BRMasterPubKey key = _ethAccount.masterPubKey
        #return "Done"

    def derivePrivateKeyFromSeed(self,_seed, _phrase,_index):
        cdef BRKey privateKey
        cdef UInt512 ss;
        ss.u8= _seed
        BRBIP39DeriveKey (ss.u8, _phrase, NULL);
        print(binascii.hexlify(bytearray(ss.u8)))
        cdef unsigned long v1 = 44 | 0x80000000
        cdef unsigned long v2 = 60 | 0x80000000
        cdef unsigned long v3 = 0 | 0x80000000
        cdef unsigned long v4 = 0
        cdef unsigned long index = _index
        # The BIP32 privateKey for m/44'/60'/0'/0/index
        print(v1)
        print(v2)
        print(v3)
        print(v4)
        BRBIP32PrivKeyPath(&privateKey, &ss, sizeof(UInt512), 5, v1,v2,v3,v4,index)#, 60 | 0x80000000,0 | 0x80000000,0,0);                   
        privateKey.compressed = 0;
        cdef size_t keyLen = BRKeyPubKey(&privateKey, NULL, 0);

        print(binascii.hexlify(bytearray(privateKey.secret.u8)))
        print(binascii.hexlify(bytearray(privateKey.pubKey)))

        return privateKey.secret.u8;
