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

#include "frqspectrum.h"

#include <math.h>

FrqSpectrum::FrqSpectrum(int frqBands)
	: m_bands(frqBands)
	, m_maxFrequency(24000.0f)
{
}

FrqSpectrum &FrqSpectrum::operator =(const FrqSpectrum &other)
{
	if (m_bands.size() != other.m_bands.size())
		setFrqBands(other.size());

	m_bands = other.m_bands;

	return *this;
}

void FrqSpectrum::setFrqBands(int frqBands)
{
	m_bands.fill(Band(),frqBands);
}

void FrqSpectrum::clear()
{
	for (int t=0; t<m_bands.size(); t++) {
		m_bands[t].level = 0;
		m_bands[t].phase = 0;
	}
}

void FrqSpectrum::fillSpectrumFFTQVectorArray(const QVector<float> &fftArray)
{
	// Amount of frequency bands is half than FFT result size (nyquist)
	int bands = fftArray.size()/2;

	// Resize the spectrum if source array does not have the same size
	if (bands != m_bands.size())
		setFrqBands(bands);


	for (int t=0; t<bands; t++) {
		const float real = fftArray.at(t);
		const float imag = fftArray.at(bands + t);
		const float magnitude = sqrt(real * real + imag * imag);
		const float level = log(magnitude) * 0.18f;

		if (level < 0.0f) {
			m_bands[t].level = 0.0f;
		}
		else if (level > 1.0f) {
			m_bands[t].level = 1.0f;
		}
		else {
			m_bands[t].level = level;
		}

	}
}

void FrqSpectrum::fillSpectrumFFTFloatArray(const float fftArray[], int arraySize)
{
	// Amount of frequency bands is half than FFT result size (nyquist)
	int bands = arraySize/2;

	// Resize the spectrum if source array does not have the same size
	if (bands != m_bands.size())
		setFrqBands(bands);


	for (int t=0; t<bands; t++) {
		const float real = fftArray[t];
		const float imag = fftArray[bands + t];
		const float magnitude = sqrt(real * real + imag * imag);
		const float level = log(magnitude) * 0.18f;

		if (level < 0.0f) {
			m_bands[t].level = 0.0f;
		}
		else if (level > 1.0f) {
			m_bands[t].level = 1.0f;
		}
		else {
			m_bands[t].level = level;
		}

	}
}

float FrqSpectrum::getPeakSpecFromSegment(int seg, int segments) const
{
	if (seg >= segments)
		return 0;

	int segwidth = m_bands.size() / segments;
	int startband = seg * segwidth;

	float peak = 0;

	for (int t = startband; t<startband+segwidth; t++) {
		if (m_bands.at(t).level > peak)
			peak = m_bands.at(t).level;
	}
	return peak;
}

float FrqSpectrum::getAvgSpecFromSegment(int seg, int segments) const
{
	if (seg >= segments)
		return 0;

	int segwidth = m_bands.size() / segments;
	int startband = seg * segwidth;

	float sum = 0;

	for (int t = startband; t<startband+segwidth; t++) {
			sum += m_bands.at(t).level;
	}


	return sum * 2.0f / segwidth;
}

void FrqSpectrum::transformPeakFrom(const FrqSpectrum &other)
{
	if (size() < other.size()) {
		for (int t=0; t<size(); t++) {
			m_bands[t].level = other.getPeakSpecFromSegment(t,size());
		}
	}
	m_maxFrequency = other.m_maxFrequency;
}

void FrqSpectrum::transformAvgFrom(const FrqSpectrum &other)
{
	if (size() < other.size()) {
		for (int t=0; t<size(); t++) {
			m_bands[t].level = other.getAvgSpecFromSegment(t,size());
		}
	}
	m_maxFrequency = other.m_maxFrequency;
}
