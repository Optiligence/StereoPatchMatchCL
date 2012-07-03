//__constant int xmax = 1282;
//__constant int ymax = 1110;
//__constant int patchsize = 5;
//__kernel void run(__global const int *in, __global const int *in2, __global int *out, __global int *xNNF, __global int *yNNF, __global const int *offset)
{
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
		for(int row = patchsize; row > 0; --row) {
			for(int i = patchsize; i > 0; --i, ++subpixel, ++subpixel2) {
				dif += abs(in[subpixel] - in2[subpixel2]);//R
				int blue_subpixel = subpixel + pixelCount, blue_subpixel2 = subpixel2 + pixelCount;
				dif += abs(in[blue_subpixel] - in2[blue_subpixel2]);//G
				dif += abs(in[blue_subpixel + pixelCount] - in2[blue_subpixel2 + pixelCount]);//B
			}
			//plus one row, minus patch width
			subpixel += xmax - patchsize;
			subpixel2 += xmax - patchsize;
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
		for(int row = patchsize; row > 0; --row) {
			for(int i = patchsize; i > 0; --i, ++subpixel, ++subpixel2) {
				dif += abs(in[subpixel] - in2[subpixel2]);//R
				int blue_subpixel = subpixel + pixelCount, blue_subpixel2 = subpixel2 + pixelCount;
				dif += abs(in[blue_subpixel] - in2[blue_subpixel2]);//G
				dif += abs(in[blue_subpixel + pixelCount] - in2[blue_subpixel2 + pixelCount]);//B
			}
			//plus one row, minus patch width
			subpixel += xmax - patchsize;
			subpixel2 += xmax - patchsize;
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
		for(int row = patchsize; row > 0; --row) {
			for(int i = patchsize; i > 0; --i, ++subpixel, ++subpixel2) {
				dif += abs(in[subpixel] - in2[subpixel2]);//R
				int blue_subpixel = subpixel + pixelCount, blue_subpixel2 = subpixel2 + pixelCount;
				dif += abs(in[blue_subpixel] - in2[blue_subpixel2]);//G
				dif += abs(in[blue_subpixel + pixelCount] - in2[blue_subpixel2 + pixelCount]);//B
			}
			//plus one row, minus patch width
			subpixel += xmax - patchsize;
			subpixel2 += xmax - patchsize;
		}
	}
	if(bestDif > dif) {//better patch match – switch patches
		bestDif = dif;
		int oriPatch = patchcenter - xmax;
		xNNF[oriPatch] = xNNF[patchcenter];
		yNNF[oriPatch] = yNNF[patchcenter];
	}
}