//
//  BRCryptoFeeBasisP.h
//  BRCore
//
//  Created by Ed Gamble on 11/22/19.
//  Copyright © 2019 Breadwinner AG. All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.

#ifndef BRCryptoFeeBasisP_h
#define BRCryptoFeeBasisP_h

#include "BRCryptoFeeBasis.h"
#include "BRCryptoBaseP.h"

#ifdef __cplusplus
extern "C" {
#endif

struct BRCryptoFeeBasisRecord {
    BRCryptoAmount costPerPriceFactor;
    double costFactor;
    BRCryptoRef ref;
};

#ifdef REFACTOR
 private_extern uint64_t
 cryptoFeeBasisAsBTC (BRCryptoFeeBasis feeBasis);

 private_extern BREthereumFeeBasis
 cryptoFeeBasisAsETH (BRCryptoFeeBasis feeBasis);

 private_extern BRGenericFeeBasis
 cryptoFeeBasisAsGEN (BRCryptoFeeBasis feeBasis);

 private_extern BRCryptoFeeBasis
 cryptoFeeBasisCreateAsBTC (BRCryptoUnit unit,
                            uint32_t feePerKB,
                            uint32_t sizeInByte);

 private_extern BRCryptoFeeBasis
 cryptoFeeBasisCreateAsETH (BRCryptoUnit unit,
                            BREthereumGas gas,
                            BREthereumGasPrice gasPrice);

 private_extern BRCryptoFeeBasis
 cryptoFeeBasisCreateAsGEN (BRCryptoUnit unit,
                            OwnershipGiven BRGenericFeeBasis bid);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BRCryptoFeeBasisP_h */
