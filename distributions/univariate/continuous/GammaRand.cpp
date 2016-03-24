#include "GammaRand.h"
#include "UniformRand.h"
#include "ExponentialRand.h"
#include "NormalRand.h"

GammaRand::GammaRand(double shape, double scale)
{
    setParameters(shape, scale);
}

std::string GammaRand::name()
{
    return "Gamma(" + toStringWithPrecision(getShape()) + ", " + toStringWithPrecision(getScale()) + ")";
}

void GammaRand::setConstantsForGenerator()
{
    m = alpha - 1;
    s_2 = std::sqrt(8.0 * alpha / 3) + alpha;
    s = std::sqrt(s_2);
    d = M_SQRT2 * M_SQRT3 * s_2;
    b = d + m;
    w = s_2 / (m - 1);
    v = (s_2 + s_2) / (m * std::sqrt(alpha));
    c = b + std::log(s * d / b) - m - m - 3.7203285;
}

void GammaRand::setParameters(double shape, double scale)
{
    alpha = shape;
    if (alpha <= 0)
        alpha = 1.0;
    alphaInv = 1.0 / alpha;
    
    theta = scale;
    if (theta <= 0)
        theta = 1.0;
    beta = 1.0 / theta;
   
    double alphaRound = std::round(alpha);
    if (RandMath::areClose(alpha, alphaRound)) {
        alpha = alphaRound;
        cdfCoef = 1.0 / RandMath::factorial(alpha - 1);
    }
    else {
        cdfCoef = 1.0 / std::tgamma(alpha);
    }
    pdfCoef = cdfCoef * std::pow(beta, alpha);
    variateCoef = alphaInv + M_1_E;

    if (alpha > 3)
        setConstantsForGenerator();
}

double GammaRand::f(double x) const
{
    if (x < 0)
        return 0;
    double y = std::pow(x, alpha - 1);
    y *= std::exp(-x * beta);
    return pdfCoef * y;
}

double GammaRand::F(double x) const
{
    return (x < 0) ? 0 : cdfCoef * RandMath::lowerIncGamma(alpha, x * beta);
}

double GammaRand::variateForIntegerShape() const
{
    double rv = 0;
    for (int i = 0; i < alpha; ++i)
        rv += ExponentialRand::standardVariate();
    return rv;
}

double GammaRand::variateForHalfIntegerShape() const
{
    double rv = 0;
    for (int i = 0; i < alpha - 1; ++i)
        rv += ExponentialRand::standardVariate();
    double N = NormalRand::standardVariate();
    return rv + .5 * N * N;
}

double GammaRand::variateForSmallShape() const
{
    double rv = 0;
    int iter = 0;
    do {
        double U = UniformRand::standardVariate();
        double p = alpha * variateCoef * U;
        double W = ExponentialRand::standardVariate();
        if (p <= 1)
        {
            rv = std::pow(p, alphaInv);
            if (rv <= W)
                return rv;
        }
        else
        {
            rv = -std::log(variateCoef * (1 - U));
            if ((1 - alpha) * std::log(rv) <= W)
                return rv;
        }
    } while (++iter < 1e9); /// one billion should be enough
    return NAN; /// shouldn't end up here
}

double GammaRand::variateForMediumShape() const
{
    double W1, W2;
    do {
        W1 = ExponentialRand::standardVariate();
        W2 = ExponentialRand::standardVariate();
    } while (W2 < (alpha - 1) * (W1 - std::log(W1) - 1));
    return alpha * W1;
}

double GammaRand::variateForLargeShape() const
{
    double rv = 0;
    int iter = 0;
    do {
        double U = UniformRand::standardVariate();
        if (U <= 0.0095722652)
        {
            double W1 = ExponentialRand::standardVariate();
            double W2 = ExponentialRand::standardVariate();
            rv = b * (1 + W1 / d);
            if (m * (rv / b - std::log(rv / m)) + c <= W2)
                return rv;
        }
        else
        {
            double N;
            do {
                N = NormalRand::standardVariate();
                rv = s * N + m;
            } while (rv < 0 || rv > b);
            U = UniformRand::standardVariate();
            double S = .5 * N * N;
            if (N > 0)
            {
                if (U < 1 - w * S)
                    return rv;
            }
            else if (U < 1 + S * (v * N - w))
                return rv;

            if (std::log(U) < m * std::log(rv / m) + m - rv + S)
                return rv;
        }
    } while (++iter < 1e9); /// one billion should be enough
    return NAN; /// shouldn't end up here
}
double GammaRand::variate() const
{
    if (alpha < 5) {
        double alphaRound = std::round(alpha);
        if (RandMath::areClose(alpha, alphaRound))
            return theta * variateForIntegerShape();
        if (RandMath::areClose(alpha - 0.5, alphaRound))
            return theta * variateForIntegerShape();
        if (alpha <= 1)
            return theta * variateForSmallShape();
        if (alpha <= 3)
            return theta * variateForMediumShape();
    }
    return theta * variateForLargeShape();
}

