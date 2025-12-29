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

#include "mvgavgcollector.h"

MvgAvgCollector::MvgAvgCollector(int n, MEAN_TYPE type)
	: m_type(type)
	, m_order(n)
	, m_sumValues(0.0f)
{
	Q_ASSERT(m_order > 0);
}

void MvgAvgCollector::clear()
{
	m_values.clear();
	m_sumValues = 0.0f;
}

void MvgAvgCollector::append(double val)
{
	m_values.append(val);
	m_sumValues += val;

	if (m_values.size() > m_order)
		m_sumValues -= m_values.takeFirst();
}

void MvgAvgCollector::append(double val, bool &isFilled)
{
	m_values.append(val);
	m_sumValues += val;

	if (m_values.size() > m_order) {
		m_sumValues -= m_values.takeFirst();
		isFilled = true;
	} else {
		isFilled = false;
	}
}

double MvgAvgCollector::value() const
{
	switch (m_type) {
	case MT_SMA:
		return m_sumValues / m_values.size();
	default:
		return 0.0;
	}
}
