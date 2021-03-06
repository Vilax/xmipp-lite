/***************************************************************************
 *
 * Authors:     Jose Luis Vilas (joseluis.vilas-prieto@yale.edu)
 *                             or (jlvilas@cnb.csic.es)
 *              Hemant. D. Tagare (hemant.tagare@yale.edu)
 *
 * Yale University, New Haven, Connecticut, United States of America
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 *  All comments concerning this program package may be sent to the
 *  e-mail address 'xmipp@cnb.csic.es'
 ***************************************************************************/

#include "resolution_fso.h"
#include <chrono> 

//#define SAVE_DIR_FSC

void ProgFSO::defineParams()
{
	addUsageLine("Calculate global resolution anisotropy - OFSC curve - via directional FSC measurements.");
	addUsageLine("If a set of particle is given, the contribution of the particle distribution to the resolution is also analyzed");
	addUsageLine("Reference: J.L. Vilas, H.D. Tagare, XXXXX (2020)");
	addUsageLine("+ ");
	addUsageLine("+* Directional Fourier Shell Correlation (FSC)", true);
	addUsageLine("+ This program may be used to estimate the directional FSC between two half maps.");
	addUsageLine("+ The directionality is measured by means of conical-like filters in Fourier Space. To avoid possible Gibbs effects ");
	addUsageLine("+ the filters are gaussian functions with their respective maxima along the filtering direction. A set of 321 directions ");
	addUsageLine("+ is used to cover the projection sphere, computing for each direction the directional FSC at 0.143 between the two half maps.");
	addUsageLine("+ The result is a set of 321 FSC curves. From then a 3DFSC is obtained by interpolation. Note that as well as it occurs with");
	addUsageLine("+ global FSC, the directional FSC is mask dependent.");
	addUsageLine(" ");
	addUsageLine("+* Occupancy Fourier Shell Curve (OFSC)", true);
	addUsageLine("+ The Occupancy Fourier Shell Curve can be obtained from the set of directional FSC curves estimated before.");
	addUsageLine("+ To do that, the two half maps are used to determine the Global FSC at threshold 0.143. Then, the ratio between the number");
	addUsageLine("+ of directions with resolution higher (better) than the Global resolution and the total number of measured directions is");
	addUsageLine("+ calculated at different frequencies (resolutions). Note that this ratio is between 0 (all directions presents worse)");
	addUsageLine("+ resolution than the global FSC)  and 1 (all directions present better resolution than the FSC) at a given resolution.");
	addUsageLine("+ In the particular case for which the OFSC curve takes the value of 0.5, then half of the directions are better, and.");
	addUsageLine("+ the other half are worse than the FSC. Therefore, the OFCS curve at 0.5 should be the FSC value. Note that a map is ");
	addUsageLine("+ isotropic if all directional resolution are similar, and anisotropic is there are significant resolution values along");
	addUsageLine("+ different directions. Thus, when the OFSC present a sharp cliff, it means step-like function the map will be isotropic.");
	addUsageLine("+ In contrast, when the OFSC shows a slope the map will be anisotropic. The lesser slope the higher resolution isotropy.");
	addUsageLine("+ ");
	addUsageLine("+* Particle contribution to the resolution", true);
	addUsageLine("+ If a set of particle is provided, the algorithm will determine the contribution of each particle to the directional");
	addUsageLine("+ resolution and it's effect in the resolution anisotropy. It means to determine if the directional resolution is ");
	addUsageLine("+ explained by particles. If not, then probably your set of particle contains empty particles (noise), the reconstruction");
	addUsageLine("+ presents heterogeneity or flexibility, in that the heterogeneity should be solved and the map reconstructed again.");
	addUsageLine(" ");
	addUsageLine(" ");
	addSeeAlsoLine("resolution_fsc");

	addParamsLine("   --half1 <input_file>               : Input Half map 1");
	addParamsLine("   --half2 <input_file>               : Input Half map 2");
	addParamsLine("   --fscfolder <output_file=\"\">     : Output folder where the directional FSC results (metadata file) will be stored.");
	addParamsLine("   [--anisotropy <output_file=\"\">]  : Anisotropy file name.");

	addParamsLine("   [--sampling <Ts=1>]                : (Optical) Pixel size (Angstrom). If it is not provided by default will be 1 A/px.");
	addParamsLine("   [--mask <input_file=\"\">]         : (Optional) Smooth mask to remove noise.");
	addParamsLine("   [--particles <input_file=\"\">]    : (Optional) Set of Particles used for reconstructing");
	addParamsLine("   [--anglecone <ang_con=-1>]               : (Optional) Angle Cone (angle axis-generatrix) for estimating the directional FSC");
	addParamsLine("   [--threedfsc <output_file=\"\">]   : (Optional) The 3D FSC map is obtained.");
	addParamsLine("   [--test]                           : (Optional) It executes an unitary test");
	addParamsLine("   [--threshold <ang_con=-1>]		 : (Optional) Threshold for cross validation");
	addParamsLine("   [--threads <Nthreads=1>]		     : (Optional) Number of threads to be used");



	addExampleLine("Resolution of two half maps half1.mrc and half2.mrc with a sampling rate of 2 A/px", false);
	addExampleLine("xmipp_resolution_fso --half1 half1.mrc  --half2 half2.mrc --sampling_rate 2 ");
	addExampleLine("Resolution of two half maps half1.mrc and half2.mrc with a sampling rate of 2 A/px and a mask mask.mrc", false);
	addExampleLine("xmipp_resolution_fso --half1 half1.mrc  --half2 half2.mrc --mask mask.mrc --sampling_rate 2 ");
	addExampleLine("Resolution of a set of images using 5.6 pixel size (in Angstrom):", false);
	addExampleLine("xmipp_resolution_fso --half1 half1.mrc  --half2 half2.mrc --sampling_rate 2");
}

void ProgFSO::readParams()
{
	sampling = getDoubleParam("--sampling");

	fnhalf1 = getParam("--half1");
	fnhalf2 = getParam("--half2");
	fnParticles = getParam("--particles");
	fnmask = getParam("--mask");
	ang_con = getDoubleParam("--anglecone");
	fn_3dfsc = getParam("--threedfsc");
	fn_fscmd_folder = getParam("--fscfolder");
	fn_ani = getParam("--anisotropy");
	test = checkParam("--test");
	thrs = getDoubleParam("--threshold");
	Nthreads = getDoubleParam("--threads");
}

void ProgFSO::defineFrequencies(const MultidimArray< std::complex<double> > &myfftV,
		const MultidimArray<double> &inputVol)
{
	double u;

	freq_fourier_z.initZeros(ZSIZE(myfftV));
	freq_fourier_x.initZeros(XSIZE(myfftV));
	freq_fourier_y.initZeros(YSIZE(myfftV));

	VEC_ELEM(freq_fourier_z,0) = 1e-38;
	for(size_t k=1; k<ZSIZE(myfftV); ++k){
		FFT_IDX2DIGFREQ(k,ZSIZE(inputVol), u);
		VEC_ELEM(freq_fourier_z,k) = u;
	}

	VEC_ELEM(freq_fourier_y,0) = 1e-38;
	for(size_t k=1; k<YSIZE(myfftV); ++k){
		FFT_IDX2DIGFREQ(k,YSIZE(inputVol), u);
		VEC_ELEM(freq_fourier_y,k) = u;
	}

	VEC_ELEM(freq_fourier_x,0) = 1e-38;
	for(size_t k=1; k<XSIZE(myfftV); ++k){
		FFT_IDX2DIGFREQ(k,XSIZE(inputVol), u);
		VEC_ELEM(freq_fourier_x,k) = u;
	}

	freqMap.resizeNoCopy(myfftV);
	freqMap.initConstant(1.9);  //Nyquist is 2, we take 1.9 greater than Nyquist

	xvoldim = XSIZE(inputVol);
	yvoldim = YSIZE(inputVol);
	zvoldim = ZSIZE(inputVol);
	freqElems.initZeros(xvoldim/2+1);

	double uz, uy, ux, uz2, u2, uz2y2;
	long n=0;
	int idx = 0;
	Ncomps=0;
	
	//SPHERE FREQUENCY REFERENCE
	MultidimArray<double> sphere;
	sphere.resizeNoCopy(freqMap);

	//TODO: check in the for enter in the if conditions

	for(size_t k=0; k<ZSIZE(myfftV); ++k)
	{
		uz = VEC_ELEM(freq_fourier_z,k);
		uz2 = uz*uz;
		for(size_t i=0; i<YSIZE(myfftV); ++i)
		{
			uy = VEC_ELEM(freq_fourier_y, i);
			uz2y2 = uz2 + uy*uy;

			for(size_t j=0; j<XSIZE(myfftV); ++j)
			{
				ux = VEC_ELEM(freq_fourier_x, j);
				ux = sqrt(uz2y2 + ux*ux);

				if	(ux<=0.5)
				{
					idx = (int) round(ux * xvoldim);
					++Ncomps;
					DIRECT_MULTIDIM_ELEM(freqElems, idx) += 1;
					DIRECT_MULTIDIM_ELEM(sphere,n) = -ux;

					if ((k != 0) || (i != 0) || (j != 0))
					{
						DIRECT_MULTIDIM_ELEM(freqMap,n) = 1/ux;
					}
					else
					{
						DIRECT_MULTIDIM_ELEM(freqMap,n) = 1e38;
					}


					if ((j == 0) && (uy<0))
					{
						DIRECT_MULTIDIM_ELEM(freqMap,n) = 1.9;
						DIRECT_MULTIDIM_ELEM(freqElems,idx) -= 1;
						--Ncomps;
					}
						
					if ((i == 0) && (j == 0) && (uz<0))
					{
						DIRECT_MULTIDIM_ELEM(freqMap,n) = 1.9;
						DIRECT_MULTIDIM_ELEM(freqElems,idx) -= 1;
						--Ncomps;
					}
						
				}				
				++n;
			}
		}
	}
	

	FileName fn;
	fn = fn_fscmd_folder+"sphere.mrc";
	createFullFourier(sphere, fn, xvoldim, yvoldim, zvoldim);
}


void ProgFSO::fscDir(MultidimArray< std::complex< double > > & FT1,
				MultidimArray< std::complex< double > > & FT2,
				double sampling_rate,
				Matrix1D<double> &freq_fourier_x,
				Matrix1D<double> &freq_fourier_y,
				Matrix1D<double> &freq_fourier_z,
				MultidimArray< double >& freqMap,
				MultidimArray< double >& freq,
				MultidimArray< double >& frc,
				double maxFreq, int m1sizeX, int m1sizeY, int m1sizeZ,
				double rot, double tilt, double ang_con, double &dres, double &thrs)
{
	MultidimArray< int > radial_count(m1sizeX/2+1);
	MultidimArray<double> num, den1, den2, testMap, numSize;
//        testMap.initZeros(freqMap);

	num.initZeros(radial_count);
	den1.initZeros(radial_count);
	den2.initZeros(radial_count);
	numSize.initZeros(radial_count);

	freq.initZeros(radial_count);
	frc.initZeros(radial_count);

	int ZdimFT1=(int)ZSIZE(FT1);
	int YdimFT1=(int)YSIZE(FT1);
	int XdimFT1=(int)XSIZE(FT1);

	double x_dir, y_dir, z_dir, uz, uy, ux, cosAngle, aux;
	x_dir = sin(tilt*PI/180)*cos(rot*PI/180);
	y_dir = sin(tilt*PI/180)*sin(rot*PI/180);
	z_dir = cos(tilt*PI/180);
	cosAngle = cos(ang_con);
	aux = 4.0/((cos(ang_con) -1)*(cos(ang_con) -1));
	long n = 0;
	double wt = 0;
	double count = 0;
	for (int k=0; k<ZdimFT1; k++)
	{
		double uz = VEC_ELEM(freq_fourier_z,k);
		uz *= z_dir;
		for (int i=0; i<YdimFT1; i++)
		{
			double uy = VEC_ELEM(freq_fourier_y,i);
			uy *= y_dir;
			for (int j=0; j<XdimFT1; j++)
			{
				double ux = VEC_ELEM(freq_fourier_x,j);
//                	if (ux < 0.000001)
//                    {
//                		DIRECT_MULTIDIM_ELEM(testMap,n) = imag(DIRECT_MULTIDIM_ELEM(FT1,n));
//                    }
				ux *= x_dir;
				double iun = DIRECT_MULTIDIM_ELEM(freqMap,n);
				double f = 1/iun;
				iun *= (ux + uy + uz);

				double cosine = fabs(iun);
				++n;

				if (cosine>=cosAngle)
					{
						if (f>maxFreq)
							continue;

						int idx = (int) round(f * m1sizeX);
						cosine = sqrt(exp( -((cosine -1)*(cosine -1))*aux ));
						wt += cosine;

						std::complex<double> &z1 = dAkij(FT1, k, i, j);
						std::complex<double> &z2 = dAkij(FT2, k, i, j);
						double absz1 = abs(z1*cosine);
						double absz2 = abs(z2*cosine);
						dAi(num,idx) += real(conj(z1) * z2 * cosine * cosine);
						dAi(den1,idx) += absz1*absz1;
						dAi(den2,idx) += absz2*absz2;
						dAi(numSize,idx) += 1.0;
					}
			}
		}
	}


	FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
	{
		dAi(freq,i) = (float) i / (m1sizeX * sampling_rate);
		dAi(frc,i) = dAi(num,i)/sqrt(dAi(den1,i)*dAi(den2,i));
	}
	dAi(frc,0) = 1; dAi(frc,1) = 1; dAi(frc,2) = 1; dAi(frc,3) = 1;

	FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
	{
			if ( (dAi(frc,i)<=thrs) && (i>2) )
			{
			double y2, y1, x2, x1, slope, ny;
			y2 = dAi(freq,i);
			y1 = dAi(freq,i-1);
			x2 = dAi(frc,i);
			x1 = dAi(frc,i-1);

			slope = (y2 - y1)/(x2 - x1);
			ny = y2 - slope*x2;

			dres = 1/(slope*thrs + ny);
			break;
			}
		}
}

