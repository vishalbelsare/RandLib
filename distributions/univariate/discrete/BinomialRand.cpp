#include "BinomialRand.h"
#include "../continuous/UniformRand.h"
#include "../continuous/NormalRand.h"
#include "../continuous/ExponentialRand.h"
#include "BernoulliRand.h"

BinomialRand::BinomialRand(int number, double probability)
{
    setParameters(number, probability);
}

std::string BinomialRand::name() const
{
    return "Binomial(" + toStringWithPrecision(getNumber()) + ", " + toStringWithPrecision(getProbability()) + ")";
}

void BinomialRand::setGeneratorConstants()
{
    minpq = std::min(p, q);
    npFloor = std::floor(n * minpq);
    pFloor = npFloor / n;
    pRes = (RandMath::areClose(npFloor, n * minpq) ? 0.0 : minpq - pFloor);

    GENERATOR_ID genId = getIdOfUsedGenerator();
    if (genId == BERNOULLI_SUM)
        return;
    else if (genId == WAITING) {
        G.setProbability(minpq);
        return;
    }

    nqFloor = n - npFloor;
    double qFloor = 1.0 - pFloor;
    if (pRes > 0)
        G.setProbability(pRes / qFloor);

    /// set deltas
    double npq = npFloor * qFloor;
    double coef = 128.0 * n / M_PI;
    delta1 = coef * pFloor / (81.0 * qFloor);
    delta1 = npq * std::log(delta1);
    if (delta1 > 1.0)
        delta1 = std::sqrt(delta1);
    else
        delta1 = 1.0;
    delta2 = coef * qFloor / pFloor;
    delta2 = npq * std::log(delta2);
    if (delta2 > 1.0)
        delta2 = std::sqrt(delta2);
    else
        delta2 = 1.0;

    /// set sigmas and c
    double npqSqrt = std::sqrt(npq);
    sigma1 = npqSqrt * (1.0 + 0.25 * delta1 / npFloor);
    sigma2 = npqSqrt * (1.0 + 0.25 * delta2 / nqFloor);
    c = 2.0 * delta1 / npFloor;

    /// set a's
    a1 = 0.5 * std::exp(c) * sigma1 * M_SQRT2PI;

    a2 = 0.5 * sigma2 * M_SQRT2PI;
    a2 += a1;

    coefa3 = 0.5 * delta1 / (sigma1 * sigma1);
    a3 = 1.0 / nqFloor - coefa3;
    a3 *= delta1;
    a3 = std::exp(a3);
    a3 /= coefa3;
    a3 += a2;

    coefa4 = 0.5 * delta2 / (sigma2 * sigma2);
    a4 = std::exp(-delta2 * coefa4) / coefa4;
    a4 += a3;

    logPFloor = std::log(pFloor);
    logQFloor = (pFloor == qFloor) ? logPFloor : std::log(qFloor);

    logPnpInv = logProbFloor(npFloor);
}

void BinomialRand::setParameters(int number, double probability)
{
    n = std::max(number, 1);
    p = std::min(probability, 1.0);
    p = std::max(p, 0.0);
    q = 1.0 - p;
    np = n * p;
    setGeneratorConstants();
}

double BinomialRand::logProbFloor(int k) const
{
    double y = std::log(RandMath::binomialCoef(n, k));
    y += k * logPFloor;
    y += (n - k) * logQFloor;
    return y;
}

double BinomialRand::P(int k) const
{
    if (k < 0 || k > n)
        return 0;
    if (k == n - k)
        return RandMath::binomialCoef(n, k) * std::pow(p * q, k);
    return RandMath::binomialCoef(n, k) * std::pow(p, k) * std::pow(q, n - k);
}

double BinomialRand::F(int k) const
{
    if (k < 0)
        return 0.0;
    if (k >= n)
        return 1.0;
    return RandMath::regularizedBetaFun(q, n - k, 1 + k);
}

BinomialRand::GENERATOR_ID BinomialRand::getIdOfUsedGenerator() const
{
    /// if (n is tiny and minpq is big) or p = 0.5 and n is not so large,
    /// we just sum Bernoulli random variables
    if ((n <= 3) || (n <= 13 && minpq > 0.025 * (n + 6)) || (n <= 200 && RandMath::areClose(p, 0.5)))
        return BERNOULLI_SUM;

    /// for small [np] we use simple waiting algorithm
    if ((npFloor <= 12) ||
        (pRes > 0 && npFloor <= 16))
        return WAITING;

    /// otherwise
    return REJECTION;
}

