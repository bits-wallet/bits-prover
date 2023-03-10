//
//  wizdata.cpp
//  Bits
//
//  Created by Burak on 1.12.2022.
//

#include "wizdata.h"

valtype WizData::splitValtypeSet(valtype *in, int startIndex, int size){
    valtype returnValType;
    if(size > 0)
        returnValType.insert(returnValType.begin(), in->begin() + startIndex, in->begin() + startIndex + size);
    return returnValType;
}

valtype WizData::bufferAnySizeToValtype(unsigned char *buffer, int size){
    valtype val;
    for (int i = 0; i < size; i++) {
        val.push_back(buffer[i]);
    }
    return val;
}

valtype WizData::buffer80ToValtype(unsigned char *buffer){
    valtype val;
    for (int i = 0; i < 80; i++) {
        val.push_back(buffer[i]);
    }
    return val;
}

valtype WizData::buffer32ToValtype(unsigned char *buffer){
    valtype val;
    for (int i = 0; i < 32; i++) {
        val.push_back(buffer[i]);
    }
    return val;
}

uint8_t *WizData::LEtoUint8(valtype in){
    uint8_t *val = new uint8_t(0);
    *val = (int)in[0];
    return val;
}

uint16_t *WizData::LEtoUint16(valtype in){
    uint16_t *val = new uint16_t(0);
    for (int i = 0; i < in.size(); i++) { *val+= (uint16_t)in[i] * pow(256,i); }
    return val;
}

uint32_t *WizData::LEtoUint32(valtype in){
    uint32_t *val = new uint32_t(0);
    for (int i = 0; i < in.size(); i++) { *val+= (uint32_t)in[i] * pow(256,i); }
    return val;
}

uint64_t *WizData::LEtoUint64(valtype in){
    uint64_t *val = new uint64_t(0);
    for (int i = 0; i < in.size(); i++) { *val+= (uint64_t)in[i] * pow(256, i); }
    return val;
}

valtype *WizData::Uint16ToLE(uint16_t in) {
    unsigned char *ptr = new unsigned char;
    WriteLE16(ptr,in);

    valtype *returnVal = new valtype;
    returnVal->push_back(ptr[0]);
    returnVal->push_back(ptr[1]);
    
    delete ptr;
    return returnVal;
}

valtype *WizData::Uint32ToLE(uint32_t in) {
    unsigned char *ptr = new unsigned char;
    WriteLE32(ptr,in);

    valtype *returnVal = new valtype;
    returnVal->push_back(ptr[0]);
    returnVal->push_back(ptr[1]);
    returnVal->push_back(ptr[2]);
    returnVal->push_back(ptr[3]);
    
    delete ptr;
    return returnVal;
}

valtype *WizData::Uint64ToLE(uint64_t in) {
    unsigned char *ptr = new unsigned char;
    WriteLE64(ptr,in);

    valtype *returnVal = new valtype;
    returnVal->push_back(ptr[0]);
    returnVal->push_back(ptr[1]);
    returnVal->push_back(ptr[2]);
    returnVal->push_back(ptr[3]);
    returnVal->push_back(ptr[4]);
    returnVal->push_back(ptr[5]);
    returnVal->push_back(ptr[6]);
    returnVal->push_back(ptr[7]);
    
    delete ptr;
    return returnVal;
}

valtype WizData::hexStringToValtype(std::string in){
    
    std::stringstream ss;

    for (int i=0; i < in.size()/2; i++) {

        std::stringstream icat;
        icat << in[i*2] << in[i*2+1];
        std::string icatstr = icat.str();
        unsigned char hexNumber;
        sscanf(icatstr.c_str(), "%x", &hexNumber);
        ss << hexNumber;
    }
    
    const unsigned char* t = reinterpret_cast<const unsigned char*>(ss.str().c_str());
    valtype returnVal;
    
    for (int i=0; i < in.size()/2; i++){
        returnVal.push_back(*(t+i));
    }
    
    return  returnVal;
}

valtype WizData::prefixCompactSizeCast(uint32_t size) {
    valtype returnValtype;
    if (size < 253) {
        returnValtype.push_back((unsigned char)size);
    }
    else if (size <= 65535){
        returnValtype.push_back(0xFD);
        valtype val = *WizData::Uint16ToLE((uint16_t)size);
        returnValtype.insert(returnValtype.end(), val.begin(), val.end());
    }
    else if (size <= 4294967295){
        returnValtype.push_back(0xFE);
        valtype val = *WizData::Uint32ToLE(size);
        returnValtype.insert(returnValtype.end(), val.begin(), val.end());
    }
    else {
        returnValtype.push_back(0xFF);
        valtype val = *WizData::Uint64ToLE(size);
        returnValtype.insert(returnValtype.end(), val.begin(), val.end());
    }
    return returnValtype;
}