void ProgFSO::arrangeFSC_and_fscGlobal(double sampling_rate,
				double &fscFreq, double &thrs, double &resInterp, MultidimArray<double> &freq)
	{

		// Determining the cumulative number of frequencies per shell number, cumpos
		// First shell has 0 elements
		// second shell has the number of elements of the first shell
		// Third shell has the number of elements of the first+sencond shells and so on
		size_t dim = NZYXSIZE(freqElems);

		cumpos.resizeNoCopy(dim);

		DIRECT_MULTIDIM_ELEM(cumpos,0) = 0;
		for (long n = 1; n<NZYXSIZE(cumpos); ++n)
		{
			DIRECT_MULTIDIM_ELEM(cumpos,n) = DIRECT_MULTIDIM_ELEM(cumpos,n-1) + DIRECT_MULTIDIM_ELEM(freqElems,n-1);
		}
 		
		// FT1_vec, and FT2_vec will storage the Fourier coeffs of the half maps
		// The storage will be in order, first elements of the vector are the freq, next freq 1 and so on
		// The position of each component is also stored in the vectors idx_x, idx_y and idx_z
		FT1_vec.initZeros(Ncomps);
		FT2_vec = FT1_vec;
		// threeD_FSC.resizeNoCopy(FT1_vec);
		// normalizationMap.resizeNoCopy(FT1_vec);
		// threeD_FSC.initZeros();
		// normalizationMap.initZeros();

		MultidimArray<long> pos;
		MultidimArray<double> num, den1, den2;
		num.initZeros(freqElems);
		pos.resizeNoCopy(num);
		freqidx.resizeNoCopy(FT1_vec);
		arr2indx.resizeNoCopy(FT1_vec);
		arr2indx.initZeros();
		freqidx.initZeros();
		pos.initZeros();
		den1 = num;
		den2 = num;


		int ZdimFT1=(int)ZSIZE(FT1);
		int YdimFT1=(int)YSIZE(FT1);
		int XdimFT1=(int)XSIZE(FT1);

		
		fx.resizeNoCopy(FT1_vec);
		fy.resizeNoCopy(FT1_vec);
		fz.resizeNoCopy(FT1_vec);

		long n = 0;
		for (int k=0; k<ZdimFT1; k++)
		{
			double uz = VEC_ELEM(freq_fourier_z, k);
			for (int i=0; i<YdimFT1; i++)
			{
				double uy = VEC_ELEM(freq_fourier_y, i);
				for (int j=0; j<XdimFT1; j++)
				{
					double ux = VEC_ELEM(freq_fourier_x, j);
					
					double iun = DIRECT_MULTIDIM_ELEM(freqMap,n);
					double f = 1/iun;
					++n;

					if (f>0.5)
						continue;
					
					int idx = (int) round(f * xvoldim);
					
					int idx_count = DIRECT_MULTIDIM_ELEM(cumpos, idx) + DIRECT_MULTIDIM_ELEM(pos, idx); 

					DIRECT_MULTIDIM_ELEM(arr2indx, idx_count) = n - 1;

					DIRECT_MULTIDIM_ELEM(fx, idx_count) = ux*iun;
					DIRECT_MULTIDIM_ELEM(fy, idx_count) = uy*iun;
					DIRECT_MULTIDIM_ELEM(fz, idx_count) = uz*iun;


					DIRECT_MULTIDIM_ELEM(freqidx, idx_count) = idx;

					std::complex<double> &z1 = dAkij(FT1, k, i, j);
					std::complex<double> &z2 = dAkij(FT2, k, i, j);

					DIRECT_MULTIDIM_ELEM(FT1_vec, idx_count) = z1;
					DIRECT_MULTIDIM_ELEM(FT2_vec, idx_count) = z2;
					double absz1 = abs(z1);
					double absz2 = abs(z2);
					dAi(num,idx) += real(conj(z1) * z2);
					dAi(den1,idx) += absz1*absz1;
					dAi(den2,idx) += absz2*absz2;

					DIRECT_MULTIDIM_ELEM(pos, idx) +=1;
				}
			}
		}
		
		// The global FSC is stored as a metadata
		size_t id;
		MetaData mdRes;
		MultidimArray< double > frc;
		freq.initZeros(freqElems);
		frc.initZeros(freqElems);
		FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
		{
			dAi(frc,i) = dAi(num,i)/sqrt(dAi(den1,i)*dAi(den2,i));
			dAi(freq,i) = (float) i / (xvoldim * sampling_rate);

			if (i>0)
			{
				id=mdRes.addObject();
				mdRes.setValue(MDL_RESOLUTION_FREQ,dAi(freq, i),id);
				mdRes.setValue(MDL_RESOLUTION_FRC,dAi(frc, i),id);
				mdRes.setValue(MDL_RESOLUTION_FREQREAL, 1./dAi(freq, i), id);
			}
		}
		mdRes.write(fn_fscmd_folder+"GlobalFSC.xmd");

		std::cout << "    " << std::endl;
		
		// Here the FSC at 0.143 is obtained byt interpolating
		FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
		{
			if ( (dAi(frc,i)<=thrs) && (i>2) )
			{
			double y2, y1, x2, x1, slope, ny;
			y2 = dAi(freq,i);
			y1 = dAi(freq,i-1);
			x2 = dAi(frc,i);
			x1 = dAi(frc,i-1);

			slope = (y2 - y1)/(x2 - x1);
			ny = y2 - slope*x2;

			resInterp = 1/(slope*thrs + ny);
			fscFreq = 1.0/dAi(freq, i);
			break;
			}
		}
		std::cout << "Resolution " << fscFreq << " " << resInterp << std::endl;

	}


void ProgFSO::fscDir_fast(MultidimArray<double> &fsc, double rot, double tilt,
				MetaData &mdRes, MultidimArray<double> &threeD_FSC, MultidimArray<double> &normalizationMap,
				double &fscFreq, double &thrs, double &resol, size_t dirnumber)
{
	size_t dim = NZYXSIZE(freqElems);
	//MultidimArray<long> pos;
	
	
	MultidimArray<double> num, den1, den2;	
	num.initZeros(dim);
	den1.initZeros(dim);
	den2.initZeros(dim);
	//pos.resizeNoCopy(num);
	//pos.initZeros();
	std::vector<long> vecidx;
	std::vector<double> weightFSC3D;


	//Parameter to determine the cone
	double x_dir, y_dir, z_dir, cosAngle, aux;
	x_dir = sin(tilt)*cos(rot);
	y_dir = sin(tilt)*sin(rot);
	z_dir = cos(tilt);

	cosAngle = cos(ang_con);

	// It is multiply by 0.5 because later the weight is
	// cosine = sqrt(exp( -((cosine -1)*(cosine -1))*aux )); 
	// thus the computation of the weight is speeded up
	// aux = 4.0/((cos(ang_con) -1)*(cos(ang_con) -1));
	aux = (4.0/((cos(ang_con) -1)*(cos(ang_con) -1)))*0.5;
	double wt = 0;
	

	// Computing directional resolution
	FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(FT1_vec)
	{
		// TODO: Remove unuseful vectors
		// double cosine = fabs((x_dir*ux + y_dir*uy + z_dir*uz)/sqrt(ux*ux + uy*uy + uz*uz));
		double ux = DIRECT_MULTIDIM_ELEM(fx, n);
		double uy = DIRECT_MULTIDIM_ELEM(fy, n);
		double uz = DIRECT_MULTIDIM_ELEM(fz, n);

		double cosine = fabs(x_dir*ux + y_dir*uy + z_dir*uz);

		//std::cout << ux << " " << uy << " " << uz << " " << cosine << std::endl;
		
		if (cosine >= cosAngle)	
		{
			std::complex<double> &z1 = DIRECT_MULTIDIM_ELEM(FT1_vec, n);
			std::complex<double> &z2 = DIRECT_MULTIDIM_ELEM(FT2_vec, n);
			
			// cosine =      exp( -((cosine -1)*(cosine -1))*aux );

			// in fact it is sqrt(exp( -((cosine -1)*(cosine -1))*aux )); 
			// For shake of performance The sqrt is avoided dividing the by 2 in aux
			cosine = exp( -((cosine -1)*(cosine -1))*aux); 
			//wt += cosine;

			double absz1 = abs(z1)*cosine;
			double absz2 = abs(z2)*cosine;

			vecidx.push_back(n);
			cosine *= cosine;
			weightFSC3D.push_back(cosine);

			size_t idxf = DIRECT_MULTIDIM_ELEM(freqidx, n);
			dAi(num, idxf) += real(conj(z1) * z2)  * cosine;
			dAi(den1,idxf) += absz1*absz1;
			dAi(den2,idxf) += absz2*absz2;
		}
	}

	MultidimArray< double > freq;
	MultidimArray< double > frc;
	freq.initZeros(dim);
	fsc.resizeNoCopy(freq);
	fsc.initConstant(1.0);


	//The fsc is stored in a metadata and saved
	#ifdef SAVE_DIR_FSC
	FileName fnmd;
	
	size_t id;
	FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
	{

		dAi(fsc,i) = (dAi(num,i)+1e-38)/(sqrt(dAi(den1,i)*dAi(den2,i))+1e-38);
		dAi(freq,i) = (float) i / (xvoldim * sampling);

		// std::cout << i << " " << dAi(fsc,i) << std::endl;

		if (i>0)
		{
			id=mdRes.addObject();
			mdRes.setValue(MDL_RESOLUTION_FREQ,dAi(freq, i),id);
			mdRes.setValue(MDL_RESOLUTION_FRC,dAi(fsc, i),id);
			mdRes.setValue(MDL_RESOLUTION_FREQREAL, 1./dAi(freq, i), id);
		}
	}
	fnmd = fn_fscmd_folder + formatString("fscDirection_%i.xmd", dirnumber);
	mdRes.write(fnmd);
	#endif

	//TODO: join with previous loop
	FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
	{
		if ( (dAi(fsc,i)<=thrs) && (i>2) )
		{
			double y2, y1, x2, x1, slope, ny;
			y2 = dAi(freq,i);
			y1 = dAi(freq,i-1);
			x2 = dAi(fsc,i);
			x1 = dAi(fsc,i-1);

			slope = (y2 - y1)/(x2 - x1);
			ny = y2 - slope*x2;

			resol = 1/(slope*thrs + ny);
			fscFreq = 1.0/dAi(freq, i);
			break;
		}
	}
	
	size_t sizevec = vecidx.size();
	for (size_t kk = 0; kk< sizevec; ++kk)
	{
		double w = weightFSC3D[kk];
		long n = vecidx[kk];
		size_t ind = DIRECT_MULTIDIM_ELEM(freqidx, n);
		
		DIRECT_MULTIDIM_ELEM(threeD_FSC, n) += w*DIRECT_MULTIDIM_ELEM(fsc, ind);
		DIRECT_MULTIDIM_ELEM(normalizationMap, n) += w;
	}
}


void ProgFSO::fscGlobal(double sampling_rate,
				MultidimArray< double >& freq,
				MultidimArray< double >& frc,
				double maxFreq, int m1sizeX, int m1sizeY, int m1sizeZ, MetaData &mdRes,
				double &fscFreq, double &thrs, double &resInterp)
{
	MultidimArray< int > radial_count(m1sizeX/2+1);
	MultidimArray<double> num, den1, den2;

	num.initZeros(radial_count);
	den1.initZeros(radial_count);
	den2.initZeros(radial_count);

	freq.initZeros(radial_count);
	frc.initZeros(radial_count);

	int ZdimFT1=(int)ZSIZE(FT1);
	int YdimFT1=(int)YSIZE(FT1);
	int XdimFT1=(int)XSIZE(FT1);

	long n = 0;
	for (int k=0; k<ZdimFT1; k++)
	{
		for (int i=0; i<YdimFT1; i++)
		{
			for (int j=0; j<XdimFT1; j++)
			{
				double iun = DIRECT_MULTIDIM_ELEM(freqMap,n);
				double f = 1/iun;
				++n;

				if (f>maxFreq)
					continue;

				int idx = (int) round(f * m1sizeX);
				std::complex<double> &z1 = dAkij(FT1, k, i, j);
				std::complex<double> &z2 = dAkij(FT2, k, i, j);
				double absz1 = abs(z1);
				double absz2 = abs(z2);
				dAi(num,idx) += real(conj(z1) * z2);
				dAi(den1,idx) += absz1*absz1;
				dAi(den2,idx) += absz2*absz2;
			}
		}
	}
	
	size_t id;
	FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
	{
		dAi(frc,i) = dAi(num,i)/sqrt(dAi(den1,i)*dAi(den2,i));
		dAi(freq,i) = (float) i / (m1sizeX * sampling_rate);

		if (i>0)
		{
			id=mdRes.addObject();
			mdRes.setValue(MDL_RESOLUTION_FREQ,dAi(freq, i),id);
			mdRes.setValue(MDL_RESOLUTION_FRC,dAi(frc, i),id);
			mdRes.setValue(MDL_RESOLUTION_FREQREAL, 1./dAi(freq, i), id);
		}
	}
	
	FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
	{
		if ( (dAi(frc,i)<=thrs) && (i>2) )
		{
		double y2, y1, x2, x1, slope, ny;
		y2 = dAi(freq,i);
		y1 = dAi(freq,i-1);
		x2 = dAi(frc,i);
		x1 = dAi(frc,i-1);

		slope = (y2 - y1)/(x2 - x1);
		ny = y2 - slope*x2;

		resInterp = 1/(slope*thrs + ny);
			fscFreq = 1.0/dAi(freq, i);
			break;
		}
	}
	std::cout << ZdimFT1 << std::endl;
	mdRes.write(fn_fscmd_folder+"GlobalFSC.xmd");

	std::cout << "    " << std::endl;
}



void ProgFSO::createfrequencySphere(MultidimArray<double> &sphere,
		Matrix1D<double> &freq_fourier_x,
			Matrix1D<double> &freq_fourier_y,
			Matrix1D<double> &freq_fourier_z)
{
	int ZdimFT1=(int)ZSIZE(sphere);
	int YdimFT1=(int)YSIZE(sphere);
	int XdimFT1=(int)XSIZE(sphere);

	long n = 0;
	sphere.initConstant(-0.5);
	for (int k=0; k<ZdimFT1; k++)
	{
		double uz = VEC_ELEM(freq_fourier_z,k);
		uz *= uz;
		for (int i=0; i<YdimFT1; i++)
		{
			double uy = VEC_ELEM(freq_fourier_y,i);
			uy *= uy;
			for (int j=0; j<XdimFT1; j++)
			{
				double ux = VEC_ELEM(freq_fourier_x,j);
				ux *= ux;
				ux = sqrt(ux + uy + uz);

				if (ux>0.5)
				{
					++n;
					continue;
				}
				else
					DIRECT_MULTIDIM_ELEM(sphere,n) = -ux;
				++n;
			}
		}
	}
}


void ProgFSO::crossValues(Matrix2D<double> &indexesFourier, double &rot, double &tilt, double &angCon,
			MultidimArray<std::complex<double>> &f1, MultidimArray<std::complex<double>> &f2,
			std::complex<double> &f1_mean, std::complex<double> &f2_mean)
{
	double x_dir, y_dir, z_dir, cosAngle, aux;
	double lastCosine = 0;

	x_dir = sin(tilt*PI/180)*cos(rot*PI/180);
	y_dir = sin(tilt*PI/180)*sin(rot*PI/180);
	z_dir = cos(tilt*PI/180);

	cosAngle = cos(angCon*PI/180);
	aux = 4.0/((cos(angCon*PI/180) -1)*(cos(angCon*PI/180) -1));

	double counter_ = 0;
	double wt = 0;

	long n = 0;
	FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(f1)
	{
		double cosine = x_dir*MAT_ELEM(indexesFourier, 1, n) + y_dir*MAT_ELEM(indexesFourier, 2, n) +
						z_dir*MAT_ELEM(indexesFourier, 0, n);

		cosine = fabs(cosine);

		if (cosine>=cosAngle)
		{
//        		std::cout << "wt = " << sqrt(exp( -((cosine -1)*(cosine -1))*aux )) << std::endl;
			wt += sqrt(exp( -((cosine -1)*(cosine -1))*aux ));
//        		std::cout << "wt = " << wt << std::endl;
		}
	}
	wt = 1/wt;
//        std::cout << "-----------" << std::endl;
//        std::cout << "wt" << wt << std::endl;
//        std::cout << "-----------" << std::endl;


	f1_mean = std::complex<double> (0,0);
	f2_mean = std::complex<double> (0,0);
	n = 0;
	FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(f1)
	{
		double cosine = x_dir*MAT_ELEM(indexesFourier, 1, n) + y_dir*MAT_ELEM(indexesFourier, 2, n) +
									z_dir*MAT_ELEM(indexesFourier, 0, n);

		if (cosine>=cosAngle)
		{
			cosine = sqrt(exp( -((cosine - 1)*(cosine - 1))*aux ))*wt;

			f1_mean += cosine*DIRECT_MULTIDIM_ELEM(f1,n);
			f2_mean += cosine*DIRECT_MULTIDIM_ELEM(f2,n);
			// std::cout << "+  " << DIRECT_MULTIDIM_ELEM(f1,n) << " " << DIRECT_MULTIDIM_ELEM(f2,n) << std::endl;
		}
		else
		{
			if (cosine<=(-cosAngle))
			{
				cosine = sqrt(exp( -((fabs(cosine) -1)*(fabs(cosine) -1))*aux ))*wt;

				f1_mean += cosine*conj(DIRECT_MULTIDIM_ELEM(f1,n));
				f2_mean += cosine*conj(DIRECT_MULTIDIM_ELEM(f2,n));
				// std::cout << DIRECT_MULTIDIM_ELEM(f1,n) << " " << DIRECT_MULTIDIM_ELEM(f2,n) << std::endl;
			}
		}
	}
	
}

