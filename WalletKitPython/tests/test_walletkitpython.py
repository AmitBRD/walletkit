import unittest
from walletkit import Hasher, HasherType, BRSequence
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
      a = BRSequence()
      #512 bit seed, we use first 32 and last 32 seperately
      seed = [0] * 64
      #print(binascii.hexlify(bytearray(seed[0:32])))
      #print(binascii.hexlify(bytearray(seed[32:])))

      #Tests keys generated from https://iancoleman.io/bip39/
      phrase = "ginger settle marine tissue robot crane night number ramp coast roast critic".encode("UTF-8")
      val = a.derive_private_key_from_seed_and_index_eth(seed,phrase,0)
      self.assertEqual(binascii.hexlify(bytearray(val["secret"]["u8"])), b'bf89b834472a1058b83eb4dee176d7c788ea35c70a55ce8a166b4119bfe3b8fb')
      self.assertEqual(binascii.hexlify(bytearray(val["pubKey"][:33])),b'02c6476357bfeabf1eec2bbf283a0ed21b285ec2c5e04edc05bc6e2ca59cce0e87')
      val = a.derive_private_key_from_seed_and_index_eth(seed,phrase,1)
      self.assertEqual(binascii.hexlify(bytearray(val["secret"]["u8"])), b'03fad2c9dcc89519a4c1ac0021cd94f5bc1af981038d71fffac6538251ede2f8')
      self.assertEqual(binascii.hexlify(bytearray(val["pubKey"][:33])),b'03032b815405e235a37552b185c577223fef428dd7f53d5548fc5a11d5646d9220')
      print(binascii.hexlify(bytearray(val["secret"]["u8"])))
      address = a.generate_address_eth(val["pubKey"], val["compressed"])
      print(binascii.hexlify(bytearray(address["bytes"])))
   

if __name__ == '__main__':
    unittest.main()
