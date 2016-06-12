#include "ContinuousDistribution.h"

void ContinuousDistribution::ProbabilityDensityFunction(const std::vector<double> &x, std::vector<double> &y) const
{
    size_t size = x.size();
    if (size > y.size())
        return;
    for (size_t i = 0; i != size; ++i)
        y[i] = f(x[i]);
}

double ContinuousDistribution::Quantile(double p) const
{
    if (p < 0 || p > 1)
        return NAN;

    double root = Mean(); /// good starting point
    if (std::isnan(root) || std::isinf(root))
        root = 0.0;
    if (RandMath::findRoot([this, p] (double x)
    {
        return F(x) - p;
    },
    [this] (double x)
    {
        return f(x);
    }, root))
        return root;
    /// if we can't find quantile, then probably p == 1
    return INFINITY;
}

double ContinuousDistribution::Hazard(double x) const
{
    return f(x) / (1.0 - F(x));
}

double ContinuousDistribution::ExpectedValue(const std::function<double (double)> &funPtr, double startPoint) const
{
    /// attempt to calculate expected value by numerical method
    /// use for distributions w/o explicit formula
    /// works good for unimodal and wide distributions

    /// get such low boundary 'x' that |integrand(x)| < epsilon
    static constexpr double epsilon = 1e-10;
    static constexpr int maxIter = 1000;
    int iter = 0;
    double lowerBoundary = startPoint;
    double integrand = 0;
    /// WARNING: we use variance - so there can be deadlock if we don't define this function explicitly
    /// therefore function Variance() should stay pure and noone should calculate it by this function
    double var = Variance();
    do {
       lowerBoundary -= var;
       double y = funPtr(lowerBoundary);
       integrand = (y == 0) ? 0.0 : y * f(lowerBoundary);
    } while (std::fabs(integrand) > epsilon && ++iter < maxIter);

    if (iter == maxIter) /// can't take integral, integrand decreases too slow
        return NAN;

    /// get such upper boundary 'x' that |integrand(x)| < epsilon
    double upperBoundary = startPoint;
    iter = 0;
    do {
       upperBoundary += var;
       double y = funPtr(upperBoundary);
       integrand = (y == 0) ? 0.0 : y * f(upperBoundary);
    } while (std::fabs(integrand) > epsilon && ++iter < maxIter);

    if (iter == maxIter) /// can't take integral, integrand decreases too slow
        return NAN;

    return RandMath::integral([this, funPtr] (double x)
    {
        double y = funPtr(x);
        return (y == 0.0) ? 0.0 : y * f(x);
    },
    lowerBoundary, upperBoundary, epsilon);
}

double ContinuousDistribution::Median() const
{
    return Quantile(0.5);
}

double ContinuousDistribution::Mode() const
{
    // use only for unimodal distributions!

    double mu = Mean(); /// good starting point
    if (std::isnan(mu) || std::isinf(mu))
    {
        mu = Median(); /// this shouldn't be nan or inf
        if (std::isnan(mu) || std::isinf(mu)) /// if the error occured
            mu = 0.0;
    }
    double step = 10 * Variance();
    if (std::isnan(step) || std::isinf(step))
        step = 100; // dirty hack

    /// localization
    double a = mu - step;
    double b = mu + step;
    double fa = f(a), fb = f(b), fmu = f(mu);
    while (fa > fmu)
    {
       b = mu; fb = fmu;
       mu = a; fmu = fa;
       a -= step; fa = f(a);
    }
    while (fb > fmu)
    {
       a = mu;
       mu = b; fmu = fb;
       b += step; fb = f(b);
    }

    double root = 0;
    RandMath::findMin([this] (double x)
    {
        return -f(x);
    }, a, b, root);

    return root;
}

double ContinuousDistribution::likelihood(const std::vector<double> &sample) const
{
    double res = 1.0;
    for (const double & var : sample )
        res *= f(var);
    return res;
}

double ContinuousDistribution::loglikelihood(const std::vector<double> &sample) const
{
    double res = 0.0;
    for (const double & var : sample )
        res += std::log(f(var));
    return res;
}