void ProgFSO::weights(double freq, Matrix2D<double> &indexesFourier, Matrix2D<int> &indexesFourier2, double &rot, double &tilt, double &angCon,
			MultidimArray<std::complex<double>> &f1, MultidimArray<std::complex<double>> &f2,
			MultidimArray<std::complex<double>> &FT1, MultidimArray<std::complex<double>> &FT2,
			Matrix1D<double> &freq_fourier_x,
			Matrix1D<double> &freq_fourier_y,
			Matrix1D<double> &freq_fourier_z,
			double &cross)
{
	std::complex<double> f1_orig, f2_orig, f1_mean, f2_mean;
	double angCone1degree = 1.0;

	crossValues(indexesFourier, rot, tilt, angCone1degree, f1, f2, f1_orig, f2_orig);
	// shellValue(freq, rot, tilt, FT1, FT2, 
	// 			freq_fourier_x, freq_fourier_y, freq_fourier_z, 
	// 			f1_orig, f2_orig);
	//std::cout << "........" << std::endl;
	// std::cout << ">>>> " << f1_orig << " " << f2_orig << std::endl;
	crossValues(indexesFourier, rot, tilt, angCon, f1, f2, f1_mean, f2_mean);
	// std::cout << "<<<<" << f1_mean << " " << f2_mean << std::endl;

	//std::cout << rot << " " << tilt << " " << real(f1_orig) << " " << imag(f1_orig) << " " << real(f2_orig) << " " << imag(f2_orig) << " " << real(f1_mean) << " " << imag(f1_mean) << " " << real(f2_mean) <<  " " << imag(f2_mean) << ";" << std::endl;

	// std::cout << f1_orig << "     " << f2_orig << std::endl;
	// std::cout << f1_mean << "     " << f2_mean << std::endl;
	// std::cout << ">>>> " << fabs(f1_orig - f2_mean)*fabs(f1_orig - f2_mean) << " " << fabs(f2_orig - f1_mean)*fabs(f2_orig - f1_mean) << std::endl;
	cross += fabs(f1_orig - f2_mean)*fabs(f1_orig - f2_mean) +
			fabs(f2_orig - f1_mean)*fabs(f2_orig - f1_mean);

	std::cout << cross << std::endl;
}

void ProgFSO::findIndexinVector(double freq, double x_dir, 
								size_t &idx,
								Matrix1D<double> &freq_fourier)
{
	// x_dir = -0.525731;
	double xf = x_dir*freq;
    // std::cout << xf <<  std::endl;

	idx = 0;
	double u;
	
	if (xf>=0)
	{
		if (xf < 1e-38){
			xf = 1e-38;}
		for (size_t k = 1; k<freq_fourier.vdim; k++)
		{
			double u = VEC_ELEM(freq_fourier,k);
			if (xf >= u)
			{
				idx = k;
			}
			else
			{
				break;
			}
		}

		if ( fabs( VEC_ELEM(freq_fourier,idx) - xf )  > fabs( VEC_ELEM(freq_fourier,idx+1) - xf ) )
		{
			idx = idx + 1;
			// std::cout << "*" << VEC_ELEM(freq_fourier,idx) << " " << xf << " " << VEC_ELEM(freq_fourier,idx+1) << std::endl;
		}
	}
	else
	{
		// std::cout << "entro " <<  std::endl;
		for (size_t k = (freq_fourier.vdim -1); k>=1; --k)
		{
			double u = VEC_ELEM(freq_fourier,k);
			// std::cout << xf << " " << u << std::endl;
			if (xf <= u)
			{
				idx = k;
			}
			else
			{
				break;
			}
		}

		if ( fabs( VEC_ELEM(freq_fourier,idx) - xf )  > fabs( VEC_ELEM(freq_fourier,idx-1) - xf ) )
		{
			std::cout << "*" << VEC_ELEM(freq_fourier,idx) << " " << xf << " " << VEC_ELEM(freq_fourier,idx-1) << std::endl;
			idx = idx - 1;
		}
		else
		{
			std::cout << "*" << VEC_ELEM(freq_fourier,idx) << " " << xf << " " << VEC_ELEM(freq_fourier,idx-1) << std::endl;
		}
		
	}
	

	// else
	// {
	// 	std::cout << " " << VEC_ELEM(freq_fourier,idx) << " " << xf << " " << VEC_ELEM(freq_fourier,idx+1) << std::endl;
	// }

	
	// for (size_t k = 1; k<freq_fourier.vdim; k++){
	// 	double u = VEC_ELEM(freq_fourier,k);
	// 	std::cout << xf << " " << u << std::endl;
	// 	if (xf>=u)
	// 	{
	// 		std::cout << "entro" << std::endl;
	// 		if (xf*VEC_ELEM(freq_fourier,k-1)<=xf*u){
	// 			idx  = k;}
	// 		else{
	// 			idx  = k-1;
	// 		}
	// 		break;
	// 	}
	// 	else
	// 	{
	// 		if (k == 1){
	// 			idx = k -1;
	// 			break;}
	// 		else{
	// 			idx = k - 1;
	// 			break;
	// 		}
	// 	}
		
	// }
	std::cout << "idx = " << idx << std::endl;


}

// void ProgFSO::shellValue(Matrix2D<double> &indexesFourier, double &rot, double &tilt,
// 			MultidimArray<std::complex<double>> &f1, MultidimArray<std::complex<double>> &f2,
// 			std::complex<double> &f_coeff_1, std::complex<double>  &f_coeff_2)
void ProgFSO::shellValue(double freq, double &rot, double &tilt,
			MultidimArray<std::complex<double>> &FT1, MultidimArray<std::complex<double>> &FT2,
			Matrix1D<double> &freq_fourier_x,
			Matrix1D<double> &freq_fourier_y,
			Matrix1D<double> &freq_fourier_z,
			std::complex<double> &f_coeff_1, std::complex<double>  &f_coeff_2)
{
	double x_dir, y_dir, z_dir;

	x_dir = sin(tilt*PI/180)*cos(rot*PI/180);
	y_dir = sin(tilt*PI/180)*sin(rot*PI/180);
	z_dir = cos(tilt*PI/180);

	size_t idxx, idxy, idxz;

	findIndexinVector(freq, x_dir, idxx, freq_fourier_x);
	findIndexinVector(freq, y_dir, idxy, freq_fourier_y);
	findIndexinVector(freq, z_dir, idxz, freq_fourier_z);
	// exit(0);
	f_coeff_1 = dAkij(FT1, idxz, idxx, idxy);
	f_coeff_2 = dAkij(FT2, idxz, idxx, idxy);



	// double lastcosine = 0;

    // FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(f1)
	// {
	// 	double cosine = x_dir*MAT_ELEM(indexesFourier, 1, n) + y_dir*MAT_ELEM(indexesFourier, 2, n) +
	// 					z_dir*MAT_ELEM(indexesFourier, 0, n);
		
	// 	if (cosine > lastcosine)
	// 	{
	// 		f_coeff_1 = DIRECT_MULTIDIM_ELEM(f1, n);
	// 		f_coeff_2 = DIRECT_MULTIDIM_ELEM(f2, n);
	// 		lastcosine = cosine;
	// 	}
	// 	if (cosine < -lastcosine)
	// 	{
	// 		f_coeff_1 = conj(DIRECT_MULTIDIM_ELEM(f1, n));
	// 		f_coeff_2 = conj(DIRECT_MULTIDIM_ELEM(f2, n));
	// 		lastcosine = fabs(cosine);
	// 	}
	// }
	
}


void ProgFSO::fscShell(MultidimArray< std::complex< double > > & FT1,
			MultidimArray< std::complex< double > > & FT2,
			Matrix1D<double> &freq_fourier_x,
			Matrix1D<double> &freq_fourier_y,
			Matrix1D<double> &freq_fourier_z,
			MultidimArray< double >& freqMap,
			int m1sizeX, Matrix2D<double> &indexesFourier, Matrix2D<int> &indexesFourier2, double &cutoff,
			MultidimArray<std::complex<double>> &f1, MultidimArray<std::complex<double>> &f2)
{
	int idxcutoff = (int) round(cutoff * m1sizeX);
	std::cout << "shell = " << idxcutoff << std::endl;
	int ZdimFT1=(int)ZSIZE(FT1);
	int YdimFT1=(int)YSIZE(FT1);
	int XdimFT1=(int)XSIZE(FT1);

	long n = 0;
	int Nelems = 0;
	//Here he determine the number of elements of the FSC shell (that number is Nelems)
	for (int k=0; k<ZdimFT1; k++)
	{
		for (int i=0; i<YdimFT1; i++)
		{
			for (int j=0; j<XdimFT1; j++)
			{

				if (DIRECT_MULTIDIM_ELEM(freqMap,n)<2)
				{
					++n;
					continue;
				}
				double f = 1/DIRECT_MULTIDIM_ELEM(freqMap,n);
				++n;
				int idx = (int) round(f * m1sizeX);

				if (idx != idxcutoff)
					continue;
				Nelems++;
			}
		}
	}

	f1.initZeros(Nelems);
	f2.initZeros(Nelems);
	int counter = 0;

	std::cout << "shell elements = " << Nelems << std::endl;

	indexesFourier.initZeros(3,Nelems);
	indexesFourier2.initZeros(3,Nelems);

	n = 0;
	for (int k=0; k<ZdimFT1; k++)
	{
		
		for (int i=0; i<YdimFT1; i++)
		{
			for (int j=0; j<XdimFT1; j++)
			{

				double iu = DIRECT_MULTIDIM_ELEM(freqMap,n);

				if (iu<2)
				{
					++n;
					continue;
				}
				double f = 1/iu;
				++n;
				int idx = (int) round(f * m1sizeX);

				if (idx != idxcutoff)
					continue;

				DIRECT_MULTIDIM_ELEM(f1,counter) = dAkij(FT1, k, i, j);
				DIRECT_MULTIDIM_ELEM(f2,counter) = dAkij(FT2, k, i, j);

				MAT_ELEM(indexesFourier, 0, counter) = VEC_ELEM(freq_fourier_z,k)*iu;
				MAT_ELEM(indexesFourier, 1, counter) = VEC_ELEM(freq_fourier_x,j)*iu;
				MAT_ELEM(indexesFourier, 2, counter) = VEC_ELEM(freq_fourier_y,i)*iu;
//					MAT_ELEM(indexesFourier, 3, counter) = iu;
				MAT_ELEM(indexesFourier2, 0, counter) = k;
				MAT_ELEM(indexesFourier2, 1, counter) = j;
				MAT_ELEM(indexesFourier2, 2, counter) = i;
				counter++;
			}
		}
	}

	// MultidimArray<double> rsh;
	// rsh.initZeros(Nelems);
	// FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(f1)
	// 	DIRECT_MULTIDIM_ELEM(rsh,n) = real(DIRECT_MULTIDIM_ELEM(f1,n));
    // std::sort(&DIRECT_MULTIDIM_ELEM(rsh,0), &DIRECT_MULTIDIM_ELEM(rsh,Nelems));
	// std::cout << rsh << std::endl;

}


