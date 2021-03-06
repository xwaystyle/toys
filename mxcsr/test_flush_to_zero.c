#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mxcsr.c"
#include "time.c"

#if !defined(DUMP_MXCSR)
//#   define DUMP_MXCSR
#endif

void mulps_in_loop();

void mulps_without_flush_to_zero();
void mulps_with_flush_to_zero();

int main() {
	mulps_without_flush_to_zero();
	mulps_with_flush_to_zero();
}

void mulps_without_flush_to_zero() {
	uint32_t t1, t2;

	printf("multiply small values with masked precision... ");
	fflush(stdout);

	t1 = get_time();
	mulps_in_loop();
	t2 = get_time();

	printf("%0.3fs\n", (t2-t1)/1000000.0);

#if defined(DUMP_MXCSR)
	print_mxcsr_exception_flags(stdout, get_mxcsr());
#endif
}

void mulps_with_flush_to_zero() {
	uint32_t mxcsr;
	uint32_t t1, t2;

	mxcsr = get_mxcsr();
	mxcsr |= FLUSH_TO_ZERO;
	mxcsr &= RESET_ERROR_FLAGS;
	set_mxcsr(mxcsr);

	printf("multiply small values with flush to zero flag set... ");
	fflush(stdout);

	t1 = get_time();
	mulps_in_loop();
	t2 = get_time();

	printf("%0.3fs\n", (t2-t1)/1000000.0);

#if defined(DUMP_MXCSR)
	print_mxcsr_exception_flags(stdout, get_mxcsr());
#endif
}


#define packed_float(x) {(x), (x), (x), (x)}

float min_floats[4] = packed_float(FLT_MIN);

void mulps_in_loop() {
	const int32_t iterations = 10000000;
	uint32_t dummy;

	__asm__ __volatile__(
		"movups 	%2, %%xmm0\n" // min_floats
		"1:\n"
		"movaps 	%%xmm0, %%xmm1\n"
		"mulps 		%%xmm1, %%xmm1\n"
		"loop 		1b\n"
		: "=c" (dummy)
		: "c"  (iterations)
		, "m"  (min_floats)
	);
}
