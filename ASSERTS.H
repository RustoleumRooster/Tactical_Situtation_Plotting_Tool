#ifndef _ASSERTS_H_
#define _ASSERTS_H_

void ___ASSERT(const char* error, const char* file, int line);

#define ASSERT(e) ( (e) ? (void)0 : ___ASSERT(#e,__FILE__,__LINE__) )




#endif /* _ASSERTS_H_ */