void ProgFSO::generateDirections(Matrix2D<double> &angles, bool alot)
{
	if (alot == true)
	{
		angles.initZeros(2,321);
		MAT_ELEM(angles,0,0)=0;MAT_ELEM(angles,1,0)=0;
		MAT_ELEM(angles,0,1)=324;MAT_ELEM(angles,1,1)=63.4349;
		MAT_ELEM(angles,0,2)=36;MAT_ELEM(angles,1,2)=63.4349;
		MAT_ELEM(angles,0,3)=180;MAT_ELEM(angles,1,3)=63.435;
		MAT_ELEM(angles,0,4)=252;MAT_ELEM(angles,1,4)=63.435;
		MAT_ELEM(angles,0,5)=108;MAT_ELEM(angles,1,5)=63.435;
		MAT_ELEM(angles,0,6)=324;MAT_ELEM(angles,1,6)=31.7175;
		MAT_ELEM(angles,0,7)=36;MAT_ELEM(angles,1,7)=31.7175;
		MAT_ELEM(angles,0,8)=0;MAT_ELEM(angles,1,8)=58.2825;
		MAT_ELEM(angles,0,9)=288;MAT_ELEM(angles,1,9)=58.2825;
		MAT_ELEM(angles,0,10)=342;MAT_ELEM(angles,1,10)=90;
		MAT_ELEM(angles,0,11)=306;MAT_ELEM(angles,1,11)=90;
		MAT_ELEM(angles,0,12)=72;MAT_ELEM(angles,1,12)=58.2825;
		MAT_ELEM(angles,0,13)=18;MAT_ELEM(angles,1,13)=90;
		MAT_ELEM(angles,0,14)=54;MAT_ELEM(angles,1,14)=90;
		MAT_ELEM(angles,0,15)=90;MAT_ELEM(angles,1,15)=90;
		MAT_ELEM(angles,0,16)=216;MAT_ELEM(angles,1,16)=58.282;
		MAT_ELEM(angles,0,17)=144;MAT_ELEM(angles,1,17)=58.282;
		MAT_ELEM(angles,0,18)=180;MAT_ELEM(angles,1,18)=31.718;
		MAT_ELEM(angles,0,19)=252;MAT_ELEM(angles,1,19)=31.718;
		MAT_ELEM(angles,0,20)=108;MAT_ELEM(angles,1,20)=31.718;
		MAT_ELEM(angles,0,21)=346.3862;MAT_ELEM(angles,1,21)=43.6469;
		MAT_ELEM(angles,0,22)=58.3862;MAT_ELEM(angles,1,22)=43.6469;
		MAT_ELEM(angles,0,23)=274.3862;MAT_ELEM(angles,1,23)=43.6469;
		MAT_ELEM(angles,0,24)=0;MAT_ELEM(angles,1,24)=90;
		MAT_ELEM(angles,0,25)=72;MAT_ELEM(angles,1,25)=90;
		MAT_ELEM(angles,0,26)=288;MAT_ELEM(angles,1,26)=90;
		MAT_ELEM(angles,0,27)=225.7323;MAT_ELEM(angles,1,27)=73.955;
		MAT_ELEM(angles,0,28)=153.7323;MAT_ELEM(angles,1,28)=73.955;
		MAT_ELEM(angles,0,29)=216;MAT_ELEM(angles,1,29)=26.565;
		MAT_ELEM(angles,0,30)=144;MAT_ELEM(angles,1,30)=26.565;
		MAT_ELEM(angles,0,31)=0;MAT_ELEM(angles,1,31)=26.5651;
		MAT_ELEM(angles,0,32)=72;MAT_ELEM(angles,1,32)=26.5651;
		MAT_ELEM(angles,0,33)=288;MAT_ELEM(angles,1,33)=26.5651;
		MAT_ELEM(angles,0,34)=350.2677;MAT_ELEM(angles,1,34)=73.9549;
		MAT_ELEM(angles,0,35)=62.2677;MAT_ELEM(angles,1,35)=73.9549;
		MAT_ELEM(angles,0,36)=278.2677;MAT_ELEM(angles,1,36)=73.9549;
		MAT_ELEM(angles,0,37)=206.2677;MAT_ELEM(angles,1,37)=73.955;
		MAT_ELEM(angles,0,38)=134.2677;MAT_ELEM(angles,1,38)=73.955;
		MAT_ELEM(angles,0,39)=202.3862;MAT_ELEM(angles,1,39)=43.647;
		MAT_ELEM(angles,0,40)=130.3862;MAT_ELEM(angles,1,40)=43.647;
		MAT_ELEM(angles,0,41)=13.6138;MAT_ELEM(angles,1,41)=43.6469;
		MAT_ELEM(angles,0,42)=85.6138;MAT_ELEM(angles,1,42)=43.6469;
		MAT_ELEM(angles,0,43)=301.6138;MAT_ELEM(angles,1,43)=43.6469;
		MAT_ELEM(angles,0,44)=9.7323;MAT_ELEM(angles,1,44)=73.9549;
		MAT_ELEM(angles,0,45)=81.7323;MAT_ELEM(angles,1,45)=73.9549;
		MAT_ELEM(angles,0,46)=297.7323;MAT_ELEM(angles,1,46)=73.9549;
		MAT_ELEM(angles,0,47)=36;MAT_ELEM(angles,1,47)=90;
		MAT_ELEM(angles,0,48)=324;MAT_ELEM(angles,1,48)=90;
		MAT_ELEM(angles,0,49)=229.6138;MAT_ELEM(angles,1,49)=43.647;
		MAT_ELEM(angles,0,50)=157.6138;MAT_ELEM(angles,1,50)=43.647;
		MAT_ELEM(angles,0,51)=324;MAT_ELEM(angles,1,51)=15.8587;
		MAT_ELEM(angles,0,52)=36;MAT_ELEM(angles,1,52)=15.8587;
		MAT_ELEM(angles,0,53)=341.533;MAT_ELEM(angles,1,53)=59.6208;
		MAT_ELEM(angles,0,54)=306.467;MAT_ELEM(angles,1,54)=59.6208;
		MAT_ELEM(angles,0,55)=333.5057;MAT_ELEM(angles,1,55)=76.5584;
		MAT_ELEM(angles,0,56)=314.4943;MAT_ELEM(angles,1,56)=76.5584;
		MAT_ELEM(angles,0,57)=53.533;MAT_ELEM(angles,1,57)=59.6208;
		MAT_ELEM(angles,0,58)=26.4943;MAT_ELEM(angles,1,58)=76.5584;
		MAT_ELEM(angles,0,59)=45.5057;MAT_ELEM(angles,1,59)=76.5584;
		MAT_ELEM(angles,0,60)=197.533;MAT_ELEM(angles,1,60)=59.621;
		MAT_ELEM(angles,0,61)=162.467;MAT_ELEM(angles,1,61)=59.621;
		MAT_ELEM(angles,0,62)=180;MAT_ELEM(angles,1,62)=47.576;
		MAT_ELEM(angles,0,63)=269.533;MAT_ELEM(angles,1,63)=59.621;
		MAT_ELEM(angles,0,64)=252;MAT_ELEM(angles,1,64)=47.576;
		MAT_ELEM(angles,0,65)=108;MAT_ELEM(angles,1,65)=47.576;
		MAT_ELEM(angles,0,66)=324;MAT_ELEM(angles,1,66)=47.5762;
		MAT_ELEM(angles,0,67)=36;MAT_ELEM(angles,1,67)=47.5762;
		MAT_ELEM(angles,0,68)=18.467;MAT_ELEM(angles,1,68)=59.6208;
		MAT_ELEM(angles,0,69)=170.4943;MAT_ELEM(angles,1,69)=76.558;
		MAT_ELEM(angles,0,70)=117.5057;MAT_ELEM(angles,1,70)=76.558;
		MAT_ELEM(angles,0,71)=189.5057;MAT_ELEM(angles,1,71)=76.558;
		MAT_ELEM(angles,0,72)=242.4943;MAT_ELEM(angles,1,72)=76.558;
		MAT_ELEM(angles,0,73)=261.5057;MAT_ELEM(angles,1,73)=76.558;
		MAT_ELEM(angles,0,74)=98.4943;MAT_ELEM(angles,1,74)=76.558;
		MAT_ELEM(angles,0,75)=234.467;MAT_ELEM(angles,1,75)=59.621;
		MAT_ELEM(angles,0,76)=125.533;MAT_ELEM(angles,1,76)=59.621;
		MAT_ELEM(angles,0,77)=180;MAT_ELEM(angles,1,77)=15.859;
		MAT_ELEM(angles,0,78)=252;MAT_ELEM(angles,1,78)=15.859;
		MAT_ELEM(angles,0,79)=90.467;MAT_ELEM(angles,1,79)=59.621;
		MAT_ELEM(angles,0,80)=108;MAT_ELEM(angles,1,80)=15.859;
		MAT_ELEM(angles,0,81)=0;MAT_ELEM(angles,1,81)=42.8321;
		MAT_ELEM(angles,0,82)=72;MAT_ELEM(angles,1,82)=42.8321;
		MAT_ELEM(angles,0,83)=288;MAT_ELEM(angles,1,83)=42.8321;
		MAT_ELEM(angles,0,84)=4.7693;MAT_ELEM(angles,1,84)=81.9488;
		MAT_ELEM(angles,0,85)=76.7693;MAT_ELEM(angles,1,85)=81.9488;
		MAT_ELEM(angles,0,86)=292.7693;MAT_ELEM(angles,1,86)=81.9488;
		MAT_ELEM(angles,0,87)=220.7693;MAT_ELEM(angles,1,87)=81.9488;
		MAT_ELEM(angles,0,88)=148.7693;MAT_ELEM(angles,1,88)=81.9488;
		MAT_ELEM(angles,0,89)=224.2677;MAT_ELEM(angles,1,89)=34.924;
		MAT_ELEM(angles,0,90)=152.2677;MAT_ELEM(angles,1,90)=34.924;
		MAT_ELEM(angles,0,91)=13.5146;MAT_ELEM(angles,1,91)=20.3172;
		MAT_ELEM(angles,0,92)=85.5146;MAT_ELEM(angles,1,92)=20.3172;
		MAT_ELEM(angles,0,93)=301.5146;MAT_ELEM(angles,1,93)=20.3172;
		MAT_ELEM(angles,0,94)=346.1363;MAT_ELEM(angles,1,94)=66.7276;
		MAT_ELEM(angles,0,95)=58.1363;MAT_ELEM(angles,1,95)=66.7276;
		MAT_ELEM(angles,0,96)=274.1363;MAT_ELEM(angles,1,96)=66.7276;
		MAT_ELEM(angles,0,97)=197.8362;MAT_ELEM(angles,1,97)=75.105;
		MAT_ELEM(angles,0,98)=269.8362;MAT_ELEM(angles,1,98)=75.105;
		MAT_ELEM(angles,0,99)=125.8362;MAT_ELEM(angles,1,99)=75.105;
		MAT_ELEM(angles,0,100)=199.6899;MAT_ELEM(angles,1,100)=51.609;
		MAT_ELEM(angles,0,101)=127.6899;MAT_ELEM(angles,1,101)=51.609;
		MAT_ELEM(angles,0,102)=334.8124;MAT_ELEM(angles,1,102)=45.0621;
		MAT_ELEM(angles,0,103)=46.8124;MAT_ELEM(angles,1,103)=45.0621;
		MAT_ELEM(angles,0,104)=175.3133;MAT_ELEM(angles,1,104)=83.2562;
		MAT_ELEM(angles,0,105)=247.3133;MAT_ELEM(angles,1,105)=83.2562;
		MAT_ELEM(angles,0,106)=103.3133;MAT_ELEM(angles,1,106)=83.2562;
		MAT_ELEM(angles,0,107)=229.8637;MAT_ELEM(angles,1,107)=66.728;
		MAT_ELEM(angles,0,108)=157.8637;MAT_ELEM(angles,1,108)=66.728;
		MAT_ELEM(angles,0,109)=202.4854;MAT_ELEM(angles,1,109)=20.317;
		MAT_ELEM(angles,0,110)=130.4854;MAT_ELEM(angles,1,110)=20.317;
		MAT_ELEM(angles,0,111)=16.3101;MAT_ELEM(angles,1,111)=51.6091;
		MAT_ELEM(angles,0,112)=88.3101;MAT_ELEM(angles,1,112)=51.6091;
		MAT_ELEM(angles,0,113)=304.3101;MAT_ELEM(angles,1,113)=51.6091;
		MAT_ELEM(angles,0,114)=18.1638;MAT_ELEM(angles,1,114)=75.1046;
		MAT_ELEM(angles,0,115)=306.1638;MAT_ELEM(angles,1,115)=75.1046;
		MAT_ELEM(angles,0,116)=40.6867;MAT_ELEM(angles,1,116)=83.2562;
		MAT_ELEM(angles,0,117)=328.6867;MAT_ELEM(angles,1,117)=83.2562;
		MAT_ELEM(angles,0,118)=241.1876;MAT_ELEM(angles,1,118)=45.062;
		MAT_ELEM(angles,0,119)=97.1876;MAT_ELEM(angles,1,119)=45.062;
		MAT_ELEM(angles,0,120)=169.1876;MAT_ELEM(angles,1,120)=45.062;
		MAT_ELEM(angles,0,121)=351.7323;MAT_ELEM(angles,1,121)=34.9243;
		MAT_ELEM(angles,0,122)=63.7323;MAT_ELEM(angles,1,122)=34.9243;
		MAT_ELEM(angles,0,123)=279.7323;MAT_ELEM(angles,1,123)=34.9243;
		MAT_ELEM(angles,0,124)=355.2307;MAT_ELEM(angles,1,124)=81.9488;
		MAT_ELEM(angles,0,125)=67.2307;MAT_ELEM(angles,1,125)=81.9488;
		MAT_ELEM(angles,0,126)=283.2307;MAT_ELEM(angles,1,126)=81.9488;
		MAT_ELEM(angles,0,127)=216;MAT_ELEM(angles,1,127)=73.733;
		MAT_ELEM(angles,0,128)=144;MAT_ELEM(angles,1,128)=73.733;
		MAT_ELEM(angles,0,129)=207.7323;MAT_ELEM(angles,1,129)=34.924;
		MAT_ELEM(angles,0,130)=135.7323;MAT_ELEM(angles,1,130)=34.924;
		MAT_ELEM(angles,0,131)=346.4854;MAT_ELEM(angles,1,131)=20.3172;
		MAT_ELEM(angles,0,132)=58.4854;MAT_ELEM(angles,1,132)=20.3172;
		MAT_ELEM(angles,0,133)=274.4854;MAT_ELEM(angles,1,133)=20.3172;
		MAT_ELEM(angles,0,134)=341.8362;MAT_ELEM(angles,1,134)=75.1046;
		MAT_ELEM(angles,0,135)=53.8362;MAT_ELEM(angles,1,135)=75.1046;
		MAT_ELEM(angles,0,136)=202.1363;MAT_ELEM(angles,1,136)=66.728;
		MAT_ELEM(angles,0,137)=130.1363;MAT_ELEM(angles,1,137)=66.728;
		MAT_ELEM(angles,0,138)=190.8124;MAT_ELEM(angles,1,138)=45.062;
		MAT_ELEM(angles,0,139)=262.8124;MAT_ELEM(angles,1,139)=45.062;
		MAT_ELEM(angles,0,140)=118.8124;MAT_ELEM(angles,1,140)=45.062;
		MAT_ELEM(angles,0,141)=343.6899;MAT_ELEM(angles,1,141)=51.6091;
		MAT_ELEM(angles,0,142)=55.6899;MAT_ELEM(angles,1,142)=51.6091;
		MAT_ELEM(angles,0,143)=271.6899;MAT_ELEM(angles,1,143)=51.6091;
		MAT_ELEM(angles,0,144)=184.6867;MAT_ELEM(angles,1,144)=83.2562;
		MAT_ELEM(angles,0,145)=256.6867;MAT_ELEM(angles,1,145)=83.2562;
		MAT_ELEM(angles,0,146)=112.6867;MAT_ELEM(angles,1,146)=83.2562;
		MAT_ELEM(angles,0,147)=234.1638;MAT_ELEM(angles,1,147)=75.105;
		MAT_ELEM(angles,0,148)=90.1638;MAT_ELEM(angles,1,148)=75.105;
		MAT_ELEM(angles,0,149)=162.1638;MAT_ELEM(angles,1,149)=75.105;
		MAT_ELEM(angles,0,150)=229.5146;MAT_ELEM(angles,1,150)=20.317;
		MAT_ELEM(angles,0,151)=157.5146;MAT_ELEM(angles,1,151)=20.317;
		MAT_ELEM(angles,0,152)=25.1876;MAT_ELEM(angles,1,152)=45.0621;
		MAT_ELEM(angles,0,153)=313.1876;MAT_ELEM(angles,1,153)=45.0621;
		MAT_ELEM(angles,0,154)=13.8637;MAT_ELEM(angles,1,154)=66.7276;
		MAT_ELEM(angles,0,155)=85.8637;MAT_ELEM(angles,1,155)=66.7276;
		MAT_ELEM(angles,0,156)=301.8637;MAT_ELEM(angles,1,156)=66.7276;
		MAT_ELEM(angles,0,157)=31.3133;MAT_ELEM(angles,1,157)=83.2562;
		MAT_ELEM(angles,0,158)=319.3133;MAT_ELEM(angles,1,158)=83.2562;
		MAT_ELEM(angles,0,159)=232.3101;MAT_ELEM(angles,1,159)=51.609;
		MAT_ELEM(angles,0,160)=160.3101;MAT_ELEM(angles,1,160)=51.609;
		MAT_ELEM(angles,0,161)=8.2677;MAT_ELEM(angles,1,161)=34.9243;
		MAT_ELEM(angles,0,162)=80.2677;MAT_ELEM(angles,1,162)=34.9243;
		MAT_ELEM(angles,0,163)=296.2677;MAT_ELEM(angles,1,163)=34.9243;
		MAT_ELEM(angles,0,164)=0;MAT_ELEM(angles,1,164)=73.733;
		MAT_ELEM(angles,0,165)=72;MAT_ELEM(angles,1,165)=73.733;
		MAT_ELEM(angles,0,166)=288;MAT_ELEM(angles,1,166)=73.733;
		MAT_ELEM(angles,0,167)=211.2307;MAT_ELEM(angles,1,167)=81.9488;
		MAT_ELEM(angles,0,168)=139.2307;MAT_ELEM(angles,1,168)=81.9488;
		MAT_ELEM(angles,0,169)=216;MAT_ELEM(angles,1,169)=42.832;
		MAT_ELEM(angles,0,170)=144;MAT_ELEM(angles,1,170)=42.832;
		MAT_ELEM(angles,0,171)=0;MAT_ELEM(angles,1,171)=12.9432;
		MAT_ELEM(angles,0,172)=72;MAT_ELEM(angles,1,172)=12.9432;
		MAT_ELEM(angles,0,173)=288;MAT_ELEM(angles,1,173)=12.9432;
		MAT_ELEM(angles,0,174)=337.2786;MAT_ELEM(angles,1,174)=68.041;
		MAT_ELEM(angles,0,175)=49.2786;MAT_ELEM(angles,1,175)=68.041;
		MAT_ELEM(angles,0,176)=193.2786;MAT_ELEM(angles,1,176)=68.041;
		MAT_ELEM(angles,0,177)=265.2786;MAT_ELEM(angles,1,177)=68.041;
		MAT_ELEM(angles,0,178)=121.2786;MAT_ELEM(angles,1,178)=68.041;
		MAT_ELEM(angles,0,179)=189.4537;MAT_ELEM(angles,1,179)=53.278;
		MAT_ELEM(angles,0,180)=261.4537;MAT_ELEM(angles,1,180)=53.278;
		MAT_ELEM(angles,0,181)=117.4537;MAT_ELEM(angles,1,181)=53.278;
		MAT_ELEM(angles,0,182)=333.4537;MAT_ELEM(angles,1,182)=53.2783;
		MAT_ELEM(angles,0,183)=45.4537;MAT_ELEM(angles,1,183)=53.2783;
		MAT_ELEM(angles,0,184)=180;MAT_ELEM(angles,1,184)=76.378;
		MAT_ELEM(angles,0,185)=252;MAT_ELEM(angles,1,185)=76.378;
		MAT_ELEM(angles,0,186)=108;MAT_ELEM(angles,1,186)=76.378;
		MAT_ELEM(angles,0,187)=238.7214;MAT_ELEM(angles,1,187)=68.041;
		MAT_ELEM(angles,0,188)=94.7214;MAT_ELEM(angles,1,188)=68.041;
		MAT_ELEM(angles,0,189)=166.7214;MAT_ELEM(angles,1,189)=68.041;
		MAT_ELEM(angles,0,190)=216;MAT_ELEM(angles,1,190)=12.943;
		MAT_ELEM(angles,0,191)=144;MAT_ELEM(angles,1,191)=12.943;
		MAT_ELEM(angles,0,192)=26.5463;MAT_ELEM(angles,1,192)=53.2783;
		MAT_ELEM(angles,0,193)=314.5463;MAT_ELEM(angles,1,193)=53.2783;
		MAT_ELEM(angles,0,194)=22.7214;MAT_ELEM(angles,1,194)=68.041;
		MAT_ELEM(angles,0,195)=310.7214;MAT_ELEM(angles,1,195)=68.041;
		MAT_ELEM(angles,0,196)=36;MAT_ELEM(angles,1,196)=76.3782;
		MAT_ELEM(angles,0,197)=324;MAT_ELEM(angles,1,197)=76.3782;
		MAT_ELEM(angles,0,198)=242.5463;MAT_ELEM(angles,1,198)=53.278;
		MAT_ELEM(angles,0,199)=98.5463;MAT_ELEM(angles,1,199)=53.278;
		MAT_ELEM(angles,0,200)=170.5463;MAT_ELEM(angles,1,200)=53.278;
		MAT_ELEM(angles,0,201)=336.7264;MAT_ELEM(angles,1,201)=37.1611;
		MAT_ELEM(angles,0,202)=48.7264;MAT_ELEM(angles,1,202)=37.1611;
		MAT_ELEM(angles,0,203)=351;MAT_ELEM(angles,1,203)=90;
		MAT_ELEM(angles,0,204)=63;MAT_ELEM(angles,1,204)=90;
		MAT_ELEM(angles,0,205)=279;MAT_ELEM(angles,1,205)=90;
		MAT_ELEM(angles,0,206)=221.1634;MAT_ELEM(angles,1,206)=66.042;
		MAT_ELEM(angles,0,207)=149.1634;MAT_ELEM(angles,1,207)=66.042;
		MAT_ELEM(angles,0,208)=196.498;MAT_ELEM(angles,1,208)=27.943;
		MAT_ELEM(angles,0,209)=268.498;MAT_ELEM(angles,1,209)=27.943;
		MAT_ELEM(angles,0,210)=124.498;MAT_ELEM(angles,1,210)=27.943;
		MAT_ELEM(angles,0,211)=340.498;MAT_ELEM(angles,1,211)=27.9429;
		MAT_ELEM(angles,0,212)=52.498;MAT_ELEM(angles,1,212)=27.9429;
		MAT_ELEM(angles,0,213)=346.0516;MAT_ELEM(angles,1,213)=81.9568;
		MAT_ELEM(angles,0,214)=58.0516;MAT_ELEM(angles,1,214)=81.9568;
		MAT_ELEM(angles,0,215)=274.0516;MAT_ELEM(angles,1,215)=81.9568;
		MAT_ELEM(angles,0,216)=210.8366;MAT_ELEM(angles,1,216)=66.042;
		MAT_ELEM(angles,0,217)=138.8366;MAT_ELEM(angles,1,217)=66.042;
		MAT_ELEM(angles,0,218)=192.7264;MAT_ELEM(angles,1,218)=37.161;
		MAT_ELEM(angles,0,219)=264.7264;MAT_ELEM(angles,1,219)=37.161;
		MAT_ELEM(angles,0,220)=120.7264;MAT_ELEM(angles,1,220)=37.161;
		MAT_ELEM(angles,0,221)=6.0948;MAT_ELEM(angles,1,221)=50.7685;
		MAT_ELEM(angles,0,222)=78.0948;MAT_ELEM(angles,1,222)=50.7685;
		MAT_ELEM(angles,0,223)=294.0948;MAT_ELEM(angles,1,223)=50.7685;
		MAT_ELEM(angles,0,224)=13.9484;MAT_ELEM(angles,1,224)=81.9568;
		MAT_ELEM(angles,0,225)=85.9484;MAT_ELEM(angles,1,225)=81.9568;
		MAT_ELEM(angles,0,226)=301.9484;MAT_ELEM(angles,1,226)=81.9568;
		MAT_ELEM(angles,0,227)=45;MAT_ELEM(angles,1,227)=90;
		MAT_ELEM(angles,0,228)=333;MAT_ELEM(angles,1,228)=90;
		MAT_ELEM(angles,0,229)=239.2736;MAT_ELEM(angles,1,229)=37.161;
		MAT_ELEM(angles,0,230)=95.2736;MAT_ELEM(angles,1,230)=37.161;
		MAT_ELEM(angles,0,231)=167.2736;MAT_ELEM(angles,1,231)=37.161;
		MAT_ELEM(angles,0,232)=324;MAT_ELEM(angles,1,232)=7.9294;
		MAT_ELEM(angles,0,233)=36;MAT_ELEM(angles,1,233)=7.9294;
		MAT_ELEM(angles,0,234)=332.6069;MAT_ELEM(angles,1,234)=61.2449;
		MAT_ELEM(angles,0,235)=315.3931;MAT_ELEM(angles,1,235)=61.2449;
		MAT_ELEM(angles,0,236)=328.9523;MAT_ELEM(angles,1,236)=69.9333;
		MAT_ELEM(angles,0,237)=319.0477;MAT_ELEM(angles,1,237)=69.9333;
		MAT_ELEM(angles,0,238)=44.6069;MAT_ELEM(angles,1,238)=61.2449;
		MAT_ELEM(angles,0,239)=31.0477;MAT_ELEM(angles,1,239)=69.9333;
		MAT_ELEM(angles,0,240)=40.9523;MAT_ELEM(angles,1,240)=69.9333;
		MAT_ELEM(angles,0,241)=188.6069;MAT_ELEM(angles,1,241)=61.245;
		MAT_ELEM(angles,0,242)=171.3931;MAT_ELEM(angles,1,242)=61.245;
		MAT_ELEM(angles,0,243)=180;MAT_ELEM(angles,1,243)=55.506;
		MAT_ELEM(angles,0,244)=260.6069;MAT_ELEM(angles,1,244)=61.245;
		MAT_ELEM(angles,0,245)=252;MAT_ELEM(angles,1,245)=55.506;
		MAT_ELEM(angles,0,246)=108;MAT_ELEM(angles,1,246)=55.506;
		MAT_ELEM(angles,0,247)=324;MAT_ELEM(angles,1,247)=39.6468;
		MAT_ELEM(angles,0,248)=36;MAT_ELEM(angles,1,248)=39.6468;
		MAT_ELEM(angles,0,249)=9.299;MAT_ELEM(angles,1,249)=58.6205;
		MAT_ELEM(angles,0,250)=278.701;MAT_ELEM(angles,1,250)=58.6205;
		MAT_ELEM(angles,0,251)=166.1881;MAT_ELEM(angles,1,251)=83.2609;
		MAT_ELEM(angles,0,252)=121.8119;MAT_ELEM(angles,1,252)=83.2609;
		MAT_ELEM(angles,0,253)=81.299;MAT_ELEM(angles,1,253)=58.6205;
		MAT_ELEM(angles,0,254)=193.8119;MAT_ELEM(angles,1,254)=83.2609;
		MAT_ELEM(angles,0,255)=238.1881;MAT_ELEM(angles,1,255)=83.2609;
		MAT_ELEM(angles,0,256)=265.8119;MAT_ELEM(angles,1,256)=83.2609;
		MAT_ELEM(angles,0,257)=94.1881;MAT_ELEM(angles,1,257)=83.2609;
		MAT_ELEM(angles,0,258)=225.299;MAT_ELEM(angles,1,258)=58.621;
		MAT_ELEM(angles,0,259)=134.701;MAT_ELEM(angles,1,259)=58.621;
		MAT_ELEM(angles,0,260)=180;MAT_ELEM(angles,1,260)=23.788;
		MAT_ELEM(angles,0,261)=252;MAT_ELEM(angles,1,261)=23.788;
		MAT_ELEM(angles,0,262)=108;MAT_ELEM(angles,1,262)=23.788;
		MAT_ELEM(angles,0,263)=353.9052;MAT_ELEM(angles,1,263)=50.7685;
		MAT_ELEM(angles,0,264)=65.9052;MAT_ELEM(angles,1,264)=50.7685;
		MAT_ELEM(angles,0,265)=281.9052;MAT_ELEM(angles,1,265)=50.7685;
		MAT_ELEM(angles,0,266)=9;MAT_ELEM(angles,1,266)=90;
		MAT_ELEM(angles,0,267)=81;MAT_ELEM(angles,1,267)=90;
		MAT_ELEM(angles,0,268)=297;MAT_ELEM(angles,1,268)=90;
		MAT_ELEM(angles,0,269)=229.9484;MAT_ELEM(angles,1,269)=81.9568;
		MAT_ELEM(angles,0,270)=157.9484;MAT_ELEM(angles,1,270)=81.9568;
		MAT_ELEM(angles,0,271)=235.502;MAT_ELEM(angles,1,271)=27.943;
		MAT_ELEM(angles,0,272)=91.502;MAT_ELEM(angles,1,272)=27.943;
		MAT_ELEM(angles,0,273)=163.502;MAT_ELEM(angles,1,273)=27.943;
		MAT_ELEM(angles,0,274)=19.502;MAT_ELEM(angles,1,274)=27.9429;
		MAT_ELEM(angles,0,275)=307.502;MAT_ELEM(angles,1,275)=27.9429;
		MAT_ELEM(angles,0,276)=354.8366;MAT_ELEM(angles,1,276)=66.0423;
		MAT_ELEM(angles,0,277)=66.8366;MAT_ELEM(angles,1,277)=66.0423;
		MAT_ELEM(angles,0,278)=282.8366;MAT_ELEM(angles,1,278)=66.0423;
		MAT_ELEM(angles,0,279)=202.0516;MAT_ELEM(angles,1,279)=81.9568;
		MAT_ELEM(angles,0,280)=130.0516;MAT_ELEM(angles,1,280)=81.9568;
		MAT_ELEM(angles,0,281)=209.9052;MAT_ELEM(angles,1,281)=50.768;
		MAT_ELEM(angles,0,282)=137.9052;MAT_ELEM(angles,1,282)=50.768;
		MAT_ELEM(angles,0,283)=23.2736;MAT_ELEM(angles,1,283)=37.1611;
		MAT_ELEM(angles,0,284)=311.2736;MAT_ELEM(angles,1,284)=37.1611;
		MAT_ELEM(angles,0,285)=5.1634;MAT_ELEM(angles,1,285)=66.0423;
		MAT_ELEM(angles,0,286)=77.1634;MAT_ELEM(angles,1,286)=66.0423;
		MAT_ELEM(angles,0,287)=293.1634;MAT_ELEM(angles,1,287)=66.0423;
		MAT_ELEM(angles,0,288)=27;MAT_ELEM(angles,1,288)=90;
		MAT_ELEM(angles,0,289)=315;MAT_ELEM(angles,1,289)=90;
		MAT_ELEM(angles,0,290)=222.0948;MAT_ELEM(angles,1,290)=50.768;
		MAT_ELEM(angles,0,291)=150.0948;MAT_ELEM(angles,1,291)=50.768;
		MAT_ELEM(angles,0,292)=324;MAT_ELEM(angles,1,292)=23.7881;
		MAT_ELEM(angles,0,293)=36;MAT_ELEM(angles,1,293)=23.7881;
		MAT_ELEM(angles,0,294)=350.701;MAT_ELEM(angles,1,294)=58.6205;
		MAT_ELEM(angles,0,295)=297.299;MAT_ELEM(angles,1,295)=58.6205;
		MAT_ELEM(angles,0,296)=337.8119;MAT_ELEM(angles,1,296)=83.2609;
		MAT_ELEM(angles,0,297)=310.1881;MAT_ELEM(angles,1,297)=83.2609;
		MAT_ELEM(angles,0,298)=62.701;MAT_ELEM(angles,1,298)=58.6205;
		MAT_ELEM(angles,0,299)=22.1881;MAT_ELEM(angles,1,299)=83.2609;
		MAT_ELEM(angles,0,300)=49.8119;MAT_ELEM(angles,1,300)=83.2609;
		MAT_ELEM(angles,0,301)=206.701;MAT_ELEM(angles,1,301)=58.621;
		MAT_ELEM(angles,0,302)=153.299;MAT_ELEM(angles,1,302)=58.621;
		MAT_ELEM(angles,0,303)=180;MAT_ELEM(angles,1,303)=39.647;
		MAT_ELEM(angles,0,304)=252;MAT_ELEM(angles,1,304)=39.647;
		MAT_ELEM(angles,0,305)=108;MAT_ELEM(angles,1,305)=39.647;
		MAT_ELEM(angles,0,306)=324;MAT_ELEM(angles,1,306)=55.5056;
		MAT_ELEM(angles,0,307)=36;MAT_ELEM(angles,1,307)=55.5056;
		MAT_ELEM(angles,0,308)=27.3931;MAT_ELEM(angles,1,308)=61.2449;
		MAT_ELEM(angles,0,309)=175.0477;MAT_ELEM(angles,1,309)=69.933;
		MAT_ELEM(angles,0,310)=112.9523;MAT_ELEM(angles,1,310)=69.933;
		MAT_ELEM(angles,0,311)=184.9523;MAT_ELEM(angles,1,311)=69.933;
		MAT_ELEM(angles,0,312)=247.0477;MAT_ELEM(angles,1,312)=69.933;
		MAT_ELEM(angles,0,313)=256.9523;MAT_ELEM(angles,1,313)=69.933;
		MAT_ELEM(angles,0,314)=103.0477;MAT_ELEM(angles,1,314)=69.933;
		MAT_ELEM(angles,0,315)=243.3931;MAT_ELEM(angles,1,315)=61.245;
		MAT_ELEM(angles,0,316)=116.6069;MAT_ELEM(angles,1,316)=61.245;
		MAT_ELEM(angles,0,317)=180;MAT_ELEM(angles,1,317)=7.929;
		MAT_ELEM(angles,0,318)=252;MAT_ELEM(angles,1,318)=7.929;
		MAT_ELEM(angles,0,319)=99.3931;MAT_ELEM(angles,1,319)=61.245;
		MAT_ELEM(angles,0,320)=108;MAT_ELEM(angles,1,320)=7.929;
	}
	else
	{
	//TODO: use coordinates on the sphere instead of angles
	angles.initZeros(2,81);
	MAT_ELEM(angles, 0, 0) = 0.000000;	 	 MAT_ELEM(angles, 1, 0) = 0.000000;
	MAT_ELEM(angles, 0, 1) = 36.000000;	 	 MAT_ELEM(angles, 1, 1) = 15.858741;
	MAT_ELEM(angles, 0, 2) = 36.000000;	 	 MAT_ELEM(angles, 1, 2) = 31.717482;
	MAT_ELEM(angles, 0, 3) = 36.000000;	 	 MAT_ELEM(angles, 1, 3) = 47.576224;
	MAT_ELEM(angles, 0, 4) = 36.000000;	 	 MAT_ELEM(angles, 1, 4) = 63.434965;
	MAT_ELEM(angles, 0, 5) = 62.494295;	 	 MAT_ELEM(angles, 1, 5) = -76.558393;
	MAT_ELEM(angles, 0, 6) = 54.000000;	 	 MAT_ELEM(angles, 1, 6) = 90.000000;
	MAT_ELEM(angles, 0, 7) = 45.505705;	 	 MAT_ELEM(angles, 1, 7) = 76.558393;
	MAT_ELEM(angles, 0, 8) = 108.000000;	 MAT_ELEM(angles, 1, 8) = 15.858741;
	MAT_ELEM(angles, 0, 9) = 108.000000;	 MAT_ELEM(angles, 1, 9) = 31.717482;
	MAT_ELEM(angles, 0, 10) = 108.000000;	 MAT_ELEM(angles, 1, 10) = 47.576224;
	MAT_ELEM(angles, 0, 11) = 108.000000;	 MAT_ELEM(angles, 1, 11) = 63.434965;
	MAT_ELEM(angles, 0, 12) = 134.494295;	 MAT_ELEM(angles, 1, 12) = -76.558393;
	MAT_ELEM(angles, 0, 13) = 126.000000;	 MAT_ELEM(angles, 1, 13) = 90.000000;
	MAT_ELEM(angles, 0, 14) = 117.505705;	 MAT_ELEM(angles, 1, 14) = 76.558393;
	MAT_ELEM(angles, 0, 15) = 144.000000;	 MAT_ELEM(angles, 1, 15) = -15.858741;
	MAT_ELEM(angles, 0, 16) = 144.000000;	 MAT_ELEM(angles, 1, 16) = -31.717482;
	MAT_ELEM(angles, 0, 17) = 144.000000;	 MAT_ELEM(angles, 1, 17) = -47.576224;
	MAT_ELEM(angles, 0, 18) = 144.000000;	 MAT_ELEM(angles, 1, 18) = -63.434965;
	MAT_ELEM(angles, 0, 19) = 170.494295;	 MAT_ELEM(angles, 1, 19) = 76.558393;
	MAT_ELEM(angles, 0, 20) = 162.000000;	 MAT_ELEM(angles, 1, 20) = 90.000000;
	MAT_ELEM(angles, 0, 21) = 153.505705;	 MAT_ELEM(angles, 1, 21) = -76.558393;
	MAT_ELEM(angles, 0, 22) = 72.000000;	 MAT_ELEM(angles, 1, 22) = -15.858741;
	MAT_ELEM(angles, 0, 23) = 72.000000;	 MAT_ELEM(angles, 1, 23) = -31.717482;
	MAT_ELEM(angles, 0, 24) = 72.000000;	 MAT_ELEM(angles, 1, 24) = -47.576224;
	MAT_ELEM(angles, 0, 25) = 72.000000;	 MAT_ELEM(angles, 1, 25) = -63.434965;
	MAT_ELEM(angles, 0, 26) = 98.494295;	 MAT_ELEM(angles, 1, 26) = 76.558393;
	MAT_ELEM(angles, 0, 27) = 90.000000;	 MAT_ELEM(angles, 1, 27) = 90.000000;
	MAT_ELEM(angles, 0, 28) = 81.505705;	 MAT_ELEM(angles, 1, 28) = -76.558393;
	MAT_ELEM(angles, 0, 29) = 0.000000;	 	 MAT_ELEM(angles, 1, 29) = -15.858741;
	MAT_ELEM(angles, 0, 30) = 0.000000;	 	 MAT_ELEM(angles, 1, 30) = -31.717482;
	MAT_ELEM(angles, 0, 31) = 0.000000;	 	 MAT_ELEM(angles, 1, 31) = -47.576224;
	MAT_ELEM(angles, 0, 32) = 0.000000;	 	 MAT_ELEM(angles, 1, 32) = -63.434965;
	MAT_ELEM(angles, 0, 33) = 26.494295;	 MAT_ELEM(angles, 1, 33) = 76.558393;
	MAT_ELEM(angles, 0, 34) = 18.000000;	 MAT_ELEM(angles, 1, 34) = 90.000000;
	MAT_ELEM(angles, 0, 35) = 9.505705;	 	 MAT_ELEM(angles, 1, 35) = -76.558393;
	MAT_ELEM(angles, 0, 36) = 12.811021;	 MAT_ELEM(angles, 1, 36) = 42.234673;
	MAT_ELEM(angles, 0, 37) = 18.466996;	 MAT_ELEM(angles, 1, 37) = 59.620797;
	MAT_ELEM(angles, 0, 38) = 0.000000;	 	 MAT_ELEM(angles, 1, 38) = 90.000000;
	MAT_ELEM(angles, 0, 39) = 8.867209;	 	 MAT_ELEM(angles, 1, 39) = 75.219088;
	MAT_ELEM(angles, 0, 40) = 72.000000;	 MAT_ELEM(angles, 1, 40) = 26.565058;
	MAT_ELEM(angles, 0, 41) = 59.188979;	 MAT_ELEM(angles, 1, 41) = 42.234673;
	MAT_ELEM(angles, 0, 42) = 84.811021;	 MAT_ELEM(angles, 1, 42) = 42.234673;
	MAT_ELEM(angles, 0, 43) = 53.533003;	 MAT_ELEM(angles, 1, 43) = 59.620797;
	MAT_ELEM(angles, 0, 44) = 72.000000;	 MAT_ELEM(angles, 1, 44) = 58.282544;
	MAT_ELEM(angles, 0, 45) = 90.466996;	 MAT_ELEM(angles, 1, 45) = 59.620797;
	MAT_ELEM(angles, 0, 46) = 72.000000;	 MAT_ELEM(angles, 1, 46) = 90.000000;
	MAT_ELEM(angles, 0, 47) = 63.132791;	 MAT_ELEM(angles, 1, 47) = 75.219088;
	MAT_ELEM(angles, 0, 48) = 80.867209;	 MAT_ELEM(angles, 1, 48) = 75.219088;
	MAT_ELEM(angles, 0, 49) = 144.000000;	 MAT_ELEM(angles, 1, 49) = 26.565058;
	MAT_ELEM(angles, 0, 50) = 131.188979;	 MAT_ELEM(angles, 1, 50) = 42.234673;
	MAT_ELEM(angles, 0, 51) = 156.811021;	 MAT_ELEM(angles, 1, 51) = 42.234673;
	MAT_ELEM(angles, 0, 52) = 125.533003;	 MAT_ELEM(angles, 1, 52) = 59.620797;
	MAT_ELEM(angles, 0, 53) = 144.000000;	 MAT_ELEM(angles, 1, 53) = 58.282544;
	MAT_ELEM(angles, 0, 54) = 162.466996;	 MAT_ELEM(angles, 1, 54) = 59.620797;
	MAT_ELEM(angles, 0, 55) = 144.000000;	 MAT_ELEM(angles, 1, 55) = 90.000000;
	MAT_ELEM(angles, 0, 56) = 135.132791;	 MAT_ELEM(angles, 1, 56) = 75.219088;
	MAT_ELEM(angles, 0, 57) = 152.867209;	 MAT_ELEM(angles, 1, 57) = 75.219088;
	MAT_ELEM(angles, 0, 58) = 180.000000;	 MAT_ELEM(angles, 1, 58) = -26.565058;
	MAT_ELEM(angles, 0, 59) = 167.188979;	 MAT_ELEM(angles, 1, 59) = -42.234673;
	MAT_ELEM(angles, 0, 60) = 180.000000;	 MAT_ELEM(angles, 1, 60) = -58.282544;
	MAT_ELEM(angles, 0, 61) = 161.533003;	 MAT_ELEM(angles, 1, 61) = -59.620797;
	MAT_ELEM(angles, 0, 62) = 171.132791;	 MAT_ELEM(angles, 1, 62) = -75.219088;
	MAT_ELEM(angles, 0, 63) = 108.000000;	 MAT_ELEM(angles, 1, 63) = -26.565058;
	MAT_ELEM(angles, 0, 64) = 120.811021;	 MAT_ELEM(angles, 1, 64) = -42.234673;
	MAT_ELEM(angles, 0, 65) = 95.188979;	 MAT_ELEM(angles, 1, 65) = -42.234673;
	MAT_ELEM(angles, 0, 66) = 126.466996;	 MAT_ELEM(angles, 1, 66) = -59.620797;
	MAT_ELEM(angles, 0, 67) = 108.000000;	 MAT_ELEM(angles, 1, 67) = -58.282544;
	MAT_ELEM(angles, 0, 68) = 89.533003;	 MAT_ELEM(angles, 1, 68) = -59.620797;
	MAT_ELEM(angles, 0, 69) = 108.000000;	 MAT_ELEM(angles, 1, 69) = 90.000000;
	MAT_ELEM(angles, 0, 70) = 116.867209;	 MAT_ELEM(angles, 1, 70) = -75.219088;
	MAT_ELEM(angles, 0, 71) = 99.132791;	 MAT_ELEM(angles, 1, 71) = -75.219088;
	MAT_ELEM(angles, 0, 72) = 36.000000;	 MAT_ELEM(angles, 1, 72) = -26.565058;
	MAT_ELEM(angles, 0, 73) = 48.811021;	 MAT_ELEM(angles, 1, 73) = -42.234673;
	MAT_ELEM(angles, 0, 74) = 23.188979;	 MAT_ELEM(angles, 1, 74) = -42.234673;
	MAT_ELEM(angles, 0, 75) = 54.466996;	 MAT_ELEM(angles, 1, 75) = -59.620797;
	MAT_ELEM(angles, 0, 76) = 36.000000;	 MAT_ELEM(angles, 1, 76) = -58.282544;
	MAT_ELEM(angles, 0, 77) = 17.533003;	 MAT_ELEM(angles, 1, 77) = -59.620797;
	MAT_ELEM(angles, 0, 78) = 36.000000;	 MAT_ELEM(angles, 1, 78) = 90.000000;
	MAT_ELEM(angles, 0, 79) = 44.867209;	 MAT_ELEM(angles, 1, 79) = -75.219088;
	MAT_ELEM(angles, 0, 80) = 27.132791;	 MAT_ELEM(angles, 1, 80) = -75.219088;
	}

	angles *= PI/180;

}

