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
