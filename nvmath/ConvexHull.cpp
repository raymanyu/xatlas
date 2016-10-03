// This code is in the public domain -- Ignacio Casta�o <castano@gmail.com>

#include <vector>
#include "ConvexHull.h"

#include "Vector.h"

#include "nvcore/RadixSort.h"
#include "nvcore/Array.h"

using namespace nv;

inline static float triangleArea(Vector2::Arg v1, Vector2::Arg v2, Vector2::Arg v3)
{
	return 0.5f * (v3.x * v1.y + v1.x * v2.y + v2.x * v3.y - v2.x * v1.y - v3.x * v2.y - v1.x * v3.y);
}


// Compute the convex hull using Graham Scan.
void nv::convexHull(const Array<Vector2> &input, Array<Vector2> &output, float epsilon/*=0*/)
{
	const uint inputCount = input.count();
	Array<float> coords;
	coords.resize(inputCount);
	for (uint i = 0; i < inputCount; i++) {
		coords[i] = input[i].x;
	}
	RadixSort radix;
	radix.sort(coords);
	const uint *ranks = radix.ranks();
	std::vector<Vector2> top;
	top.reserve(inputCount);
	std::vector<Vector2> bottom;
	bottom.reserve(inputCount);
	Vector2 P = input[ranks[0]];
	Vector2 Q = input[ranks[inputCount - 1]];
	float topy = max(P.y, Q.y);
	float boty = min(P.y, Q.y);
	for (uint i = 0; i < inputCount; i++) {
		Vector2 p = input[ranks[i]];
		if (p.y >= boty) top.push_back(p);
	}
	for (uint i = 0; i < inputCount; i++) {
		Vector2 p = input[ranks[inputCount - 1 - i]];
		if (p.y <= topy) bottom.push_back(p);
	}
	// Filter top list.
	output.clear();
	output.append(top[0]);
	output.append(top[1]);
	for (uint i = 2; i < top.size(); ) {
		Vector2 a = output[output.count() - 2];
		Vector2 b = output[output.count() - 1];
		Vector2 c = top[i];
		float area = triangleArea(a, b, c);
		if (area >= -epsilon) {
			output.popBack();
		}
		if (area < -epsilon || output.count() == 1) {
			output.append(c);
			i++;
		}
	}
	uint top_count = output.count();
	output.append(bottom[1]);
	// Filter bottom list.
	for (uint i = 2; i < bottom.size(); ) {
		Vector2 a = output[output.count() - 2];
		Vector2 b = output[output.count() - 1];
		Vector2 c = bottom[i];
		float area = triangleArea(a, b, c);
		if (area >= -epsilon) {
			output.popBack();
		}
		if (area < -epsilon || output.count() == top_count) {
			output.append(c);
			i++;
		}
	}
	// Remove duplicate element.
	nvDebugCheck(output.front() == output.back());
	output.popBack();
}


