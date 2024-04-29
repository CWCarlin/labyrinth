#include "math/vector.h"

#include <math.h>
#include <stdio.h>

int main() {
	LbrVector3 a = {1, 0, 0};
	LbrQuaternion q;
	LbrVector3 c;
	LbrVector3 v = {0, 1, 0};
	lbrVector3Normalize(&v);

	lbrQuaternionAxisAngle(&q, &v, M_PI / 3);
	lbrVector3QuatRot(&a, &q, &c);

	printf("%f %f %f\n", c.x, c.y, c.z);

	printf("%f\n", lbrVector3Norm(&c));

	return 0;
}
