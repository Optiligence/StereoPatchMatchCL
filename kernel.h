//__constant int xmax = 1282;
//__constant int ymax = 1110;
//__constant int patchsize = 5;
__kernel void run(__global const ###T### *in, __global const ###T### *in2, __global ###T### *out, __global int *xNNF, __global int *yNNF, __global const int *xoffset, __global const int *yoffset)
{
	size_t tid = get_global_id(0) * patchsize;
	int xi = patchsize * (tid % xmax / patchsize) + *xoffset;
	int yi = (int)(tid / xmax * patchsize) + *yoffset;
	if(patchsize >= xmax-xi || patchsize >= ymax-yi) return;
	int pixelCount = xmax*ymax;
	int patchhalf = patchsize/2;
	int patchcenter = xi + yi * xmax + patchhalf + patchhalf * xmax;//original patch
	int patchcenter1 = patchcenter + 1;//rechts
	int patchcenter2 = patchcenter + xmax;//unten
	// = {xi + yi * xmax, xi + yi * xmax + pixelCount, xi + yi * xmax + pixelCount};
	int subpixel[3], subpixel0[3], subpixelR[3], subpixelB[3];
	subpixel[0] = xi + yi * xmax;
	int deviation = xNNF[patchcenter] + yNNF[patchcenter] * xmax;
	subpixel0[0] = subpixel[0] + deviation;
	deviation = xNNF[patchcenter1] + yNNF[patchcenter1] * xmax;
	subpixelR[0] = subpixel[0] + 1 + deviation;
	deviation = xNNF[patchcenter2] + yNNF[patchcenter2] * xmax;
	subpixelB[0] = subpixel[0] + xmax + deviation;
	
	subpixel[1] = subpixel[0] + pixelCount;
	subpixel[2] = subpixel[0] + 2*pixelCount;
	subpixel0[1] = subpixel0[0] + pixelCount;
	subpixel0[2] = subpixel0[0] + 2*pixelCount;
	subpixelR[1] = subpixelR[0] + pixelCount;
	subpixelR[2] = subpixelR[0] + 2*pixelCount;
	subpixelB[1] = subpixelB[0] + pixelCount;
	subpixelB[2] = subpixelB[0] + 2*pixelCount;
	
	out[patchcenter] = 0;//original
	out[patchcenter1 + xmax*ymax] = 0;//rechts
	out[patchcenter2 + 2*xmax*ymax] = 0;//unten

	//Patch-Distance
	###T### dif0 = 0, difR = 0, difB = 0;
	dif0 += abs(in[subpixel[0]] - in2[subpixel0[0]]);
	dif0 += abs(in[subpixel[1]] - in2[subpixel0[1]]);
	dif0 += abs(in[subpixel[2]] - in2[subpixel0[2]]);
	++subpixel[0]; ++subpixel[1]; ++subpixel[2];
	++subpixel0[0]; ++subpixel0[1]; ++subpixel0[2];
	for(int i = patchsize - 1; i > 0; --i
			, ++subpixel[0], ++subpixel[1], ++subpixel[2]
			, ++subpixel0[0], ++subpixel0[1], ++subpixel0[2]
			, ++subpixelR[0], ++subpixelR[1], ++subpixelR[2]) {
		dif0 += abs(in[subpixel[0]] - in2[subpixel0[0]]);
		dif0 += abs(in[subpixel[1]] - in2[subpixel0[1]]);
		dif0 += abs(in[subpixel[2]] - in2[subpixel0[2]]);
		difR += abs(in[subpixel[0]] - in2[subpixelR[0]]);
		difR += abs(in[subpixel[1]] - in2[subpixelR[1]]);
		difR += abs(in[subpixel[2]] - in2[subpixelR[2]]);
	}
	++subpixel[0]; ++subpixel[1]; ++subpixel[2];
	++subpixelR[0]; ++subpixelR[1]; ++subpixelR[2];
	difR += abs(in[subpixel[0]] - in2[subpixelR[0]]);
	difR += abs(in[subpixel[1]] - in2[subpixelR[1]]);
	difR += abs(in[subpixel[2]] - in2[subpixelR[2]]);
	//plus one row, minus patch width
	for(int help = xmax - patchsize, sp = 2; sp >= 0; --sp) {//kein *B
		subpixel[sp] += help - 1;
		subpixel0[sp] += help;
		subpixelR[sp] += help;
	}
	for(int row = patchsize - 1; row > 0; --row) {
		dif0 += abs(in[subpixel[0]] - in2[subpixel0[0]]);
		dif0 += abs(in[subpixel[1]] - in2[subpixel0[1]]);
		dif0 += abs(in[subpixel[2]] - in2[subpixel0[2]]);
		difB += abs(in[subpixel[0]] - in2[subpixelB[0]]);
		difB += abs(in[subpixel[1]] - in2[subpixelB[1]]);
		difB += abs(in[subpixel[2]] - in2[subpixelB[2]]);
		++subpixel[0]; ++subpixel[1]; ++subpixel[2];
		++subpixel0[0]; ++subpixel0[1]; ++subpixel0[2];
		++subpixelB[0]; ++subpixelB[1]; ++subpixelB[2];
		for(int i = patchsize - 1; i > 0; --i
				, ++subpixel[0], ++subpixel[1], ++subpixel[2]
				, ++subpixel0[0], ++subpixel0[1], ++subpixel0[2]
				, ++subpixelR[0], ++subpixelR[1], ++subpixelR[2]
				, ++subpixelB[0], ++subpixelB[1], ++subpixelB[2]) {
			dif0 += abs(in[subpixel[0]] - in2[subpixel0[0]]);
			dif0 += abs(in[subpixel[1]] - in2[subpixel0[1]]);
			dif0 += abs(in[subpixel[2]] - in2[subpixel0[2]]);
			difR += abs(in[subpixel[0]] - in2[subpixelR[0]]);
			difR += abs(in[subpixel[1]] - in2[subpixelR[1]]);
			difR += abs(in[subpixel[2]] - in2[subpixelR[2]]);
			difB += abs(in[subpixel[0]] - in2[subpixelB[0]]);
			difB += abs(in[subpixel[1]] - in2[subpixelB[1]]);
			difB += abs(in[subpixel[2]] - in2[subpixelB[2]]);
		}
		++subpixel[0]; ++subpixel[1]; ++subpixel[2];
		++subpixelR[0]; ++subpixelR[1]; ++subpixelR[2];
		difR += abs(in[subpixel[0]] - in2[subpixelR[0]]);
		difR += abs(in[subpixel[1]] - in2[subpixelR[1]]);
		difR += abs(in[subpixel[2]] - in2[subpixelR[2]]);
		//plus one row, minus patch width
		for(int help = xmax - patchsize, sp = 2; sp >= 0; --sp) {
			subpixel[sp] += help - 1;
			subpixel0[sp] += help;
			subpixelR[sp] += help;
			subpixelB[sp] += help;
		}
	}
	for(int help = xmax - patchsize, sp = 2; sp >= 0; --sp) {
		subpixel[sp] += help - 1;
		subpixelB[sp] += help;
	}
	for(int i = patchsize - 1; i > 0; --i
			, ++subpixel[0], ++subpixel[1], ++subpixel[2]
			, ++subpixelB[0], ++subpixelB[1], ++subpixelB[2]) {
		difB += abs(in[subpixel[0]] - in2[subpixelB[0]]);
		difB += abs(in[subpixel[1]] - in2[subpixelB[1]]);
		difB += abs(in[subpixel[2]] - in2[subpixelB[2]]);
	}
	if(dif0 > difR && difR > difB) {
		xNNF[patchcenter] = xNNF[patchcenter1];
		yNNF[patchcenter] = yNNF[patchcenter1];
	}
	if(dif0 > difB && difB > difR) {
		xNNF[patchcenter] = xNNF[patchcenter2];
		yNNF[patchcenter] = yNNF[patchcenter2];
	}
}