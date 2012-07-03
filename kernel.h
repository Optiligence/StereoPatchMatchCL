//__constant int xmax = 1282;
//__constant int ymax = 1110;
//__constant int patchsize = 5;
__kernel void run(__global const ###T### *in, __global const ###T### *in2, __global ###T### *out, __global int *xNNF, __global int *yNNF, __global const int *offset)
{
	size_t tid = get_global_id(0) * patchsize;
	int xi = patchsize * (tid % xmax / patchsize);
	int yi = tid / xmax * patchsize;
	if(patchsize > xmax-1-xi || patchsize > ymax-1-yi) return;
	int pixelCount = xmax*ymax;
	int patchhalf = patchsize/2;
	int patchcenter = xi + yi * xmax + patchhalf + patchhalf * xmax;
	// = {xi + yi * xmax, xi + yi * xmax + pixelCount, xi + yi * xmax + pixelCount};
	int subpixel[3], subpixel0[3], subpixelR[3], subpixelB[3];
	subpixel[0] = xi + yi * xmax;
	subpixel[1] = subpixel[0] + pixelCount;
	subpixel[2] = subpixel[1] + pixelCount;
	int deviation = (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;
	subpixel0[0] = subpixel[0] + deviation;
	subpixel0[1] = subpixel[1] + deviation;
	subpixel0[2] = subpixel[2] + deviation;
	++patchcenter;
	deviation = (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;
	subpixelR[0] = subpixel[0] + 1 + deviation;
	subpixelR[1] = subpixel[0] + pixelCount;
	subpixelR[2] = subpixel[1] + pixelCount;
	out[patchcenter] = 0; out[patchcenter + xmax*ymax] = 255; out[patchcenter + 2*xmax*ymax] = 0;//rechts
	--patchcenter;
	out[patchcenter] = 255; out[patchcenter + xmax*ymax] = 0; out[patchcenter + 2*xmax*ymax] = 0;//original
	patchcenter += xmax;
	out[patchcenter] = 0; out[patchcenter + xmax*ymax] = 0; out[patchcenter + 2*xmax*ymax] = 255;//unten
	deviation = (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;
	subpixelB[0] = subpixel[0] + xmax + deviation;
	subpixelB[1] = subpixel[0] + pixelCount;
	subpixelB[2] = subpixel[1] + pixelCount;

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
		int patchcenter = xi + yi * xmax + patchhalf + patchhalf * xmax;
		int help = patchcenter + 1;
		xNNF[patchcenter] = xNNF[help];
		yNNF[patchcenter] = yNNF[help];
	}
	if(dif0 > difB && difB > difR) {
		int patchcenter = xi + yi * xmax + patchhalf + patchhalf * xmax;
		int help = patchcenter + xmax;
		xNNF[patchcenter] = xNNF[help];
		yNNF[patchcenter] = yNNF[help];
	}
}
	/*
	int xi, yi, patchcenter, patchhalf, dif, bestDif, pixelCount;
	size_t tid = get_global_id(0) * patchsize;
	xi = patchsize * (tid % xmax / patchsize);
	yi = tid / xmax * patchsize;
	pixelCount = xmax*ymax;
	patchhalf = patchsize/2;
	if(patchsize > xmax-1-xi || patchsize > ymax-1-yi)
		return;
	{
		int subpixel = xi + yi * xmax;
		patchcenter = subpixel + patchhalf + patchhalf * xmax;
		int subpixel2 = subpixel + (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;//x und y verschiebung
		//Patch-Distance
		dif=0;
		for(int sp = 3; sp > 0; --sp) {
			for(int row = 0; row < patchsize; row++) {
				for(int i = 0; i < patchsize; i++, ++subpixel, ++subpixel2)
					dif += abs(in[subpixel] - in2[subpixel2]);//R
				//plus one row, minus patch width
				subpixel += xmax - patchsize;
				subpixel2 += xmax - patchsize;
			}
			subpixel += pixelCount - patchsize * xmax;
			subpixel2 += pixelCount - patchsize * xmax;
		}
	}
	bestDif = dif;//inital difference
	//eins nach rechts
	++xi;
	++patchcenter;
	{
		int subpixel = xi + yi * xmax;
		int subpixel2 = subpixel + (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;//x und y verschiebung
		//Patch-Distance
		dif=0;
		for(int sp = 3; sp > 0; --sp) {
			for(int row = 0; row < patchsize; row++) {
				for(int i = 0; i < patchsize; i++, ++subpixel, ++subpixel2)
					dif += abs(in[subpixel] - in2[subpixel2]);//R
				//plus one row, minus patch width
				subpixel += xmax - patchsize;
				subpixel2 += xmax - patchsize;
			}
			subpixel += pixelCount - patchsize * xmax;
			subpixel2 += pixelCount - patchsize * xmax;
		}
	}
	if(bestDif > dif) {//better patch match – switch patches
		bestDif = dif;
		int oriPatch = patchcenter - 1;
		xNNF[oriPatch] = xNNF[patchcenter];
		yNNF[oriPatch] = yNNF[patchcenter];
	}
	out[patchcenter] = 0; out[patchcenter + xmax*ymax] = 255; out[patchcenter + 2*xmax*ymax] = 0;//rechts
	--xi;//original x
	--patchcenter;//original patch
	out[patchcenter] = 255; out[patchcenter + xmax*ymax] = 0; out[patchcenter + 2*xmax*ymax] = 0;//original
	//eins nach unten
	++yi;
	patchcenter += xmax;
	out[patchcenter] = 0; out[patchcenter + xmax*ymax] = 0; out[patchcenter + 2*xmax*ymax] = 255;//unten
	{
		int subpixel = xi + yi * xmax;
		int subpixel2 = subpixel + (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;//x und y verschiebung
		//Patch-Distance
		dif=0;
		for(int sp = 3; sp > 0; --sp) {
			for(int row = 0; row < patchsize; row++) {
				for(int i = 0; i < patchsize; i++, ++subpixel, ++subpixel2)
					dif += abs(in[subpixel] - in2[subpixel2]);//R
				//plus one row, minus patch width
				subpixel += xmax - patchsize;
				subpixel2 += xmax - patchsize;
			}
			subpixel += pixelCount - patchsize * xmax;
			subpixel2 += pixelCount - patchsize * xmax;
		}
	}
	if(bestDif > dif) {//better patch match – switch patches
		bestDif = dif;
		int oriPatch = patchcenter - xmax;
		xNNF[oriPatch] = xNNF[patchcenter];
		yNNF[oriPatch] = yNNF[patchcenter];
	}
}
*/