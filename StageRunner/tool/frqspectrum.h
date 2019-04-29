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

#ifndef FRQSPECTRUM_H
#define FRQSPECTRUM_H

#include <QVector>

class FrqSpectrum
{
public:
	struct Band {
		Band()
			: level(0.0f)
			, phase(0.0f)
		{}
		float level;	///< amplitude [0.0:1.0]
		float phase;	///< phase [0.0:2PI]
	};

private:
	QVector<Band> m_bands;
	double m_maxFrequency;

public:
	FrqSpectrum(int frqBands = 8192);
	FrqSpectrum & operator = (const FrqSpectrum &other);
	void setFrqBands(int frqBands);
	void clear();
	inline void setMaxFrequency(double val) {m_maxFrequency = val;}
	inline double maxFrequency() const {return m_maxFrequency;}
	inline int size() const {return m_bands.count();}
	inline const Band bandAt(int i) const {return m_bands.at(i);}
	inline float levelAt(int i) const {return m_bands.at(i).level;}

	void fillSpectrumFFTQVectorArray(const QVector<float> &fftArray);
	void fillSpectrumFFTFloatArray(const float fftArray[], int arraySize);
	float getPeakSpecFromSegment(int seg, int segments) const;
	float getAvgSpecFromSegment(int seg,int segments) const;

	void transformPeakFrom(const FrqSpectrum &other);
	void transformAvgFrom(const FrqSpectrum &other);

};

#endif // FRQSPECTRUM_H
