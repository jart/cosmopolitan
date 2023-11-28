#ifndef COSMOPOLITAN_NET_FINGER_FINGER_H_
#define COSMOPOLITAN_NET_FINGER_FINGER_H_
COSMOPOLITAN_C_START_

char *DescribeSyn(char *, size_t, const char *, size_t);
const char *GetOsName(int);
int GetSynFingerOs(uint32_t);
uint32_t FingerSyn(const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NET_FINGER_FINGER_H_ */