void ProgFSO::interpolationCoarse(MultidimArray< double > fsc,
		const Matrix2D<double> &angles,
		Matrix1D<double> &freq_fourier_x,
		Matrix1D<double> &freq_fourier_y,
		Matrix1D<double> &freq_fourier_z,
		MultidimArray<double> &threeD_FSC,
		MultidimArray<double> &counterMap,
		MultidimArray< double >& freqMap,
		MultidimArray< double >& freq,
		double maxFreq, int m1sizeX, int m1sizeY, int m1sizeZ,
		double rot, double tilt, double ang_con)
{
	int ZdimFT1=(int)ZSIZE(threeD_FSC);
	int YdimFT1=(int)YSIZE(threeD_FSC);
	int XdimFT1=(int)XSIZE(threeD_FSC);

	double x_dir, y_dir, z_dir, uz, uy, ux, cosAngle, aux;
	cosAngle = cos(ang_con);
	x_dir = sin(tilt*PI/180)*cos(rot*PI/180);
	y_dir = sin(tilt*PI/180)*sin(rot*PI/180);
	z_dir = cos(tilt*PI/180);
//		aux = 1.0/pow(ang_con,6);
	aux = 4.0/((cos(ang_con) -1)*(cos(ang_con) -1));
	long n = 0;
	for (int k=0; k<ZdimFT1; k++)
	{
		double uz = VEC_ELEM(freq_fourier_z,k);
		uz *= z_dir;
		for (int i=0; i<YdimFT1; i++)
		{
			double uy = VEC_ELEM(freq_fourier_y,i);
			uy *= y_dir;
			for (int j=0; j<XdimFT1; j++)
			{
				double ux = VEC_ELEM(freq_fourier_x,j);
				ux *= x_dir;
				double iun = DIRECT_MULTIDIM_ELEM(freqMap, n);
				double f = 1/iun;
				iun *= (ux + uy + uz);
				double cosine = fabs(iun);

				if (cosine>=cosAngle)
					{
					if (f>maxFreq)
					{
						++n;
						continue;
					}
					int idx = (int) round(f * m1sizeX);
					cosine = exp( -((cosine -1)*(cosine -1))*aux );
					DIRECT_MULTIDIM_ELEM(threeD_FSC, n) += cosine*dAi(fsc, idx);
					DIRECT_MULTIDIM_ELEM(counterMap, n) += cosine;
					}
				++n;
			}
		}
	}
}

