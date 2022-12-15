//
//  main.cpp
//  Bits
//
//  Created by Burak on 15.12.2022.
//
#include <chrono>
#include "transaction/transaction.h"
#include "block/block.h"
#include "prover/prover.h"
#include "utxo/proof.h"

using namespace std::chrono;

valtype stringToValtype(std::string const& hex) {
    
    std::string newStr = "";
    
    for (int i = 0; i < (hex.size()/2); i++) {
        newStr += hex.substr((i*2),2) + " ";
    }
    
    std::string cipher = newStr;
    
    
    std::istringstream strm{cipher};
    strm >> std::hex;

    return {std::istream_iterator<int>{strm}, {}};
}

std::string getLastHeight() {
    
    char buf[32];
    std::string query = "curl -H  \"Content-Type: application/json\" -X GET  http://mempool-env.eba-qh6r3f7d.us-east-2.elasticbeanstalk.com/lastblock";
    
    FILE * output = popen(query.c_str(), "r");
    
    while (fgets (buf, 32, output)) {}
    
    pclose(output);
    return buf;
  }

std::string getRawBlock(int height) {
    
    char buf[8000000];
    std::string query = "curl -H  \"Content-Type: application/json\" -X GET  http://mempool-env.eba-qh6r3f7d.us-east-2.elasticbeanstalk.com/block/"+ std::to_string(height) ;
    
    FILE * output = popen(query.c_str(), "r");
    
    while (fgets (buf, 8000000, output)) {}
    
    pclose(output);
    return buf;
  }

int main() {
        
    new ProverSync();
    
    for (int l = 1; l < 2147483647; l++) {
        
        std::string lastHeightStr = getLastHeight();
        int lastHeight = std::stoi(lastHeightStr.substr(1,lastHeightStr.size()-2));
        std::cout << "lastHeight: " << lastHeight << std::endl;
        
        if(l >= lastHeight){
            break;
        }
        
        std::string aa = getRawBlock(l);
        valtype blockRaw = stringToValtype(aa.substr(1,aa.size()-2));
        Prover *proverBlock = new Prover(blockRaw);

        std::vector<uint8_t> proofBlock;
        proverBlock->readProof().Serialize(proofBlock);
        proverBlock->readProof().Print();
  
        std::cout << "proofBlock: " << proofBlock.size() << std::endl;
        
        //std::to_string(l)
        std::string p_str = std::to_string(l) + "_p.bin";
        
        char pn[p_str.size()];
        for (int m = 0; m < p_str.size(); m++) {
            pn[m] = p_str[m];
        }
        FILE * pFile;
        
        char proofBuffer[proofBlock.size()];
                
        for (int z = 0; z < proofBlock.size(); z++) {
            proofBuffer[z] = proofBlock[z];
        }
        
        pFile = fopen ( pn , "w+b" );
        fwrite (proofBuffer , 1 , sizeof(proofBuffer) , pFile );
        fclose (pFile);
        
        
        valtype spendingsBlock = proverBlock->readSpendingsRaw();
        std::cout << "spendingsBlock: " << spendingsBlock.size() << std::endl;
        
        //std::to_string(l)
        std::string s_str = std::to_string(l) + "_s.bin";
        char fn[s_str.size()];
        for (int m = 0; m < s_str.size(); m++) {
            fn[m] = s_str[m];
        }
        FILE * sFile;
        
        char spendingsBuffer[spendingsBlock.size()];
                
        for (int z = 0; z < spendingsBlock.size(); z++) {
            spendingsBuffer[z] = spendingsBlock[z];
        }
        
        sFile = fopen ( fn , "w+b" );
        fwrite (spendingsBuffer , 1 , sizeof(spendingsBuffer) , sFile );
        fclose (sFile);
        
        
        
        
        
        
        delete proverBlock;
        
        
    
        
        
        

        std::cout << "__________PROVER_BLOCK_HEIGHT_" << std::to_string(l) << "_SYNCED_SIZE_" << std::to_string(blockRaw.size()) << "_________"<< std::endl;
        
       
        
    }

    
    
    std::string s;
    std::cin >> s;
    
return 0;
}
