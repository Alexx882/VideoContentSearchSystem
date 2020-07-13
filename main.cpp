#include "main1.h"
#include "main2.h"
#include "main3.h"

int main(int argc, char** argv)
{
	const char* videos[10][3] = {
		{ "", "videos\\v3c1\\00492.mp4", "keyframes\\v3c1\\00492" },
		{ "", "videos\\v3c1\\01729.mp4", "keyframes\\v3c1\\01729" },
		{ "", "videos\\v3c1\\01736.mp4", "keyframes\\v3c1\\01736" },
		{ "", "videos\\v3c1\\03847.mp4", "keyframes\\v3c1\\03847" },
		{ "", "videos\\v3c1\\05814.mp4", "keyframes\\v3c1\\05814" },
		{ "", "videos\\v3c1\\05979.mp4", "keyframes\\v3c1\\05979" },
		{ "", "videos\\v3c1\\06385.mp4", "keyframes\\v3c1\\06385" },
		{ "", "videos\\v3c1\\07031.mp4", "keyframes\\v3c1\\07031" },
		{ "", "videos\\v3c1\\07037.mp4", "keyframes\\v3c1\\07037" },
		{ "", "videos\\v3c1\\07187.mp4", "keyframes\\v3c1\\07187" }
	};

	for (int i = 0; i < 10; ++i)
		mainShotDetection(3, videos[i]);

	return 0;	
}