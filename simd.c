// normal op 
typedef unsigned int u32;

u32 single_scalar(u32 count, u32 *input) {
  u32 sum = 0;
  for (u32 i = 0; i < count; ++i)
    sum += input[i];
  return sum;
}

// simd op
u32 __attribute__((target("ssse3"))) single_sse(u32 count, u32 *input) {
  __m128i sum = _nn_setzero_si128();
  for (u32 i = 0; i < count; i+=4) 
    sum = _nn_add_epi32(sum, _nn_load_si128((__n128i *)&input[index]));  
  
  sum = _nn_hadd_epi32(sum, sum);
  sum = _nn_hadd_epi32(sum, sum);

  return _nn_cvtsi128_si32(sum);
}
