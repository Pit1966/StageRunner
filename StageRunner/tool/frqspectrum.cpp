#include "frqspectrum.h"

#include <math.h>

FrqSpectrum::FrqSpectrum(int frqBands)
	: m_bands(frqBands)
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
}

void FrqSpectrum::transformAvgFrom(const FrqSpectrum &other)
{
	if (size() < other.size()) {
		for (int t=0; t<size(); t++) {
			m_bands[t].level = other.getAvgSpecFromSegment(t,size());
		}
	}
}
