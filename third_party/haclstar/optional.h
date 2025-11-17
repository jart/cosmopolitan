#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_OPTIONAL_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_OPTIONAL_H_
COSMOPOLITAN_C_START_

#define Hacl_Streaming_Types_None 0
#define Hacl_Streaming_Types_Some 1

typedef uint8_t Hacl_Streaming_Types_optional;

typedef struct Hacl_Streaming_Types_optional_32_s
{
    Hacl_Streaming_Types_optional tag;
    uint32_t* v;
} Hacl_Streaming_Types_optional_32;

typedef struct Hacl_Streaming_Types_optional_64_s
{
    Hacl_Streaming_Types_optional tag;
    uint64_t* v;
} Hacl_Streaming_Types_optional_64;

typedef struct FStar_Pervasives_Native_option___uint8_t__s
{
    Hacl_Streaming_Types_optional tag;
    uint8_t* v;
} FStar_Pervasives_Native_option___uint8_t_;

typedef struct Hacl_Streaming_Types_two_pointers_s
{
    uint64_t* fst;
    uint64_t* snd;
} Hacl_Streaming_Types_two_pointers;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_OPTIONAL_H_ */
