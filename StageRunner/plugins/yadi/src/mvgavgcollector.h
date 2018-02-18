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
