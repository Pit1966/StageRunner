#ifndef FFTREALFIXLENWRAPPER_H
#define FFTREALFIXLENWRAPPER_H

#define REALFFT_FFT_POWER 10
#define REALFFT_FFT_DIM 1024		// pow(2,10)

#include <QVector>

class FrqSpectrum;

class FFTRealFixLenWrapperPrivate;
class FFTRealFixLenWrapper
{
public:
	FFTRealFixLenWrapper();
	~FFTRealFixLenWrapper();

	inline int fftDimension() const {return REALFFT_FFT_DIM;}
	inline void setOversampling(int over) {m_oversampling = over;}
	inline void setHannEnabled(bool state) {m_hannEnabled = state;}
	inline void appendToBuffer(float real) {m_inBuffer.append(real);}
	inline void clearBuffer() {m_inBuffer.clear();}
	inline bool bufferFilled() const {return m_inBuffer.size() >= REALFFT_FFT_DIM;}
	inline QVector<float> & fftComplexArray() {return m_fftComplexArray;}

	void calculateFFT();
	void calcutateReverseFFT();

	void doFFT(float f[], const float real[]);		///< Transform Real to FFT
	void doIFFT(const float f[], float real[]);		///< Transform FFT complex to Real
	void doFFT(QVector<float> & fvec, const QVector<float> &realvec);

private:
	void init();

private:
	FFTRealFixLenWrapperPrivate * m_priv;
	QVector<float>m_fftRealArray;
	QVector<float>m_fftComplexArray;
	QVector<float>m_windowArray;

	QVector<float>m_inBuffer;

	int m_oversampling;
	bool m_hannEnabled;
};


#endif // FFTREALFIXLENWRAPPER_H
