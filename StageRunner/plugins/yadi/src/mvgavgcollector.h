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

#ifndef MVGAVGCOLLECTOR_H
#define MVGAVGCOLLECTOR_H

#include <QList>

class MvgAvgCollector
{
public:
	enum MEAN_TYPE {
		MT_SMA					/// simple moving average
	};
private:
	MEAN_TYPE m_type;
	int m_order;				///< Base width
	QList<double> m_values;		///< This is the list of values
	double m_sumValues;			///< current sum over all values in the list

public:
	MvgAvgCollector(int n, MEAN_TYPE type = MT_SMA);
	void clear();
	void append(double val);
	void append(double val, bool &isFilled);
	double value() const;
};

#endif // MVGAVGCOLLECTOR_H
