//
// Created by Swee Balachandran on 3/14/18.
//

#ifndef PLEXILSRC_SERIALIZE_H
#define PLEXILSRC_SERIALIZE_H

#include "cfs-data-format.hh"
#include <string.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
static char* serializeBool(bool arrayelement,const bool o,char* b);
static char* serializeBoolArray(int size,const bool val[],char* buffer);
static char* serializeInt(bool arrayelement,const int32_t val,char* buffer);
static char* serializeIntArray(int size,const int32_t val[],char* buffer);
static char* serializeReal(bool arrayelement,const double val,char* buffer);
static char* serializeRealArray(int size,const double val[],char* buffer);
static char* serializeString(int size,const char val[],char* buffer);

static const char* deSerializeBool(bool arrayelement,bool* val,const char* buffer);
static const char* deSerializeBoolArray(bool val[],const char* buffer);
static const char* deSerializeInt(bool arrayelement,int32_t* val,const char* buffer);
static const char* deSerializeIntArray(int32_t val[],const char* buffer);
static const char* deSerializeReal(bool arrayelement,double* val,const char* buffer);
static const char* deSerializeRealArray(double val[],const char* buffer);
static const char* deSerializeString(char val[],const char* buffer);

static char *serializeBoolVector(int size,const bool o[], char *b);
static char const *deserializeBoolVector(int size,bool o[], const char *b);

#ifdef __cplusplus
}
#endif

static char* serializeBool(bool arrayelement,const bool o,char* b){
    arrayelement?0:(*b++ = _BOOLEAN_TYPE_);
    *b++ = (char) o;
    return b;
}

static char* serializeInt(bool arrayelement,const int32_t val,char* b){

    int32_t o = val;
    arrayelement?0:(*b++ = _INTEGER_TYPE_);
    // Store in big-endian format
    *b++ = (char) (0xFF & (o >> 24));
    *b++ = (char) (0xFF & (o >> 16));
    *b++ = (char) (0xFF & (o >> 8));
    *b++ = (char) (0xFF & o);
    return b;
}

static char* serializeReal(bool arrayelement,const double val,char* b){
    union realInt{
        double r;
        uint64_t l;
    };
    union realInt data;
    data.r = val;
    data.l = data.l;
    arrayelement?0:(*b++ = _REAL_TYPE_);
    // Store in big-endian format
    *b++ = (char) (0xFF & (data.l >> 56));
    *b++ = (char) (0xFF & (data.l >> 48));
    *b++ = (char) (0xFF & (data.l >> 40));
    *b++ = (char) (0xFF & (data.l >> 32));
    *b++ = (char) (0xFF & (data.l >> 24));
    *b++ = (char) (0xFF & (data.l >> 16));
    *b++ = (char) (0xFF & (data.l >> 8));
    *b++ = (char) (0xFF & data.l);
    return b;
}

static char* serializeString(int size,const char val[],char* b){

    unsigned long s = size;
    if (s > 0xFFFFFF)
        return NULL; // too big

    *b++ = _STRING_TYPE_;
    // Put 3 bytes of size first - std::string may contain embedded NUL
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);
    memcpy(b, val, s);
    return b + s;
}

static char* serializeBoolArray(int size,const bool val[],char* b){
    unsigned long s = size;
    if (s > 0xFFFFFF)
        return NULL; // too big to serialize

    // Write type code
    *b++ = (char) _BOOLEAN_ARRAY_TYPE_;

    // Write 3 bytes of size
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);

    //TODO: This should be a malloc;
    bool known[20];
    memset(known,1,20);

    // Write known vector
    b = serializeBoolVector(s,known, b);

    b = serializeBoolVector(s,val,b);

    return b;
}

static const char* deSerializeBool(bool arrayelement,bool* o,const char* b){
    if (!arrayelement && _BOOLEAN_TYPE_ != (dataType_t) *b++)
        return NULL;
    *o = (bool) *b++;
    return b;
}

static const char* deSerializeInt(bool arrayelement,int32_t* val,const char* b){
    if (!arrayelement && _INTEGER_TYPE_ != (dataType_t) *b++)
        return NULL;
    uint32_t n = ((uint32_t) (unsigned char) *b++) << 8;
    n = (n + (uint32_t) (unsigned char) *b++) << 8;
    n = (n + (uint32_t) (unsigned char) *b++) << 8;
    n = (n + (uint32_t) (unsigned char) *b++);

    *val = (int32_t) n;
    return b;
}

static const char* deSerializeReal(bool arrayelement,double* val,const char* b){
    if (!arrayelement && _REAL_TYPE_ != (dataType_t) *b++)
        return NULL;
    union realInt{
        double r;
        uint64_t l;
    };
    union realInt data;
    data.l = (uint64_t) (unsigned char) *b++;  data.l = data.l << 8;
    data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
    data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
    data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
    data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
    data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
    data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
    data.l += (uint64_t) (unsigned char) *b++;
    *val = data.r;
    return b;

}

static const char* deSerializeString(char val[],const char* b){
    if (_STRING_TYPE_ != (dataType_t) *b++)
        return NULL;

    // Get 3 bytes of size
    size_t s = ((size_t) (unsigned char) *b++) << 8;
    s = (s + (size_t) (unsigned char) *b++) << 8;
    s = s + (size_t) (unsigned char) *b++;

    memcpy(val,b,s);
    return b + s;
}

