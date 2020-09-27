import unittest
from walletkit import Hasher, HasherType, Account
import binascii


class TestHasher(unittest.TestCase):
    def test_do_hash(self):
        h = Hasher(HasherType.SHA256)
        v = h.hash('hello'.encode('utf8'))
        self.assertEqual(v.hex(), '2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824')
        

class TestKey(unittest.TestCase):
    # def test_generate_key(self):
    #   a = Account()
    #   #512 bit seed, we use first 32 and last 32 seperately
    #   seed = [0] * 64
    #   #print(binascii.hexlify(bytearray(seed[0:32])))
    #   #print(binascii.hexlify(bytearray(seed[32:])))
    #   val = a.createAccount(seed)
    #   #print(binascii.hexlify(bytearray(val)))
    # def test_generate_key_with_mnemonic(self):
    #   a = Account()
    #   #512 bit seed, we use first 32 and last 32 seperately
    #   seed = [0] * 64
    #   #print(binascii.hexlify(bytearray(seed[0:32])))
    #   #print(binascii.hexlify(bytearray(seed[32:])))
    #   phrase = "ginger settle marine tissue robot crane night number ramp coast roast critic".encode("UTF-8")
    #   val = a.test(seed,phrase)
    #   print(binascii.hexlify(bytearray(val)))

     def test_generate_private_key_with_mnemonic(self):
      a = Account()
      #512 bit seed, we use first 32 and last 32 seperately
      seed = [0] * 64
      #print(binascii.hexlify(bytearray(seed[0:32])))
      #print(binascii.hexlify(bytearray(seed[32:])))
      phrase = "ginger settle marine tissue robot crane night number ramp coast roast critic".encode("UTF-8")
      val = a.derivePrivateKeyFromSeed(seed,phrase,0)
      print(binascii.hexlify(bytearray(val)))
   

if __name__ == '__main__':
    unittest.main()