//TODO: Merge with Simple
void ProgFSO::anistropyParameter(const MultidimArray<double> FSC,
		MultidimArray<double> &directionAnisotropy, size_t dirnumber,
		MultidimArray<double> &aniParam, double thrs)
{
	double N = 0;
	for (size_t k = 0; k<aniParam.nzyxdim; k++)
	{
		if (DIRECT_MULTIDIM_ELEM(FSC, k) >= thrs)
		{
			DIRECT_MULTIDIM_ELEM(aniParam, k) += 1.0;
			N++;
		}
	}
	DIRECT_MULTIDIM_ELEM(directionAnisotropy, dirnumber) = N;
}

void ProgFSO::anistropyParameterSimple(const MultidimArray<double> FSC,
		MultidimArray<double> &aniParam, double thrs)
{

	for (size_t k = 0; k<aniParam.nzyxdim; k++)
		if (DIRECT_MULTIDIM_ELEM(FSC, k) >= thrs)
			DIRECT_MULTIDIM_ELEM(aniParam, k) += 1.0;
}

void ProgFSO::prepareData(MultidimArray<double> &half1, MultidimArray<double> &half2, bool test)
{
	Image<double> mask;
	MultidimArray<double> &pmask = mask();

	if (test)
	{
		Monogenic mono;
		std::cout << "Preparing test data ..." << std::endl;
		size_t xdim = 256, ydim = 256, zdim = 256;
		double wavelength = 5.0, mean = 0.0, std = 0.5;
		int maskrad = 125;
		half1 = mono.createDataTest(xdim, ydim, zdim, wavelength, mean, 0.0);
		half2 = half1;

		mono.addNoise(half1, 0, std);
		mono.addNoise(half2, 0, std);
		FileName fn;
		Image<double> saveImg;
		fn = formatString("inputVol1_large.vol");
		saveImg() = half1;
		saveImg.write(fn);
		fn = formatString("inputVol2_large.vol");
		saveImg() = half2;
		saveImg.write(fn);
	}
	else
	{
		std::cout << "Reading data..." << std::endl;
		Image<double> imgHalf1, imgHalf2;
		imgHalf1.read(fnhalf1);
		imgHalf2.read(fnhalf2);

		half1 = imgHalf1();
		half2 = imgHalf2();

		if (fnmask!="")
		{
			mask.read(fnmask);
			FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(pmask)
			{
				double valmask = (double) DIRECT_MULTIDIM_ELEM(pmask, n);
				DIRECT_MULTIDIM_ELEM(half1, n) = DIRECT_MULTIDIM_ELEM(half1, n) * valmask;
				DIRECT_MULTIDIM_ELEM(half2, n) = DIRECT_MULTIDIM_ELEM(half2, n) * valmask;
			}
		}
		mask.clear();
		pmask.clear();
	}

	half1.setXmippOrigin();
	half2.setXmippOrigin();

	CenterFFT(half1, true);
	CenterFFT(half2, true);

}


void ProgFSO::saveFSCToMetadata(MetaData &mdRes,
		const MultidimArray<double> &freq,
		const MultidimArray<double> &FSC, FileName &fnmd)
{
	size_t id;
	FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
	{
		if (i>0)
		{
			id=mdRes.addObject();
			mdRes.setValue(MDL_RESOLUTION_FREQ,dAi(freq, i),id);
			mdRes.setValue(MDL_RESOLUTION_FRC,dAi(FSC, i),id);
			mdRes.setValue(MDL_RESOLUTION_FREQREAL,1./dAi(freq, i),id);
		}
	}
	mdRes.write(fnmd);
}