static char* serializeIntArray(int size,const int32_t val[],char* b){
    unsigned long s = size;
    if (s > 0xFFFFFF)
        return NULL; // too big to serialize

    // Write type code
    *b++ = (char) _INTEGER_ARRAY_TYPE_;

    // Write 3 bytes of size
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);

    //TODO: This should be a malloc;
    bool known[20];
    memset(known,1,20);

    // Write known vector
    b = serializeBoolVector(s,known, b);

    // Write array contents
    for (size_t i = 0; i < s; ++i) {
        b = serializeInt(true,val[i], b);
        if (!b)
            return NULL; // serializeElement failed
    }
    return b;
}

static char* serializeRealArray(int size,const double val[],char* b){
    unsigned long s = size;
    if (s > 0xFFFFFF)
        return NULL; // too big to serialize

    // Write type code
    *b++ = (char) _REAL_ARRAY_TYPE_;

    // Write 3 bytes of size
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);

    //TODO: This should be a malloc;
    bool known[20];
    memset(known,1,20);

    // Write known vector
    b = serializeBoolVector(s,known, b);

    // Write array contents
    for (size_t i = 0; i < s; ++i) {
        b = serializeReal(true,val[i], b);
        if (!b)
            return NULL; // serializeElement failed
    }
    return b;
}

static char const *deSerializeBoolArray(bool val[],const char* b)
{
    // Check type code
    if (_BOOLEAN_ARRAY_TYPE_ !=  *b++)
        return NULL; // not an appropriate array

    // Get 3 bytes of size
    size_t s = (size_t) *b++; s = s << 8;
    s += (size_t) *b++; s = s << 8;
    s += (size_t) *b++;

    //TODO: This should be a malloc;
    bool known[20];
    memset(known,1,20);

    b = deserializeBoolVector(s,known, b);
    b = deserializeBoolVector(s,val,b);

    return b;
}

static char const *deSerializeIntArray(int32_t val[],const char* b)
{
    // Check type code
    if (_INTEGER_ARRAY_TYPE_ !=  *b++)
        return NULL; // not an appropriate array

    // Get 3 bytes of size
    size_t s = (size_t) *b++; s = s << 8;
    s += (size_t) *b++; s = s << 8;
    s += (size_t) *b++;

    //TODO: This should be a malloc;
    bool known[20];
    memset(known,1,20);

    b = deserializeBoolVector(s,known, b);
    for (size_t i = 0; i < s; ++i)
        b = deSerializeInt(true,val+i, b);

    return b;
}

static char const *deSerializeRealArray(double val[],const char* b)
{
    // Check type code
    if (_REAL_ARRAY_TYPE_ !=  *b++)
        return NULL; // not an appropriate array

    // Get 3 bytes of size
    size_t s = (size_t) *b++; s = s << 8;
    s += (size_t) *b++; s = s << 8;
    s += (size_t) *b++;

    //TODO: This should be a malloc;
    bool known[20];
    memset(known,1,20);

    b = deserializeBoolVector(s,known, b);
    for (size_t i = 0; i < s; ++i)
        b = deSerializeReal(true,val+i, b);

    return b;
}


static char *serializeBoolVector(int size,const bool o[], char *b)
{
    int s = size;
    int i = 0;
    while (s > 0) {
        uint8_t tmp = 0;
        uint8_t mask = 0x80;
        switch (s) {
            default: // s >= 8
                if (o[i++])
                    tmp |= mask;
                mask = mask >> 1;

            case 7:
                if (o[i++])
                    tmp |= mask;
                mask = mask >> 1;

            case 6:
                if (o[i++])
                    tmp |= mask;
                mask = mask >> 1;

            case 5:
                if (o[i++])
                    tmp |= mask;
                mask = mask >> 1;

            case 4:
                if (o[i++])
                    tmp |= mask;
                mask = mask >> 1;

            case 3:
                if (o[i++])
                    tmp |= mask;
                mask = mask >> 1;

            case 2:
                if (o[i++])
                    tmp |= mask;
                mask = mask >> 1;

            case 1:
                if (o[i++])
                    tmp |= mask;
                break;
        }

        *b++ = tmp;
        s -= 8;
    }

    return b;
}

// Internal function
// Read from buffer in big-endian form
// Presumes vector size has already been set.
static char const *deserializeBoolVector(int size,bool o[], const char *b)
{
    int s = size;
    int i = 0;
    while (s > 0) {
        uint8_t tmp = *b++;
        uint8_t mask = 0x80;
        switch (s) {
            default: // s >= 8
                o[i++] = (tmp & mask) ? true : false;
                mask = mask >> 1;

            case 7:
                o[i++] = (tmp & mask) ? true : false;
                mask = mask >> 1;

            case 6:
                o[i++] = (tmp & mask) ? true : false;
                mask = mask >> 1;

            case 5:
                o[i++] = (tmp & mask) ? true : false;
                mask = mask >> 1;

            case 4:
                o[i++] = (tmp & mask) ? true : false;
                mask = mask >> 1;

            case 3:
                o[i++] = (tmp & mask) ? true : false;
                mask = mask >> 1;

            case 2:
                o[i++] = (tmp & mask) ? true : false;
                mask = mask >> 1;

            case 1:
                o[i++] = (tmp & mask) ? true : false;
                break;
        }
        s -= 8;
    }
    return b;
}

#endif //PLEXILSRC_SERIALIZE_H