void GammaRand::sample(QVector<double> &outputData) const
{
    if (alpha < 5) {
        double alphaRound = std::round(alpha);
        if (RandMath::areClose(alpha, alphaRound)) {
            for (double &var : outputData)
                var = theta * variateForIntegerShape();
            return;
        }
        if (RandMath::areClose(alpha - 0.5, alphaRound)) {
            for (double &var : outputData)
                var = theta * variateForHalfIntegerShape();
            return;
        }
        if (alpha <= 1) {
            for (double &var : outputData)
                var = theta * variateForSmallShape();
            return;
        }
        if (alpha <= 3) {
            for (double &var : outputData)
                var = theta * variateForMediumShape();
            return;
        }
    }

    for (double &var : outputData)
        var = theta * variateForLargeShape();
}

double GammaRand::Mean() const
{
    return alpha * theta;
}

double GammaRand::Variance() const
{
    return alpha * theta * theta;
}

std::complex<double> GammaRand::CF(double t) const
{
    return std::pow(std::complex<double>(1.0, -theta * t), -alpha);
}

double GammaRand::Mode() const
{
    return (alpha < 1) ? 0 : (alpha - 1) * theta;
}

double GammaRand::Skewness() const
{
    return 2.0 / std::sqrt(alpha);
}

double GammaRand::ExcessKurtosis() const
{
    return 6.0 * alphaInv;
}

bool GammaRand::checkValidity(const QVector<double> &sample)
{
    for (double var : sample) {
        if (var < 0)
            return false;
    }
    return true;
}

bool GammaRand::fitScaleMLE(const QVector<double> &sample)
{
    if (!checkValidity(sample))
        return false;
    setParameters(alpha, RandMath::sampleMean(sample) / alpha);
    return true;
}

bool GammaRand::fitShapeAndScaleMLE(const QVector<double> &sample)
{
    int n = sample.size();
    if (n <= 0 || !checkValidity(sample))
        return false;

    /// Calculate averages
    double average = RandMath::sampleMean(sample);
    long double logAverage = 0.0L;
    for (double var : sample) {
        logAverage += std::log(var);
    }
    logAverage /= n;

    /// Calculate initial guess for shape
    double s = std::log(average) - logAverage;
    double sm3 = s - 3.0, sp12 = 12.0 * s;
    double shape = sm3 * sm3 + sp12 + sp12;
    shape = std::sqrt(shape);
    shape -= sm3;
    shape /= sp12;

    if (!RandMath::findRoot([s] (double x)
    {
        return std::log(x) - RandMath::digamma(x) - s;
    },
    [] (double x)
    {
        return 1.0 / x - RandMath::trigamma(x);
    }, shape))
        return false;

    setParameters(shape, average / shape);
    return true;
}

bool GammaRand::fitShapeMM(const QVector<double> &sample)
{
    if (!checkValidity(sample))
        return false;
    setParameters(RandMath::sampleMean(sample) / theta, theta);
    return true;
}

bool GammaRand::fitScaleMM(const QVector<double> &sample)
{
    return fitScaleMLE(sample);
}

bool GammaRand::fitShapeAndScaleMM(const QVector<double> &sample)
{  
    if (!checkValidity(sample))
        return false;
    double mu1 = RandMath::sampleMean(sample);
    double var = RandMath::sampleVariance(sample, mu1);
    double shape = mu1 * mu1 / var;
    
    setParameters(shape, mu1 / shape);
    return true;
}

bool GammaRand::fitRateBayes(const QVector<double> &sample, GammaRand &priorDistribution)
{
    int n = sample.size();
    if (n <= 0 || !checkValidity(sample))
        return false;
    double alpha0 = priorDistribution.getShape();
    double beta0 = priorDistribution.getRate();
    double newAlpha = alpha * n + alpha0;
    double newBeta = RandMath::sum(sample) + beta0;
    priorDistribution.setParameters(newAlpha, 1.0 / newBeta);
    setParameters(alpha, 1.0 / priorDistribution.Mean());
    return true;
}


/// CHI-SQUARED

ChiSquaredRand::ChiSquaredRand(int degree)
{
    setDegree(degree);
}

std::string ChiSquaredRand::name()
{
    return "Chi-squared(" + toStringWithPrecision(getDegree()) + ")";
}

void ChiSquaredRand::setDegree(int degree)
{
    GammaRand::setParameters((degree < 1) ? 0.5 : 0.5 * degree, 2);
}

int ChiSquaredRand::getDegree() const
{
    return static_cast<int>(alpha + alpha);
}


/// ERLANG

ErlangRand::ErlangRand(int shape, double rate)
{
    setParameters(shape, rate);
}

std::string ErlangRand::name()
{
    return "Erlang(" + toStringWithPrecision(getShape()) + ", " + toStringWithPrecision(getRate()) + ")";
}

void ErlangRand::setParameters(int shape, double rate)
{
    GammaRand::setParameters(std::max(shape, 1), 1.0 / rate);
}

int ErlangRand::getShape() const
{
    return static_cast<int>(GammaRand::getShape());
}

double ErlangRand::getRate() const
{
    return 1.0 / GammaRand::getScale();
}