int BinomialRand::variateRejection() const
{
    /// a rejection algorithm by Devroye and Naderlsamanl (1980)
    /// p.533. Non-Uniform Random Variate Generation. Luc Devroye
    /// it can be used only when n * p is integer and p < 0.5
    bool reject = true;
    int iter = 0;
    double X, Y, V;
    do {
        double U = UniformRand::variate(0, a4);
        if (U <= a1)
        {
            double N = NormalRand::standardVariate();
            Y = sigma1 * std::fabs(N);
            reject = (Y >= delta1);
            if (!reject)
            {
                double W = ExponentialRand::standardVariate();
                X = std::floor(Y);
                V = -W - 0.5 * N * N + c;
            }
        }
        else if (U <= a2)
        {
            double N = NormalRand::standardVariate();
            Y = sigma2 * std::fabs(N);
            reject = (Y >= delta2);
            if (!reject)
            {
                double W = ExponentialRand::standardVariate();
                X = std::floor(-Y);
                V = -W - 0.5 * N * N;
            }
        }
        else if (U <= a3)
        {
            double W1 = ExponentialRand::standardVariate();
            double W2 = ExponentialRand::standardVariate();
            Y = delta1 + W1 / coefa3;
            X = std::floor(Y);
            V = -W2 - coefa3 * Y + delta1 / nqFloor;
            reject = false;
        }
        else
        {
            double W1 = ExponentialRand::standardVariate();
            double W2 = ExponentialRand::standardVariate();
            Y = delta2 + W1 / coefa4;
            X = std::floor(-Y);
            V = -W2 - coefa4 * Y;
            reject = false;
        }

        X += npFloor;
        if (!reject && X >= 0 && X <= n && V <= logProbFloor(X) - logPnpInv)
            return X;

    } while (++iter < 1e9);
    return -1;
}

int BinomialRand::variateWaiting(int number) const
{
    /// waiting algorithm, using
    /// sum of geometrically distributed variables
    int X = -1, sum = 0;
    do {
        sum += G.variate() + 1.0;
        ++X;
    } while (sum <= number);
    return X;
}

int BinomialRand::variate() const
{
    GENERATOR_ID genId = getIdOfUsedGenerator();
    switch (genId) {
    case WAITING:
    {
        double var = variateWaiting(n);
        return (p <= 0.5) ? var : n - var;
    }
    case REJECTION:
    {
        /// if X ~ Bin(n, p') and Y ~ Bin(n - Y, (p - p') / (1 - p'))
        /// then Z = X + Y ~ Bin(n, p)
        int Y = variateRejection();
        if (pRes > 0)
            Y += variateWaiting(n - Y);
        return (p > 0.5) ? n - Y : Y;
    }
    case BERNOULLI_SUM:
    default:
        return variateBernoulliSum(n, p);
    }
    return -1; /// unexpected return
}

int BinomialRand::variateBernoulliSum(int number, double probability)
{
    int var = 0;
    if (RandMath::areClose(probability, 0.5)) {
        for (int i = 0; i != number; ++i)
            var += BernoulliRand::standardVariate();
    }
    else {
        for (int i = 0; i != number; ++i)
            var += BernoulliRand::variate(probability);
    }
    return var;
}

int BinomialRand::variate(int number, double probability)
{
    return variateBernoulliSum(number, probability);
}

void BinomialRand::sample(std::vector<int> &outputData) const
{
    if (p == 0.0) {
        std::fill(outputData.begin(), outputData.end(), 0);
        return;
    }
    if (RandMath::areClose(p, 1.0)) {
        std::fill(outputData.begin(), outputData.end(), n);
        return;
    }

    GENERATOR_ID genId = getIdOfUsedGenerator();
    switch (genId) {
    case WAITING:
    {
        if (p <= 0.5) {
            for (int &var : outputData)
               var = variateWaiting(n);
        }
        else {
            for (int &var : outputData)
               var = n - variateWaiting(n);
        }
        return;
    }
    case REJECTION:
    {
        for (int &var : outputData)
            var = variateRejection();
        if (pRes > 0) {
            for (int &var : outputData)
                var += variateWaiting(n - var);
        }
        if (p > 0.5) {
            for (int &var : outputData)
               var = n - var;
        }
        return;
    }
    case BERNOULLI_SUM:
    default:
    {
        for (int &var : outputData)
           var = variateBernoulliSum(n, p);
        return;
    }
    }
}

double BinomialRand::Mean() const
{
    return np;
}

double BinomialRand::Variance() const
{
    return np * q;
}

std::complex<double> BinomialRand::CF(double t) const
{
    std::complex<double> y(q + p * std::cos(t), p * std::sin(t));
    return std::pow(y, n);
}

double BinomialRand::Median() const
{
    return std::round(np);
}

int BinomialRand::Mode() const
{
    return std::round(np + p);
}

double BinomialRand::Skewness() const
{
    return (q - p) / std::sqrt(np * q);
}

double BinomialRand::ExcessKurtosis() const
{
    double y = 1.0 / (p * q);
    y -= 6.0;
    return y / n;
}

bool BinomialRand::checkValidity(const std::vector<double> &sample)
{
    for (int var : sample) {
        if (var < 0 || var > n)
            return false;
    }
    return true;
}

bool BinomialRand::fitProbabilityMLE(const std::vector<double> &sample)
{
    if (!checkValidity(sample))
        return false;
    setParameters(n, RandMath::sampleMean(sample) / n);
    return true;
}

bool BinomialRand::fitProbabilityMM(const std::vector<double> &sample)
{
    return fitProbabilityMLE(sample);
}

bool BinomialRand::fitProbabilityBayes(const std::vector<double> &sample, BetaRand &priorDistribution)
{
    int N = sample.size();
    double sum = RandMath::sum(sample);
    double alpha = priorDistribution.getAlpha();
    double beta = priorDistribution.getBeta();
    priorDistribution.setParameters(sum + alpha, N * n - sum + beta);
    setParameters(n, priorDistribution.Mean());
    return true;
}
