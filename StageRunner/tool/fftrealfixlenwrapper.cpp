/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
************************************************************************************/

#include "fftrealfixlenwrapper.h"

// FFTReal code generates quite a lot of 'unused parameter' compiler warnings,
// which we suppress here in order to get a clean build output.
#if defined Q_CC_MSVC
#    pragma warning(disable:4100)
#elif defined Q_CC_GNU
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined Q_CC_MWERKS
#    pragma warning off (10182)
#endif

#include "ffft/FFTRealFixLen.h"
#include <qmath.h>

class FFTRealFixLenWrapperPrivate {
public:
	ffft::FFTRealFixLen<REALFFT_FFT_POWER> m_fft;
};


FFTRealFixLenWrapper::FFTRealFixLenWrapper()
 : m_priv(new FFTRealFixLenWrapperPrivate)
 , m_fftRealArray(REALFFT_FFT_DIM)
 , m_fftComplexArray(REALFFT_FFT_DIM)
 , m_windowArray(REALFFT_FFT_DIM)
 , m_oversampling(1)
 , m_hannEnabled(false)
{
	init();
}

FFTRealFixLenWrapper::~FFTRealFixLenWrapper()
{
	delete m_priv;
}

void FFTRealFixLenWrapper::calculateFFT()
{
	// FFT can only done if enough data is in input buffer
	Q_ASSERT(m_inBuffer.size() >= REALFFT_FFT_DIM);

	// Copy Buffer Dat to fft_in Buffer

	if (m_hannEnabled) {
		for (int t=0; t<REALFFT_FFT_DIM; t++)
			m_fftRealArray[t] = m_inBuffer[t] * m_windowArray[t];
	} else {
		for (int t=0; t<REALFFT_FFT_DIM; t++)
			m_fftRealArray[t] = m_inBuffer[t];
	}
	// remove used data (regarding the oversampling setting
	m_inBuffer.remove(0,REALFFT_FFT_DIM / m_oversampling);

	// Calculate FFT
	m_priv->m_fft.do_fft(m_fftComplexArray.data(),m_fftRealArray.data());

	// fprintf(stdout, "calc fft m_inBuffer size: %d\n",m_inBuffer.size());
}

void FFTRealFixLenWrapper::calcutateReverseFFT()
{
	m_priv->m_fft.do_ifft(m_fftComplexArray.data(),m_fftRealArray.data());
}

void FFTRealFixLenWrapper::doFFT(float f[], const float real[])
{
	m_priv->m_fft.do_fft(f,real);
}

void FFTRealFixLenWrapper::doIFFT(const float f[], float real[])
{
	m_priv->m_fft.do_ifft(f,real);
}

void FFTRealFixLenWrapper::doFFT(QVector<float> &fvec, const QVector<float> &realvec)
{
	m_priv->m_fft.do_fft(fvec.data(),realvec.data());
}

void FFTRealFixLenWrapper::init()
{
	// Calculate Hann Window
	for (int t=0; t<REALFFT_FFT_DIM; t++) {
		m_windowArray[t] = 0.5 * (1 - qCos((2 * M_PI * t) / (REALFFT_FFT_DIM - 1)));
	}
}
