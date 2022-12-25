//
//  prover.cpp
//  Bits
//
//  Created by Burak on 8.12.2022.
//

#include "prover.h"
#include "../block/block.h"

uint32_t ProverSync::proverHeight = 0;
std::vector<UTXO*> ProverSync::utxoSet;

utreexo::RamForest ProverSync::full(0);
utreexo::UndoBatch ProverSync::undo;
uint32_t ProverSync::elapsedUTXOs = 0;

Prover::Prover(valtype vRawBlock) {
    
    //0. Craft block template
    Block nb = Block::submitNewBlock(vRawBlock);
    std::vector<Transaction> transactions = nb.transactions;
    
    //1. Collect spending utxos
    for (int i = 0; i < transactions.size(); i++) {
        if(i > 0){
        for (uint32_t k = 0; k < transactions[i].inputs.size(); k++) {
            int exist = 0;
            std::pair<uint32_t, UTXO*> spendingUTXO = ProverSync::returnUTXOFromOutpoint(transactions[i].inputs[k].prevOutHash, transactions[i].inputs[k].voutIndex, &exist);
            if (exist == 1)
                this->spendings.push_back(spendingUTXO.second);
        }
        }
    }

    //2. setSpendingsRaw
    setCompactSpendingsRaw();
    
    //3. Craft hash array of spendings
    for(int i = 0; i < this->spendings.size(); i++) {
        this->spendingsHashes.push_back(this->spendings[i]->returnLeafHash());
    }

    //4. Craft block proof
    ProverSync::full.Prove(this->proof, this->spendingsHashes);
    
    std::vector<Leaf> newLeaves;
    std::vector<UTXO*> newUTXOs;
    
    for (int i = 0; i < transactions.size(); i++) {
        
        //5. Collect new outputs
        for (uint32_t k = 0; k < transactions[i].outputs.size(); k++) {
            std::cout << "outs before log" << std::endl;
            UTXO *newUTXO;
            newUTXO = new UTXO(ProverSync::proverHeight + 1, transactions[i].txid, k, (transactions[i].outputs[k].amount), transactions[i].outputs[k].scriptPubkey);
            newUTXOs.push_back(newUTXO);
            newLeaves.emplace_back(newUTXO->returnLeafHash(), false);
        }
 
        if(i > 0){
        for (uint32_t k = 0; k < transactions[i].inputs.size(); k++) {
            
            int exist = 0;
            std::pair<uint32_t, UTXO*> removeUTXO;
            removeUTXO = ProverSync::returnUTXOFromOutpoint(transactions[i].inputs[k].prevOutHash, transactions[i].inputs[k].voutIndex, &exist);
            
            //6. Remove spent TXOs from the utxo set
            if(exist == 1) {
                ProverSync::utxoSet.erase(ProverSync::utxoSet.begin() + removeUTXO.first);
                delete removeUTXO.second;
            }
            
            //7. Prevent same-block outputs from new outputs collection
            for (uint32_t m = 0; m < newUTXOs.size(); m++) {
                if((transactions[i].inputs[k].prevOutHash == newUTXOs[m]->prevHash) && (transactions[i].inputs[k].voutIndex == newUTXOs[m]->vout)){
                    newUTXOs.erase(newUTXOs.begin() + m);
                    newLeaves.erase(newLeaves.begin() + m);
                    m -= 1;
                }
            }
        }
        }
    }
    
    //8. Push new outputs collection to the UTXO set
    for (int i = 0; i < newUTXOs.size(); i++) {
        ProverSync::utxoSet.push_back(newUTXOs[i]);
    }

    //9. Update RAM forest
    ProverSync::full.Modify(ProverSync::undo, newLeaves, this->proof.GetTargets());

    //10. Increment prover height
    ProverSync::proverHeight++;
    
    std::cout << "utxo set size: " << ProverSync::utxoSet.size() << std::endl;
}

std::pair<uint32_t, UTXO*> ProverSync::returnUTXOFromOutpoint(valtype prevHash, uint32_t vout, int *ex) {
    std::pair<uint32_t, UTXO*> returnPair;
    for(uint32_t i = 0; i < ProverSync::utxoSet.size(); i++) {
        if((ProverSync::utxoSet[i]->prevHash == prevHash) && (ProverSync::utxoSet[i]->vout == vout)) {
            *ex = 1;
            returnPair.first = i;
            returnPair.second = ProverSync::utxoSet[i];
            return returnPair;
        }
    }
    return returnPair;
}

void Prover::setCompactSpendingsRaw() {
    if(this->spendings.size() > 0) {
    valtype returnValtype;
    valtype numUTXOs = WizData::prefixCompactSizeCast((uint32_t)this->spendings.size());
    returnValtype.insert(returnValtype.begin(), numUTXOs.begin(), numUTXOs.end());
    
    for(int i = 0; i < spendings.size(); i++) {
        valtype UTXOfield;
        valtype UTXOScriptPubkey = spendings[i]->scriptPubkey;
        valtype scriptPubkeyLen = WizData::prefixCompactSizeCast((uint32_t)(UTXOScriptPubkey.size()));
        
        valtype UTXOHeight = *WizData::Uint32ToLE(spendings[i]->height);
        UTXOfield.insert(UTXOfield.end(), UTXOHeight.begin(), UTXOHeight.end());
        
        valtype UTXOValue = *WizData::Uint64ToLE(spendings[i]->value);
        UTXOfield.insert(UTXOfield.end(), UTXOValue.begin(), UTXOValue.end());
        
        UTXOfield.insert(UTXOfield.end(), scriptPubkeyLen.begin(), scriptPubkeyLen.end());
        UTXOfield.insert(UTXOfield.end(), UTXOScriptPubkey.begin(), UTXOScriptPubkey.end());
        
        returnValtype.insert(returnValtype.end(), UTXOfield.begin(), UTXOfield.end());
    }
    this->compactSpendingsRaw = returnValtype;
    }
}

valtype Prover::readSpendingsRaw() {
    return compactSpendingsRaw;
}

utreexo::BatchProof Prover::readProof() {
    return this->proof;
}