void ProgFSO::saveAnisotropyToMetadata(MetaData &mdAnisotropy,
		const MultidimArray<double> &freq,
		const MultidimArray<double> &anisotropy, FileName &fnmd)
{
	size_t objId;
	FOR_ALL_ELEMENTS_IN_ARRAY1D(anisotropy)
	{
		if (i>0)
		{
		objId = mdAnisotropy.addObject();
		mdAnisotropy.setValue(MDL_RESOLUTION_FREQ, dAi(freq, i),objId);
		mdAnisotropy.setValue(MDL_RESOLUTION_FRC, dAi(anisotropy, i),objId);
		mdAnisotropy.setValue(MDL_RESOLUTION_FREQREAL, 1.0/dAi(freq, i),objId);
		}
	}
	mdAnisotropy.write(fnmd);
}

    void ProgFSO::directionalFilter(MultidimArray<std::complex<double>> &FThalf1, 
			MultidimArray<double> &threeDfsc, 
			MultidimArray<double> &filteredMap, int m1sizeX, int m1sizeY, int m1sizeZ)
    {
    	Image<double> imgHalf1, imgHalf2;
    	imgHalf1.read(fnhalf1);
		imgHalf2.read(fnhalf2);
    	MultidimArray<double> half1, half2;
    	half1 = imgHalf1();
		half2 = imgHalf2();

        FourierTransformer transformer1(FFTW_BACKWARD);
        transformer1.FourierTransform(half1, FThalf1);//, false);
		FourierTransformer transformer2(FFTW_BACKWARD);
		MultidimArray<std::complex<double>> FThalf2;
		FThalf2.resizeNoCopy(FThalf1);
        transformer1.FourierTransform(half2, FThalf2, false);
    	FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(threeDfsc)
        {
    		DIRECT_MULTIDIM_ELEM(FThalf1, n) += DIRECT_MULTIDIM_ELEM(FThalf2, n);
			DIRECT_MULTIDIM_ELEM(FThalf1, n) *= DIRECT_MULTIDIM_ELEM(threeDfsc, n);
    	}
    	filteredMap.resizeNoCopy(m1sizeX, m1sizeY, m1sizeZ);
    	transformer1.inverseFourierTransform(FThalf1, filteredMap);
    }


    void ProgFSO::resolutionDistribution(MultidimArray<double> &resDirFSC, FileName &fn)
    {
    	Matrix2D<int> anglesResolution;
    	size_t Nrot = 360;
    	size_t Ntilt = 91;
    	size_t objIdOut;

    	MetaData mdOut;
    	Matrix2D<double> w, wt;
    	w.initZeros(Nrot, Ntilt);
    	wt = w;
    	float cosAngle = cosf(ang_con);
    	float aux = 4.0/((cosAngle -1)*(cosAngle -1));
    	// Directional resolution is store in a metadata

			
		for (int i=0; i<Nrot; i++)
		{
			double rotmatrix =  i*PI/180;

			for (int j=0; j<Ntilt; j++)
			{
				float tiltmatrix = j*PI/180;
				float xx = sinf(tiltmatrix)*cosf(rotmatrix);
				float yy = sinf(tiltmatrix)*sinf(rotmatrix);
				float zz = cosf(tiltmatrix);

				float w = 0;
				float wt = 0;

				for (size_t k = 0; k<angles.mdimx; k++)
				{

					double rot = MAT_ELEM(angles, 0, k);
					double tilt = MAT_ELEM(angles, 1, k);

					double x_dir = sin(tilt)*cos(rot);
					double y_dir = sin(tilt)*sin(rot);
					double z_dir = cos(tilt);


					float cosine = fabs(x_dir*xx + y_dir*yy + z_dir*zz);
					if (cosine>=cosAngle)
					{
						cosine = exp( -((cosine -1)*(cosine -1))*aux );
						w += cosine*( dAi(resDirFSC, k) );
						wt += cosine;
					}
				}

			double wRes = w/wt;
			objIdOut = mdOut.addObject();
			mdOut.setValue(MDL_ANGLE_ROT, (double) i, objIdOut);
			mdOut.setValue(MDL_ANGLE_TILT, (double) j, objIdOut);
			mdOut.setValue(MDL_RESOLUTION_FRC, wRes, objIdOut);
			}
		}

		mdOut.write(fn);
    }



    void ProgFSO::getCompleteFourier(MultidimArray<double> &V, MultidimArray<double> &newV,
    		int m1sizeX, int m1sizeY, int m1sizeZ)
        {
    	newV.resizeNoCopy(m1sizeX, m1sizeY, m1sizeZ);
		int ndim=3;
		if (m1sizeX==1)
		{
			ndim=2;
			if (m1sizeY==1)
				ndim=1;
		}
		double *ptrSource=NULL;
		double *ptrDest=NULL;
		FOR_ALL_DIRECT_ELEMENTS_IN_ARRAY3D(newV)
		{
			ptrDest=(double*)&DIRECT_A3D_ELEM(newV,k,i,j);
			if (j<XSIZE(V))
			{
				ptrSource=(double*)&DIRECT_A3D_ELEM(V,k,i,j);
				*ptrDest=*ptrSource;
//				*(ptrDest+1)=*(ptrSource+1);
			}
			else
			{
				ptrSource=(double*)&DIRECT_A3D_ELEM(V,
													(m1sizeZ-k)%m1sizeZ,
													(m1sizeY-i)%m1sizeY,
													m1sizeX-j);
				*ptrDest=*ptrSource;
			}
		}
   }


    void ProgFSO::createFullFourier(MultidimArray<double> &fourierHalf, FileName &fnMap,
    		int m1sizeX, int m1sizeY, int m1sizeZ)
    {
    	MultidimArray<double> fullMap;
		getCompleteFourier(fourierHalf, fullMap, m1sizeX, m1sizeY, m1sizeZ);
		CenterFFT(fullMap, true);
		Image<double> saveImg;
		saveImg() = fullMap;
	    saveImg.write(fnMap);
    }

	void ProgFSO::run()
	{
		run_fast();
		//run_old();
	}


	void ProgFSO::run_fast()
	{
		std::cout << "Starting ... " << std::endl;
		std::cout << " " << std::endl;
		
		MultidimArray<double> half1, half2;
    	MultidimArray<double> &phalf1 = half1, &phalf2 = half2;

		//This read the data and applies a fftshift to in the next step compute the fft
		// test = true launches the unitary test
    	prepareData(half1, half2, test);

		//Computing the FFT
		FourierTransformer transformer2(FFTW_BACKWARD), transformer1(FFTW_BACKWARD);
		transformer1.setThreadsNumber(Nthreads);
		transformer2.setThreadsNumber(Nthreads);

        transformer1.FourierTransform(half1, FT1, false);
     	transformer2.FourierTransform(half2, FT2, false);
        
		// Defining frequencies freq_fourier_x,y,z and freqMap
		// The number of frequencies in each shell freqElem is determined
		// The number of accesible frequencies Ncomps and the size xvoldim
        defineFrequencies(FT1, phalf1);

		half1.clear();
		half2.clear();

		// Storing the shell of both maps as vectors global
		// The global FSC is also computed
		MultidimArray<double> fscglobal, freqglobal, freq;
		double resol, resInterp;
		arrangeFSC_and_fscGlobal(sampling, resol, thrs, resInterp, freq);
		std::cout << "Resolution FSC at 0.143 = " << resol << " " << resInterp << std::endl;
		std::cout << " " << std::endl;

		FT2.clear();
		// Determining best cone Angle
		// bestAngleFunction();


		// Generiting the set of directions to be analyzed
		// And converting the cone angle to radians
    	generateDirections(angles, true);
    	ang_con = ang_con*PI/180;

    	
		// Preparing the metadata for storing the FSO
		MultidimArray<double> directionAnisotropy(angles.mdimx), resDirFSC(angles.mdimx), aniParam;
    	aniParam.initZeros(xvoldim/2+1);
    	MetaData mdAnisotropy;


		// Computing directional FSC and 3DFSC
		MultidimArray<double> fsc, threeD_FSC, normalizationMap;
		threeD_FSC.resizeNoCopy(FT1_vec);
		threeD_FSC.initZeros();
		normalizationMap.resizeNoCopy(FT1_vec);
		normalizationMap.initZeros();
		
		
		thrs = 0.143;

    	for (size_t k = 0; k<angles.mdimx; k++)
		{
			// size_t k = 3;
			double rot  = MAT_ELEM(angles, 0, k);
			double tilt = MAT_ELEM(angles, 1, k);

			// Estimating the direction FSC along the diretion given by rot and tilt
			MetaData mdDirFSC;
			fscDir_fast(fsc, rot, tilt, mdDirFSC, threeD_FSC, normalizationMap, resol, thrs, resInterp, k);

			std::cout << "Direction " << k << "/" << angles.mdimx << " resolution = " << resInterp << std::endl;



			dAi(resDirFSC, k) = resInterp;
			
			// Updating the FSO curve
			anistropyParameter(fsc, directionAnisotropy, k, aniParam, thrs);
		}

		std::cout << "----- Directional resolution estimated -----" <<  std::endl;
    	std::cout << "   " <<  std::endl;
    	std::cout << "Preparing results ..." <<  std::endl;

		

    	// ANISOTROPY CURVE
    	aniParam /= (double) angles.mdimx;
    	MetaData mdani;
		saveAnisotropyToMetadata(mdani, freq, aniParam, fn_ani);

		// HALF 3DFSC MAP
		// TODO: do this step before
		MultidimArray<double> d3_FSCMap;
		d3_FSCMap.resizeNoCopy(FT1);
		d3_FSCMap.initConstant(0);

		FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(threeD_FSC)
		{
				double value = DIRECT_MULTIDIM_ELEM(threeD_FSC, n) /= DIRECT_MULTIDIM_ELEM(normalizationMap, n);
				if (std::isnan(value) == 1)
			 		value = 1.0;
				size_t idx = DIRECT_MULTIDIM_ELEM(arr2indx, n);
				DIRECT_MULTIDIM_ELEM(d3_FSCMap, idx) = value;
		}
		// std::cout << "llego1"<< std::endl;
		// MultidimArray<double> filteredMap;
        // filteredMap.resizeNoCopy(xvoldim, yvoldim, zvoldim);
    	// transformer1.inverseFourierTransform(FT1, filteredMap);
		// CenterFFT(filteredMap, false);

		// This code fix the empty line line in Fourier space
		//TODO enhance performance
		size_t auxVal;
		auxVal = YSIZE(d3_FSCMap)/2;
    	
		size_t j = 0;
		for(size_t i=0; i<YSIZE(d3_FSCMap); ++i)
		{
			if (i>auxVal)
			{
				for(size_t k=0; k<ZSIZE(d3_FSCMap); ++k)
				{
						DIRECT_A3D_ELEM(d3_FSCMap,k,i,j) = DIRECT_A3D_ELEM(d3_FSCMap,k,i,j+1);
				}
			}
		}
		auto clockstarts = std::chrono::high_resolution_clock::now();
		//TODO: directionalFilter reads the original map, avoid that
		// DIRECTIONAL FILTERED MAP
		MultidimArray<double> filteredMap;
		directionalFilter(FT1, d3_FSCMap, filteredMap, xvoldim, yvoldim, zvoldim);
		Image<double> saveImg2;
		saveImg2() = filteredMap;
		saveImg2.write(fn_fscmd_folder+"filteredMap.mrc");
		//FULL 3DFSC MAP
		FileName fn;
		createFullFourier(d3_FSCMap, fn_3dfsc, xvoldim, yvoldim, zvoldim);
		
		auto clockends = std::chrono::high_resolution_clock::now(); 

		std::cout << "time = " << std::chrono::duration_cast<std::chrono::microseconds>(clockstarts-clockends).count()  << std::endl;
		// DIRECTIONAL RESOLUTION DISTRIBUTION
		fn = fn_fscmd_folder+"Resolution_Distribution.xmd";
		resolutionDistribution(resDirFSC, fn);

		
		std::cout << "-------------Finished-------------" << std::endl;

	}

/*
			






    	// MultidimArray<double> fsc, freq, counterMap, threeD_FSC, aniParam;
    		
		// counterMap.resizeNoCopy(FT1);
    	// threeD_FSC.resizeNoCopy(counterMap);
    	// threeD_FSC.initZeros();
    	// counterMap.initConstant(1e-38);

    	// double cutoff;
    	// cutoff = sampling/resol;

	}
*/


    void ProgFSO::run_old()
    {
		std::cout << "Starting ... " << std::endl;
		std::cout << " " << std::endl;

    	if (ang_con == -1)
		{
	   		doCrossValidation = true;
	    	std::cout << "The best cone angle will be estimated " << std::endl;
		}
		else
		{
	    	doCrossValidation = false;
	    	std::cout << "The chosen cone angle is " << ang_con << std::endl;
		}

    	MultidimArray<double> half1, half2;
    	MultidimArray<double> &phalf1 = half1, &phalf2 = half2;

		//This creates the FFT of both halves or launches a test if test is true
    	prepareData(half1, half2, test);
    	int m1sizeX = XSIZE(phalf1), m1sizeY = YSIZE(phalf1), m1sizeZ = ZSIZE(phalf1);
		


		FourierTransformer transformer2(FFTW_BACKWARD), transformer1(FFTW_BACKWARD);
		// transformer1.setThreadsNumber(Nthreads);
		// transformer2.setThreadsNumber(Nthreads);

        transformer1.FourierTransform(phalf1, FT1, false);
     	transformer2.FourierTransform(phalf2, FT2, false);

        //Defining frequencies
        defineFrequencies(FT1, phalf1);

    	MultidimArray<double> fsc, freq, counterMap, threeD_FSC, aniParam;
    	counterMap.resizeNoCopy(FT1);
    	threeD_FSC.resizeNoCopy(counterMap);
    	threeD_FSC.initZeros();
    	counterMap.initConstant(1e-38);

    	MetaData mdFSC, mdFSC2;
    	MultidimArray<double> fscglobal, freqglobal;
    	double resol, resInterp;

		// arrangeFSC_and_fscGlobal(sampling, freqglobal, fscglobal,mdFSC, resol, thrs, resInterp);
		// fscDir_fast(freqglobal, fscglobal,mdFSC2, resol, thrs, resInterp);


    	fscGlobal(sampling, freqglobal, fscglobal, 0.5,
				m1sizeX, m1sizeY, m1sizeZ, mdFSC, resol, thrs, resInterp);

    	std::cout << "Resolution FSC at 0.143 = " << resol << " " << resInterp << std::endl;

    	double cutoff;
    	cutoff = sampling/resol;
    	Matrix2D<double> indexesFourier;
		Matrix2D<int> indexesFourier2;
    	MultidimArray<std::complex<double>> f1, f2;
    	fscShell(FT1, FT2, freq_fourier_x, freq_fourier_y, freq_fourier_z, freqMap, m1sizeX, 
				indexesFourier, indexesFourier2, cutoff, f1, f2);

		//Ready to cross validate
		findBestConeAngle(indexesFourier2, resol);
		std::cout << "best angle estimated"<< std::endl;
		exit(0);
		
		

    	double dresfsc, lastcross = 1e38;
    	FileName fnmd;
    	if (doCrossValidation)
		{
    		size_t count = 0;
    		generateDirections(angles, true);
			double angCon, cross;
			size_t objId;
			MetaData mdcrossval;
            //TODO: Avoid double for loop
			for (double myangle = 1; myangle <41; myangle = myangle + 1)
			{
				angCon = myangle*PI/180.0;
				cross = 0;
				aniParam.initZeros(m1sizeX/2+1);
				//std::cout << "A_"<<myangle<<"=[" << std::endl;
				for (size_t k = 0; k<angles.mdimx; ++k)
				{
					// size_t k = 0;
					double rot = MAT_ELEM(angles, 0, k);
					double tilt = MAT_ELEM(angles, 1, k);
					std::cout << (int) myangle << " " << (int) k+1 << " ";

					std::cout << sin(tilt*PI/180)*cos(rot*PI/180) << "  " << sin(tilt*PI/180)*sin(rot*PI/180)<< "  " << cos(tilt*PI/180) << std::endl;

					weights(cutoff, indexesFourier, indexesFourier2, rot, tilt, myangle,
									f1, f2, FT1, FT2, freq_fourier_x, freq_fourier_y, freq_fourier_z, cross);
				}
				std::cout << "angle = " << myangle << std::endl;
				objId = mdcrossval.addObject();
				mdcrossval.setValue(MDL_ANGLE_Y, myangle, objId);
				mdcrossval.setValue(MDL_SUM, cross, objId);
				if (cross<lastcross)
				{
					lastcross = cross;
					ang_con = myangle;

				}
				//std::cout << "];" << std::endl;
				std::cout << "---------------------------" << std::endl;

			}
			mdcrossval.write(fn_fscmd_folder+"crossValidation.xmd");
			std::cout << "The best cone angle is " << ang_con << std::endl;
		}
   		

    	std::cout << "                       " << std::endl;
    	generateDirections(angles, true);
    	ang_con = ang_con*PI/180;

    	MultidimArray<double> directionAnisotropy(angles.mdimx), resDirFSC(angles.mdimx);;
    	aniParam.initZeros(m1sizeX/2+1);
    	MetaData mdAnisotropy;

		thrs = 0.143;

    	size_t objId;
    	for (size_t k = 0; k<angles.mdimx; k++)
			{
			double rot = MAT_ELEM(angles, 0, k);
			double tilt = MAT_ELEM(angles, 1, k);

//			std::cout << "Direction " << k << "  rot " << rot << "  " << tilt << std::endl;
//			std::cout << rot << "  " << tilt << ";" << std::endl;

			fscDir(FT1, FT2, sampling, freq_fourier_x, freq_fourier_y, freq_fourier_z, freqMap, freq, fsc, 0.5,
					m1sizeX, m1sizeY, m1sizeZ, rot, tilt, ang_con, dresfsc, thrs);

			dAi(resDirFSC, k) = dresfsc;

			std::cout << "directional resolution = " << dresfsc << std::endl;

			MetaData mdRes;
			fnmd = formatString("fscDirection_%i.xmd", k);
			fnmd = fn_fscmd_folder + fnmd;
			saveFSCToMetadata(mdRes, freq, fsc, fnmd);

			anistropyParameter(fsc, directionAnisotropy, k, aniParam, thrs);

			//TODO: integrate this function in FSCDIR
			interpolationCoarse(fsc, angles,
					freq_fourier_x, freq_fourier_y, freq_fourier_z,
		    		threeD_FSC, counterMap,
					freqMap, freq,
					0.5, m1sizeX, m1sizeY, m1sizeZ,
					rot, tilt, ang_con);
    	}

    	std::cout << "----- Directional resolution estimated -----" <<  std::endl;
    	std::cout << "   " <<  std::endl;
    	std::cout << "Preparing results ..." <<  std::endl;

    	//ANISOTROPY CURVE
    	aniParam /= (double) angles.mdimx;
    	MetaData mdani;
		saveAnisotropyToMetadata(mdani, freq, aniParam, fn_ani);

		//HALF 3DFSC MAP
		FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(threeD_FSC)
		{
			DIRECT_MULTIDIM_ELEM(threeD_FSC, n) /= DIRECT_MULTIDIM_ELEM(counterMap, n);
			if (std::isnan(DIRECT_MULTIDIM_ELEM(threeD_FSC, n)) == 1)
				DIRECT_MULTIDIM_ELEM(threeD_FSC, n) = 1.0;
		}



		//This code fix the empty line line in Fourier space
		size_t auxVal;
		auxVal = YSIZE(threeD_FSC)/2;
    	long n=0;
    	for(size_t k=0; k<ZSIZE(threeD_FSC); ++k)
    	{
    		for(size_t i=0; i<YSIZE(threeD_FSC); ++i)
    		{
    			for(size_t j=0; j<XSIZE(threeD_FSC); ++j)
    			{
    				if ((j == 0) && (i>auxVal))
    					{
    					DIRECT_A3D_ELEM(threeD_FSC,k,i,j) = DIRECT_A3D_ELEM(threeD_FSC,k,i,j+1);
    					}
   					++n;
    			}
    		}
    	}

		//DIRECTIONAL FILTERED MAP
		MultidimArray<double> filteredMap;
		directionalFilter(FT1, threeD_FSC, filteredMap, m1sizeX, m1sizeY, m1sizeZ);

		Image<double> saveImg2;
		saveImg2() = filteredMap;
		saveImg2.write(fn_fscmd_folder+"filteredMap.mrc");

		//FULL 3DFSC MAP
		FileName fn;
		createFullFourier(threeD_FSC, fn_3dfsc, m1sizeX, m1sizeY, m1sizeZ);

		//SPHERE FREQUENCY REFERENCE
		MultidimArray<double> sphere;
		sphere.resizeNoCopy(counterMap);
		createfrequencySphere(sphere,
				freq_fourier_x, freq_fourier_y, freq_fourier_z);

		fn = fn_fscmd_folder+"sphere.mrc";
		//TODO: Include this in the frequency creation
		createFullFourier(sphere, fn, m1sizeX, m1sizeY, m1sizeZ);

		//DIRECTIONAL RESOLUTION DISTRIBUTION
		fn = fn_fscmd_folder+"Resolution_Distribution.xmd";
		resolutionDistribution(resDirFSC, fn);

		std::cout << "-------------Finished-------------" << std::endl;
    }



