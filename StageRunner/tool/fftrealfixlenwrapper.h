/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef FFTREALFIXLENWRAPPER_H
#define FFTREALFIXLENWRAPPER_H

#define REALFFT_FFT_POWER 10
#define REALFFT_FFT_DIM 1024		// pow(2,10)

#include <QVector>

class FrqSpectrum;

class FFTRealFixLenWrapperPrivate;
class FFTRealFixLenWrapper
{
public:
	FFTRealFixLenWrapper();
	~FFTRealFixLenWrapper();

	inline int fftDimension() const {return REALFFT_FFT_DIM;}
	inline void setOversampling(int over) {m_oversampling = over;}
	inline void setHannEnabled(bool state) {m_hannEnabled = state;}
	inline void appendToBuffer(float real) {m_inBuffer.append(real);}
	inline void clearBuffer() {m_inBuffer.clear();}
	inline bool bufferFilled() const {return m_inBuffer.size() >= REALFFT_FFT_DIM;}
	inline QVector<float> & fftComplexArray() {return m_fftComplexArray;}

	void calculateFFT();
	void calcutateReverseFFT();

	void doFFT(float f[], const float real[]);		///< Transform Real to FFT
	void doIFFT(const float f[], float real[]);		///< Transform FFT complex to Real
	void doFFT(QVector<float> & fvec, const QVector<float> &realvec);

private:
	void init();

private:
	FFTRealFixLenWrapperPrivate * m_priv;
	QVector<float>m_fftRealArray;
	QVector<float>m_fftComplexArray;
	QVector<float>m_windowArray;

	QVector<float>m_inBuffer;

	int m_oversampling;
	bool m_hannEnabled;
};


#endif // FFTREALFIXLENWRAPPER_H
