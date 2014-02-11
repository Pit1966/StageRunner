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
