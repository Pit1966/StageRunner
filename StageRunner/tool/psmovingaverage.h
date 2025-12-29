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

#ifndef PSMOVINGAVERAGE_H
#define PSMOVINGAVERAGE_H

template < class T >
class PsMovingAverage
{
private:
	T *data;
	T m_dataSum;
	T m_avg;

	int m_width;
	int m_in;
	bool m_filled;
public:
	PsMovingAverage(int width)
		: data(new T[width])
		, m_dataSum(0)
		, m_avg(0)
		, m_width(width)
		, m_in(0)
		, m_filled(false)
	{
	}

	~PsMovingAverage()
	{
		delete[] data;
	}

	void clear()
	{
		m_in = 0;
		m_filled = false;
		m_dataSum = 0;
	}

	template <class TP> void append (TP dat)
	{
		m_dataSum += dat;
		if (m_filled) {
			m_dataSum -= data[m_in];
			data[m_in] = dat;
			m_avg = m_dataSum / m_width;
		} else {
			data[m_in] = dat;
			m_avg = m_dataSum / (m_in+1);
		}

		if (++m_in == m_width) {
			m_filled = true;
			m_in = 0;
		}
	}

	T currentAvg()
	{
		return m_avg;
	}

	T currentSum()
	{
		return m_dataSum;
	}
};

#endif // PSMOVINGAVERAGE_H
