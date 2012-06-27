//__constant int xmax = 1282;
//__constant int ymax = 1110;
//__kernel void run(__global const int *in, __global const int *in2, __global int *out){
{
	int xi, yi, subpixel;
	size_t tid = get_global_id(0);
	xi = tid%xmax;
	yi = tid/xmax;
	/*
	subpixel=xi+yi*xmax;
	out[subpixel] = in[subpixel]+in2[subpixel];
	out[subpixel] /= 2;
	
	subpixel=ymax*xmax + xi+yi*xmax;
	out[subpixel] = in[subpixel]+in2[subpixel];
	out[subpixel] /= 2;
	
	subpixel=2*ymax*xmax + xi+yi*xmax;
	out[subpixel] = in[subpixel]+in2[subpixel];
	out[subpixel] /= 2;
	*/
	/*
	subpixel=xi+yi*xmax;
	out[subpixel] = in[subpixel]+in2[subpixel];
	out[subpixel] /= 2;
	
	subpixel=ymax*xmax + xi+yi*xmax;
	out[subpixel] = in[subpixel]+in2[subpixel];
	out[subpixel] /= 2;
	
	subpixel=2*ymax*xmax + xi+yi*xmax;
	out[subpixel] = in[subpixel]+in2[subpixel];
	out[subpixel] /= 2;
	*/
	/*
	for(subpixel = xi+yi*xmax; 2*ymax*xmax + xi+yi*xmax>=subpixel; subpixel+=ymax*xmax) {
	//for(subpixel = 2*ymax*xmax + xi+yi*xmax; subpixel>=xi+yi*xmax; subpixel-=ymax*xmax) {
		out[subpixel] = mul * in[subpixel];
		out[subpixel] = out[subpixel]>255 ?255 :(0>out[subpixel] ?0 :out[subpixel]);
	}
	*/
}
