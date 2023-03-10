//
//  prover.hpp
//  Bits
//
//  Created by Burak on 8.12.2022.
//

#ifndef prover_h
#define prover_h

#include <stdio.h>
#include "../util/wizdata.h"
#include "../crypto/sha256.h"
#include "../transaction/transaction.h"
#include "../utxo/utxo.h"
#include "../utreexo.h"

class ProverSync {
public:
    static uint32_t elapsedUTXOs;
    static utreexo::UndoBatch undo;
    static utreexo::RamForest full;
    
    static std::vector<UTXO*> utxoSet;
    
    static std::pair<uint32_t, UTXO*> returnUTXOFromOutpoint(valtype prevHash, uint32_t vout, int *exist);
    static uint32_t proverHeight;
    ProverSync() {};
    ~ProverSync() { std::cout <<  " ddeess ps " << std::endl; };
};

class Prover {
private:
    utreexo::BatchProof proof;
    valtype compactSpendingsRaw;
    std::vector<UTXO*> spendings;
    std::vector<Hash> spendingsHashes;
    void setCompactSpendingsRaw();
public:
    Prover(valtype vRawBlock);
    valtype readSpendingsRaw();
    utreexo::BatchProof readProof();
    
};

#endif /* prover_h */
