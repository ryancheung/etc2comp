/*
 * Copyright 2015 The Etc2Comp Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "EtcConfig.h"
#include "Etc.h"
#include "EtcFilter.h"

#include <string.h>

void Etc2FreeData(unsigned char* data)
{
	delete[] data;
}

void Etc2Encode(unsigned char*a_pafSourceRGBA,
	unsigned int a_uiSourceWidth,
	unsigned int a_uiSourceHeight,
	Etc::Image::Format a_format,
	unsigned char **a_ppaucEncodingBits,
	unsigned int *a_puiEncodingBitsBytes,
	unsigned int *a_puiExtendedWidth,
	unsigned int *a_puiExtendedHeight,
	int *a_piEncodingTime_ms)
{
	Etc::ColorFloatRGBA *m_pafrgbaPixels;
	m_pafrgbaPixels = new Etc::ColorFloatRGBA[a_uiSourceWidth * a_uiSourceHeight];

	bool bool16BitImage = false;
	int iBytesPerPixel = bool16BitImage ? 8 : 4;
	unsigned char *pucPixel;
	Etc::ColorFloatRGBA *pfrgbaPixel = m_pafrgbaPixels;

	int iBlockX = 0;
	int iBlockY = 0;
	unsigned int iWidth = a_uiSourceWidth;
	unsigned int iHeight = a_uiSourceHeight;
	unsigned int m_uiWidth = iWidth;
	unsigned int m_uiHeight = iHeight;

	// convert pixels from RGBA* to ColorFloatRGBA
	for (unsigned int uiV = iBlockY; uiV < (iBlockY + m_uiHeight); ++uiV)
	{
		// reset coordinate for each row
		pucPixel = &a_pafSourceRGBA[(uiV * iWidth + iBlockX) * iBytesPerPixel];

		// read each row
		for (unsigned int uiH = iBlockX; uiH < (iBlockX + m_uiWidth); ++uiH)
		{
			if (bool16BitImage)
			{
				unsigned short ushR = (pucPixel[0] << 8) + pucPixel[1];
				unsigned short ushG = (pucPixel[2] << 8) + pucPixel[3];
				unsigned short ushB = (pucPixel[4] << 8) + pucPixel[5];
				unsigned short ushA = (pucPixel[6] << 8) + pucPixel[7];

				*pfrgbaPixel++ = Etc::ColorFloatRGBA((float)ushR / 65535.0f,
					(float)ushG / 65535.0f,
					(float)ushB / 65535.0f,
					(float)ushA / 65535.0f);
			}
			else
			{
				*pfrgbaPixel++ = Etc::ColorFloatRGBA::ConvertFromRGBA8(pucPixel[0], pucPixel[1],
					pucPixel[2], pucPixel[3]);
			}

			pucPixel += iBytesPerPixel;
		}
	}

	Etc2Encode((float*)m_pafrgbaPixels,
		a_uiSourceWidth,
		a_uiSourceHeight,
		a_format,
		a_ppaucEncodingBits,
		a_puiEncodingBitsBytes,
		a_puiExtendedWidth,
		a_puiExtendedHeight,
		a_piEncodingTime_ms);

	delete[] m_pafrgbaPixels;
}

void Etc2Encode(float *a_pafSourceRGBA,
	unsigned int a_uiSourceWidth,
	unsigned int a_uiSourceHeight,
	Etc::Image::Format a_format,
	unsigned char **a_ppaucEncodingBits,
	unsigned int *a_puiEncodingBitsBytes,
	unsigned int *a_puiExtendedWidth,
	unsigned int *a_puiExtendedHeight,
	int *a_piEncodingTime_ms)
{
	Etc::ErrorMetric a_eErrMetric = Etc::ErrorMetric::RGBA;
	float a_fEffort = ETCCOMP_MIN_EFFORT_LEVEL;

	unsigned int a_uiJobs = 20;
	unsigned int a_uiMaxJobs = 1024;
	bool a_bVerboseOutput = false;

	Etc2Encode(a_pafSourceRGBA,
		a_uiSourceWidth,
		a_uiSourceHeight,
		a_format,
		a_eErrMetric,
		a_fEffort,
		a_uiJobs,
		a_uiMaxJobs,
		a_ppaucEncodingBits,
		a_puiEncodingBitsBytes,
		a_puiExtendedWidth,
		a_puiExtendedHeight,
		a_piEncodingTime_ms,
		a_bVerboseOutput);
}

void Etc2Encode(float *a_pafSourceRGBA,
	unsigned int a_uiSourceWidth,
	unsigned int a_uiSourceHeight,
	Etc::Image::Format a_format,
	Etc::ErrorMetric a_eErrMetric,
	float a_fEffort,
	unsigned int a_uiJobs,
	unsigned int a_uiMaxJobs,
	unsigned char **a_ppaucEncodingBits,
	unsigned int *a_puiEncodingBitsBytes,
	unsigned int *a_puiExtendedWidth,
	unsigned int *a_puiExtendedHeight,
	int *a_piEncodingTime_ms, bool a_bVerboseOutput)
{
	Etc::Encode(a_pafSourceRGBA,
		a_uiSourceWidth,
		a_uiSourceHeight,
		a_format,
		a_eErrMetric,
		a_fEffort,
		a_uiJobs,
		a_uiMaxJobs,
		a_ppaucEncodingBits,
		a_puiEncodingBitsBytes,
		a_puiExtendedWidth,
		a_puiExtendedHeight,
		a_piEncodingTime_ms,
		a_bVerboseOutput);
}

namespace Etc
{
	// ----------------------------------------------------------------------------------------------------
	// C-style inteface to the encoder
	//
	void Encode(float *a_pafSourceRGBA,
				unsigned int a_uiSourceWidth, 
				unsigned int a_uiSourceHeight,
				Image::Format a_format,
				ErrorMetric a_eErrMetric,
				float a_fEffort,
				unsigned int a_uiJobs,
				unsigned int a_uiMaxJobs,
				unsigned char **a_ppaucEncodingBits,
				unsigned int *a_puiEncodingBitsBytes,
				unsigned int *a_puiExtendedWidth,
				unsigned int *a_puiExtendedHeight, 
				int *a_piEncodingTime_ms, bool a_bVerboseOutput)
	{

		Image image(a_pafSourceRGBA, a_uiSourceWidth,
					a_uiSourceHeight,
					a_eErrMetric);
		image.m_bVerboseOutput = a_bVerboseOutput;
		image.Encode(a_format, a_eErrMetric, a_fEffort, a_uiJobs, a_uiMaxJobs);

		*a_ppaucEncodingBits = image.GetEncodingBits();
		*a_puiEncodingBitsBytes = image.GetEncodingBitsBytes();
		*a_puiExtendedWidth = image.GetExtendedWidth();
		*a_puiExtendedHeight = image.GetExtendedHeight();
		*a_piEncodingTime_ms = image.GetEncodingTimeMs();
	}

	void EncodeMipmaps(float *a_pafSourceRGBA,
		unsigned int a_uiSourceWidth,
		unsigned int a_uiSourceHeight,
		Image::Format a_format,
		ErrorMetric a_eErrMetric,
		float a_fEffort,
		unsigned int a_uiJobs,
		unsigned int a_uiMaxJobs,
		unsigned int a_uiMaxMipmaps,
		unsigned int a_uiMipFilterFlags,
		RawImage* a_pMipmapImages,
		int *a_piEncodingTime_ms, 
		bool a_bVerboseOutput)
	{
		auto mipWidth = a_uiSourceWidth;
		auto mipHeight = a_uiSourceHeight;
		int totalEncodingTime = 0;
		for(unsigned int mip = 0; mip < a_uiMaxMipmaps && mipWidth >= 1 && mipHeight >= 1; mip++)
		{
			float* pImageData = nullptr;
			float* pMipImage = nullptr;

			if(mip == 0)
			{
				pImageData = a_pafSourceRGBA;
			}
			else
			{
				pMipImage = new float[mipWidth*mipHeight*4];
				if(FilterTwoPass(a_pafSourceRGBA, a_uiSourceWidth, a_uiSourceHeight, pMipImage, mipWidth, mipHeight, a_uiMipFilterFlags, Etc::FilterLanczos3) )
				{
					pImageData = pMipImage;
				}
			}

			if ( pImageData )
			{
			
				Image image(pImageData, mipWidth, mipHeight,	a_eErrMetric);

			image.m_bVerboseOutput = a_bVerboseOutput;
			image.Encode(a_format, a_eErrMetric, a_fEffort, a_uiJobs, a_uiMaxJobs);

			a_pMipmapImages[mip].paucEncodingBits = std::shared_ptr<unsigned char>(image.GetEncodingBits(), [](unsigned char *p) { delete[] p; });
			a_pMipmapImages[mip].uiEncodingBitsBytes = image.GetEncodingBitsBytes();
			a_pMipmapImages[mip].uiExtendedWidth = image.GetExtendedWidth();
			a_pMipmapImages[mip].uiExtendedHeight = image.GetExtendedHeight();

			totalEncodingTime += image.GetEncodingTimeMs();
			}

			if(pMipImage)
			{
				delete[] pMipImage;
			}

			if (!pImageData)
			{
				break;
			}

			mipWidth >>= 1;
			mipHeight >>= 1;
		}

		*a_piEncodingTime_ms = totalEncodingTime;
	}


	// ----------------------------------------------------------------------------------------------------
	//

}
