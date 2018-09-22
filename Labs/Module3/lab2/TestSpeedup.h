// Define some macros to help control the speed of the tests
// These tests can take a long time if a full test is launched
// Use these to quickly test spots, or do a full test 
// Coffee break recommended for the full test ;)

//Use the increment macros directly,
//Or use INC and define FULL, BALANCED, or FAST 
// to have the macro defined for you.

// In order from slowest, but highest granularity...
// to fastest, but lowest granularity...
#define BYTE_INC(ptr) 	(ptr++)
#define WORD_INC(ptr) 	(ptr+=4)
#define HUGE_INC(ptr)	(ptr+=1000)


#if defined(FULL_TEST)
	#define INC(ptr) BYTE_INC(ptr)
#elif defined(BALANCED_TEST)
	#define INC(ptr) WORD_INC(ptr)
#elif defined(FAST_TEST)
	#define INC(ptr) HUGE_INC(ptr)
#else
	#define INC(ptr) WORD_INC(ptr)
#endif

#if defined(_DEBUG)
	#define DLOGADDR(ptr) (Serial.println((uintptr_t)addr, HEX);)
#else
	#define DLOGADDR(ptr) 
#endif