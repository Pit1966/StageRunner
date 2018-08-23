#ifndef PSMOVINGAVERAGE_H
#define PSMOVINGAVERAGE_H

template < class T > class PsMovingAverage
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
