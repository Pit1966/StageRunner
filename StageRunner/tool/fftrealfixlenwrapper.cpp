#include "fftrealfixlenwrapper.h"

// FFTReal code generates quite a lot of 'unused parameter' compiler warnings,
// which we suppress here in order to get a clean build output.
#if defined Q_CC_MSVC
#    pragma warning(disable:4100)
#elif defined Q_CC_GNU
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined Q_CC_MWERKS
#    pragma warning off (10182)
#endif

#include "ffft/FFTRealFixLen.h"
#include <qmath.h>

class FFTRealFixLenWrapperPrivate {
public:
	ffft::FFTRealFixLen<REALFFT_FFT_POWER> m_fft;
};


FFTRealFixLenWrapper::FFTRealFixLenWrapper()
 : m_priv(new FFTRealFixLenWrapperPrivate)
 , m_fftRealArray(REALFFT_FFT_DIM)
 , m_fftComplexArray(REALFFT_FFT_DIM)
 , m_windowArray(REALFFT_FFT_DIM)
 , m_oversampling(1)
 , m_hannEnabled(false)
{
	init();
}

FFTRealFixLenWrapper::~FFTRealFixLenWrapper()
{
	delete m_priv;
}

void FFTRealFixLenWrapper::calculateFFT()
{
	// FFT can only done if enough data is in input buffer
	Q_ASSERT(m_inBuffer.size() >= REALFFT_FFT_DIM);

	// Copy Buffer Dat to fft_in Buffer

	if (m_hannEnabled) {
		for (int t=0; t<REALFFT_FFT_DIM; t++)
			m_fftRealArray[t] = m_inBuffer[t] * m_windowArray[t];
	} else {
		for (int t=0; t<REALFFT_FFT_DIM; t++)
			m_fftRealArray[t] = m_inBuffer[t];
	}
	// remove used data (regarding the oversampling setting
	m_inBuffer.remove(0,REALFFT_FFT_DIM / m_oversampling);

	// Calculate FFT
	m_priv->m_fft.do_fft(m_fftComplexArray.data(),m_fftRealArray.data());
}

void FFTRealFixLenWrapper::calcutateReverseFFT()
{
	m_priv->m_fft.do_ifft(m_fftComplexArray.data(),m_fftRealArray.data());
}

void FFTRealFixLenWrapper::doFFT(float f[], const float real[])
{
	m_priv->m_fft.do_fft(f,real);
}

void FFTRealFixLenWrapper::doIFFT(const float f[], float real[])
{
	m_priv->m_fft.do_ifft(f,real);
}

void FFTRealFixLenWrapper::doFFT(QVector<float> &fvec, const QVector<float> &realvec)
{
	m_priv->m_fft.do_fft(fvec.data(),realvec.data());
}

void FFTRealFixLenWrapper::init()
{
	// Calculate Hann Window
	for (int t=0; t<REALFFT_FFT_DIM; t++) {
		m_windowArray[t] = 0.5 * (1 - qCos((2 * M_PI * t) / (REALFFT_FFT_DIM - 1)));
	}
}
