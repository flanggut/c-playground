__kernel void matrixMul( __global float* C, __global float* A, __global float* B, int widthA, int heightA, int widthB)
{
	float result = 0.0f;
	for( int i = 0; i < widthA; ++i) {
		result += A[get_global_id(1) * widthA + i] * B[i * widthB + get_global_id(0)];
	}
	C[get_global_id(1) * widthB + get_global_id(0)] = result;
    return;
}