void ProgFSO::findBestConeAngle(Matrix2D<int> &fscShell, double resolutionfsc)
{
	//TODO: It is faster work in Fourier
	Image<double> imgh1, imgh2;
	std::cout << "alla " << std::endl;
	imgh1.read(fnhalf1);
	imgh2.read(fnhalf2);
	std::cout << "aqui " << std::endl;
	MultidimArray<double> half1, half2;
	half1 = imgh1();
	half2 = imgh2();

	MultidimArray<double> noise, signal;
	noise = 1/sqrt(2)*(half1 - half2);

	Image<double> imgsave;
	imgsave() = noise;
	imgsave.write("noise_original.mrc");


	Image<double> immask;
	MultidimArray<double> mask;
	
	// TODO: Take the real mask
	immask.read(fnmask);
	mask = immask();
	
	double mean, stddev;
	noiseStatisticsInMask(noise, mask, mean, stddev);

	std::cout << "mean = " << mean << "   " << stddev << std::endl;

	noise.initZeros();
	createNoisyMap(noise, mean, stddev);

	//Determining the shell number
	double freqdig_fsc = sampling/resolutionfsc;
	int idx_noise = (int) round(freqdig_fsc * xvoldim);

	MultidimArray<std::complex<double>> FTnoise, FTmap;
	FourierTransformer transformer(FFTW_BACKWARD);
	transformer.FourierTransform(noise, FTnoise, false);

	double powernoise = 0;
	long m = 0;
	FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(FTnoise)
	{
		double f = 1/DIRECT_MULTIDIM_ELEM(freqMap,n);
		int idx = (int) round(f * xvoldim);
		if (idx_noise == idx)
		{
			std::complex<double> z = DIRECT_MULTIDIM_ELEM(FTnoise,n);
			powernoise += fabs(z)*fabs(z);
			++m;
		}
	}

	powernoise = sqrt(powernoise)/(0.5*XSIZE(noise)*YSIZE(noise)*ZSIZE(noise));
	std::cout << idx_noise  << "noise power = " << powernoise << std::endl;

	MultidimArray<double> map;
	map.resizeNoCopy(noise);

	createNoisyFringePattern(map, noise, mask, powernoise, resolutionfsc);

	FourierTransformer transformer2(FFTW_BACKWARD);
	transformer.FourierTransform(map, FTmap, false);

	MultidimArray<std::complex<double>> shellSignal(m);
	MultidimArray<size_t> xidx(m);
	MultidimArray<size_t> yidx(m);
	MultidimArray<size_t> zidx(m);

	double powersignal = 0, cosAngle, aux0, wt;
	double num = 0;
	cosAngle = 0;
	aux0 = 4.0/((cosAngle -1)*(cosAngle -1));
	//powernoise = 0;
	MultidimArray<double> aux;
	aux.resizeNoCopy(FTmap);
	aux.initZeros();
	long n=0;
	long nn = 0;
	for(size_t k=0; k<ZSIZE(FTmap); ++k)
    {
		for(size_t i=0; i<YSIZE(FTmap); ++i)
		{
			for(size_t j=0; j<XSIZE(FTmap); ++j)
			{
				std::complex<double> zs = DIRECT_MULTIDIM_ELEM(FTmap,n);
				DIRECT_MULTIDIM_ELEM(aux,n) = real(zs);
				double f = 1/DIRECT_MULTIDIM_ELEM(freqMap,n);
				int idx = (int) round(f * xvoldim);
				if (idx_noise == idx)
				{
					
					std::complex<double> zn = DIRECT_MULTIDIM_ELEM(FTnoise,n);
					DIRECT_MULTIDIM_ELEM(shellSignal,nn) = zs;

					double uy = VEC_ELEM(freq_fourier_y, i);
					double ux = VEC_ELEM(freq_fourier_x, j);
					double uz = VEC_ELEM(freq_fourier_z, k);

					double cosine = fabs(ux/(sqrt(ux*ux + uy*uy + uz*uz)));

					// if (cosine >= cosAngle)
					// {
					// 	cosine = exp( -((cosine -1)*(cosine -1))*aux0 );
					// 	double coeff = fabs(DIRECT_MULTIDIM_ELEM(shellSignal,elm))*fabs(DIRECT_MULTIDIM_ELEM(shellSignal,elm));
					// 	num += coeff;
					// 	num += cosine*coeff;	
					// 	wt += cosine;
					// }
					
					DIRECT_MULTIDIM_ELEM(xidx,nn) = j;
					DIRECT_MULTIDIM_ELEM(yidx,nn) = i;
					DIRECT_MULTIDIM_ELEM(zidx,nn) = k;
					// std::complex<double> zs = DIRECT_MULTIDIM_ELEM(FTmap,n);
					powernoise += fabs(zn)*fabs(zn);
					double coeff = fabs(zs)*fabs(zs);
					powersignal += coeff;
					std::cout << coeff << "  " << acos(cosine)*180/PI << ";" << std::endl;
					// std::cout << fabs(zs)*fabs(zs) << " " << fabs(zn)*fabs(zn) << ";" << std::endl;

					++nn;
				}
				++n;
			}
		}
	}

	// powersignal = num/wt;
	
	Image<double> img;
	img() = aux;
	img.write("TFmap.mrc");
	std::cout << " signal " << log(powersignal) << " " << log(powernoise) << std::endl;

	
	size_t objId;
	MetaData mdcv;
	//cross validation
	for (size_t myangle = 2; myangle<90; ++myangle)
	{
		cosAngle = cos(myangle*PI/180);
		aux0 = 4.0/((cosAngle -1)*(cosAngle -1));
		wt = 0;
		num = 0;
		for (size_t elm = 0; elm<m; ++elm)
		{
			double uy = VEC_ELEM(freq_fourier_y, DIRECT_MULTIDIM_ELEM(yidx,elm));
			double ux = VEC_ELEM(freq_fourier_x, DIRECT_MULTIDIM_ELEM(xidx,elm));
			double uz = VEC_ELEM(freq_fourier_z, DIRECT_MULTIDIM_ELEM(zidx,elm));

			double cosine = fabs(ux/(sqrt(ux*ux + uy*uy + uz*uz)));

			if (cosine >= cosAngle)
			{
				// cosine = exp( -((cosine -1)*(cosine -1))*aux0 );
				double coeff = fabs(DIRECT_MULTIDIM_ELEM(shellSignal,elm))*fabs(DIRECT_MULTIDIM_ELEM(shellSignal,elm));
				num += coeff;
				// num += cosine*coeff;	
				// wt += cosine;
				
			}
			// std::cout << "elm" << elm << std::endl;
		}
		std::cout << myangle << std::endl;
		objId = mdcv.addObject();
		
		wt = 1;
		mdcv.setValue(MDL_ANGLE_Y, (double) myangle, objId);
		mdcv.setValue(MDL_SUM, num/wt, objId);
		std::cout << myangle << std::endl;
	}
	mdcv.write(fn_fscmd_folder+"crossValidation.xmd");


}

void ProgFSO::noiseStatisticsInMask(MultidimArray<double> &map, MultidimArray<double> &mask,
 									double &mean, double &stdev)
{
	double count = 0;
	double sum = 0;
	double sum2 = 0;


	FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(map)
	{
		if ( DIRECT_MULTIDIM_ELEM(mask, n) > 0.5)
		{
			double value = DIRECT_MULTIDIM_ELEM(map, n);
			sum += value;
			sum2 += value*value;
			++count;
		}
			
	}

	mean = sum/count;
	stdev = sqrt(sum2/count - mean*mean);

}

void ProgFSO::createNoisyMap(MultidimArray<double> &map, double mean, double stddev)
{
	Monogenic mono;
	mono.addNoise(map, mean, stddev);

	Image<double> img;
	img() = map;
	img.write("noise_created.mrc");

}


void ProgFSO::createNoisyFringePattern(MultidimArray<double> &map, MultidimArray<double> &noise,
									 MultidimArray<double> &mask,
									 double sqrtpowernoise, double wavelength)
{
	for(size_t k=0; k<ZSIZE(noise); ++k)
	{
		for(size_t i=0; i<YSIZE(noise); ++i)
		{
			for(size_t j=0; j<XSIZE(noise); ++j)
			{
				DIRECT_A3D_ELEM(map,k,i,j) = (sqrtpowernoise*cos(2*PI*sampling/(wavelength)*j) + DIRECT_A3D_ELEM(noise, k,i,j))*DIRECT_A3D_ELEM(mask, k,i,j);
			}
		}
	}

	Image<double> img;
	img() = map;
	img.write("half_fringed.mrc");

}


//TODO: Used but to check if usefull

void ProgFSO::estimateSSNR(MultidimArray<double> &half1, MultidimArray<double> &half2,
		int m1sizeX, int m1sizeY, int m1sizeZ)
{
	MultidimArray<double> noise, signal;
	signal = half1 + half2;
	noise = half1 - half2;
	FourierTransformer signaltransformer(FFTW_BACKWARD), noisetransformer(FFTW_BACKWARD);
	MultidimArray< std::complex< double > > FTsignal, FTnoise;

	signaltransformer.FourierTransform(signal, FTsignal, false);
	noisetransformer.FourierTransform(noise, FTnoise, false);

	MultidimArray<double> noisePower, signalPower, SSNRMap;
	SSNRMap.initZeros(FTsignal);
	noisePower.resizeNoCopy(SSNRMap);
	signalPower.resizeNoCopy(SSNRMap);

	FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(signaltransformer.fFourier)
	{
		double sabs = abs(DIRECT_MULTIDIM_ELEM(FTsignal, n));
		double nabs = abs(DIRECT_MULTIDIM_ELEM(FTnoise, n));
		sabs *= sabs;
		nabs *= nabs;
		DIRECT_MULTIDIM_ELEM(noisePower, n) = log(nabs);
		DIRECT_MULTIDIM_ELEM(signalPower, n) = log(sabs);
		DIRECT_MULTIDIM_ELEM(SSNRMap, n) = log(sabs/nabs);
	}

	Image<double> iim;
	iim() = SSNRMap;
	iim.write("ssNR.mrc");
	FileName fn;
	fn = fn_fscmd_folder+"ssnrMap.mrc";
	createFullFourier(SSNRMap, fn, m1sizeX, m1sizeY, m1sizeZ);

	fn = fn_fscmd_folder+"signalPower.mrc";
	createFullFourier(signalPower, fn, m1sizeX, m1sizeY, m1sizeZ);

	fn = fn_fscmd_folder+"noisePower.mrc";
	createFullFourier(noisePower, fn, m1sizeX, m1sizeY, m1sizeZ);
}


void ProgFSO::directionalSSNR(MultidimArray< std::complex< double > > & FT1,
					 MultidimArray< std::complex< double > > & FT2, double sampling_rate,
					 Matrix1D<double> &freq_fourier_x,
					 Matrix1D<double> &freq_fourier_y,
					 Matrix1D<double> &freq_fourier_z,
					 MultidimArray< double >& freqMap, MultidimArray< double >& sig,
					 MultidimArray< double >& noi,
					 double maxFreq, int m1sizeX, int m1sizeY, int m1sizeZ,
					 double rot, double tilt, double ang_con, size_t dire)
{
	MultidimArray< int > radial_count(m1sizeX/2+1);
	MultidimArray<double> freq, counter, z1r, z1i, z2r, z2i;

	z1r.initZeros(radial_count);
	z1i.initZeros(radial_count);
	z2r.initZeros(radial_count);
	z2i.initZeros(radial_count);
	counter.initZeros(radial_count);

	freq.initZeros(radial_count);

	int ZdimFT1=(int)ZSIZE(FT1);
	int YdimFT1=(int)YSIZE(FT1);
	int XdimFT1=(int)XSIZE(FT1);

	double x_dir, y_dir, z_dir, uz, uy, ux, cosAngle, aux;
	x_dir = sin(tilt*PI/180)*cos(rot*PI/180);
	y_dir = sin(tilt*PI/180)*sin(rot*PI/180);
	z_dir = cos(tilt*PI/180);
	cosAngle = cos(ang_con);
	aux = 4.0/((cos(ang_con) -1)*(cos(ang_con) -1));
	long n = 0;
	for (int k=0; k<ZdimFT1; k++)
	{
		double uz = VEC_ELEM(freq_fourier_z,k);
		uz *= z_dir;
		for (int i=0; i<YdimFT1; i++)
		{
			double uy = VEC_ELEM(freq_fourier_y,i);
			uy *= y_dir;
			for (int j=0; j<XdimFT1; j++)
			{
				double ux = VEC_ELEM(freq_fourier_x,j);
				ux *= x_dir;
				double iun = DIRECT_MULTIDIM_ELEM(freqMap,n);
				double f = 1/iun;
				iun *= (ux + uy + uz);

				double cosine = fabs(iun);
				++n;

				if (cosine>=cosAngle)
					{
						if (f>maxFreq)
							continue;

						int idx = (int) round(f * m1sizeX);
						std::complex<double> &z1 = dAkij(FT1, k, i, j);
						std::complex<double> &z2 = dAkij(FT2, k, i, j);
						dAi(z1r,idx) += real(z1);
						dAi(z1i,idx) += imag(z1);
						dAi(z2r,idx) += real(z2);
						dAi(z2i,idx) += imag(z2);
						dAi(counter,idx) += 1.0;
					}
			}
		}
	}

	FOR_ALL_ELEMENTS_IN_ARRAY1D(sig)
	{
		dAi(z1r,i) = dAi(z1r,i)/dAi(counter,i);
		dAi(z1i,i) = dAi(z1i,i)/dAi(counter,i);
		dAi(z2r,i) = dAi(z2r,i)/dAi(counter,i);
		dAi(z2i,i) = dAi(z2i,i)/dAi(counter,i);
	}

	MetaData mdRes;
	size_t id;
	FOR_ALL_ELEMENTS_IN_ARRAY1D(freq)
	{
		if (i>0)
		{
			id=mdRes.addObject();
			dAi(freq,i) = (float) i / (m1sizeX * sampling_rate);
			mdRes.setValue(MDL_RESOLUTION_FREQ,dAi(freq, i),id);
			mdRes.setValue(MDL_VOLUME_SCORE1, dAi(z1r, i),id);
			mdRes.setValue(MDL_VOLUME_SCORE2, dAi(z1i, i),id);
			mdRes.setValue(MDL_VOLUME_SCORE3, dAi(z2r, i),id);
			mdRes.setValue(MDL_VOLUME_SCORE4, dAi(z2i, i),id);
		}
	}

	FileName fnmd;
	fnmd = fn_fscmd_folder + formatString("ssnr_%i.xmd", dire);
	mdRes.write(fnmd);
}

// void ProgFSO::testDataFSO(MultidimArray<double> &half1, MultidimArray<double> &half2)
// {
// 	half1.init
// }



