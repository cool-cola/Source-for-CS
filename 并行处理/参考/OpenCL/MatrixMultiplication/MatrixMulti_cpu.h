#ifndef CPU_MATRIX_MULTIPLICATION_H
#define CPU_MATRIX_MULTIPLICATION_H

template <typename Datatype>
void matrixMultiplication(Datatype* vC, const Datatype* vA, const Datatype* vB, unsigned int vhA, unsigned int vwA, unsigned int vwB)
{
	for (unsigned int i = 0; i < vhA; ++i)
		for (unsigned int j = 0; j < vwB; ++j) {
			Datatype sum = 0;
			for (unsigned int k = 0; k < vwA; ++k) {
				Datatype a = vA[i * vwA + k];
				Datatype b = vB[k * vwB + j];
				sum += a * b;
			}
			vC[i * wB + j] = sum;
		}
}


#endif