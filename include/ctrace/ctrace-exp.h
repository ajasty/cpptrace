
#ifndef __CTRACE_EXP_H__
#define __CTRACE_EXP_H__

#define DLLEXPORT __attribute__((visibility("default")))

DLLEXPORT void initTrace(uint64_t);
DLLEXPORT void trace(void);
DLLEXPORT void endTrace(void);

#endif

