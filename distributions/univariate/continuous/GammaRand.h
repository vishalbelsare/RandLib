#ifndef GAMMARAND_H
#define GAMMARAND_H

#include "ContinuousDistribution.h"

/**
 * @brief The GammaRand class
 * Gamma distribution
 * f(x | α, β) = β^α / Γ(α) * x^(α-1) * exp(-βx),
 * where Γ(α) denotes Gamma function
 *
 * Notation X ~ Gamma(α, β)
 *
 * Related distributions:
 * σX ~ Gamma(α, σβ)
 * If X ~ Gamma(1, β), then X ~ Exp(β)
 * If X ~ Gamma(0.5 * n, 0.5), then X ~ Chi^2(n)
 * If X ~ Gamma(k, β) for integer k, then X ~ Erlang(k, β)
 */
class RANDLIBSHARED_EXPORT GammaRand : public ContinuousDistribution
{
protected:
    double alpha, theta, beta;
    double lgammaShape; /// log(Γ(α))
    double pdfCoef; ///  α * log(β) - log(Γ(α))
    double logAlpha, logBeta; ///  log(α), log(β)

private:
    double t, b, alphaInv; /// constants for sampling

public:
    GammaRand(double shape = 1, double rate = 1);
    virtual ~GammaRand() {}

    std::string Name() const override;
    SUPPORT_TYPE SupportType() const override { return RIGHTSEMIFINITE_T; }
    double MinValue() const override { return 0; }
    double MaxValue() const override { return INFINITY; }
    /**
     * @brief SetParameters
     * @param shape α
     * @param rate β
     */
    void SetParameters(double shape, double rate);
    /**
     * @brief GetShape
     * @return α
     */
    inline double GetShape() const { return alpha; }
    /**
     * @brief GetScale
     * @return θ = 1/β
     */
    inline double GetScale() const { return theta; }
    /**
     * @brief GetRate
     * @return β
     */
    inline double GetRate() const { return beta; }
    /**
     * @brief GetLogGammaFunction
     * @return log(Γ(α))
     */
    inline double GetLogGammaFunction() const { return lgammaShape; }
    /**
     * @brief GetLogShape
     * @return log(α)
     */
    inline double GetLogShape() const { return logAlpha; }
    /**
     * @brief GetLogRate
     * @return log(β)
     */
    inline double GetLogRate() const { return logBeta; }

    double f(const double & x) const override;
    double logf(const double & x) const override;
    double F(const double & x) const override;
    double S(const double & x) const override;
    
private:
    enum GENERATOR_ID {
        INTEGER_SHAPE, /// Erlang distribution for α = 1, 2, 3
        ONE_AND_A_HALF_SHAPE, /// α = 1.5
        SMALL_SHAPE, /// α < 0.34
        FISHMAN, /// 1 < α < 1.2
        MARSAGLIA_TSANG /// 0.34 < α < 1 or α >= 1.2
    };
    /**
     * @brief getIdOfUsedGenerator
     * @param shape α
     * @return id of used variate generator according to the shape
     */
    static GENERATOR_ID getIdOfUsedGenerator(double shape);
    /**
     * @brief variateThroughExponentialSum
     * @param shape α
     * @return gamma variate, generated by sum of exponentially distributed random variables
     */
    static double variateThroughExponentialSum(int shape);
    /**
     * @brief variateForShapeOneAndAHalf
     * @return gamma variate for α = 1.5
     */
    static double variateForShapeOneAndAHalf();
    /**
     * @brief variateBest
     * @return gamma variate for small α, using Best algorithm
     */
    double variateBest() const;
    /**
     * @brief variateAhrensDieter
     * @param shape α
     * @return gamma variate for small α, using Ahrens-Dieter algorithm (if we have no pre-calculated values)
     */
    static double variateAhrensDieter(double shape);
    /**
     * @brief variateFishman
     * @param shape α
     * @return gamma variate, using Fishman algorithm
     */
    static double variateFishman(double shape);
    /**
     * @brief variateMarsagliaTsang
     * @param shape α
     * @return gamma variate, using Marsaglia-Tsang algorithm
     */
    static double variateMarsagliaTsang(double shape);
    
public:
    /**
     * @brief StandardVariate
     * @param shape α
     * @return gamma variate with shape α and unity rate
     */
    static double StandardVariate(double shape);
    /**
     * @brief Variate
     * @param shape α
     * @param rate β
     * @return gamma variate with shape α and rate β
     */
    static double Variate(double shape, double rate);

    double Variate() const override;
    void Sample(std::vector<double> &outputData) const override;

    /**
     * @brief Mean
     * @return E[X]
     */
    double Mean() const override;
    /**
     * @brief GeometricMean
     * @return E[ln(X)]
     */
    double GeometricMean() const;
    /**
     * @brief Variance
     * @return Var(X)
     */
    double Variance() const override;
    /**
     * @brief GeometricVariance
     * @return Var(ln(X))
     */
    double GeometricVariance() const;
    double Mode() const override;
    double Skewness() const override;
    double ExcessKurtosis() const override;

private:
    /// quantile auxiliary functions
    double initRootForSmallP(double r) const;
    double initRootForLargeP(double logQ) const;
    double initRootForSmallShape(double p) const;
    double initRootForLargeShape(double p) const;
    double df(double x) const;
    double quantileInitialGuess(double p) const;
    double quantileInitialGuess1m(double p) const;
    double quantileImpl(double p) const override;
    double quantileImpl1m(double p) const override;

    std::complex<double> CFImpl(double t) const override;
public:
    /**
     * @brief FitRateUMVU
     * set rate, returned by uniformly minimum variance unbiased estimator
     * @param sample
     */
    void FitRateUMVU(const std::vector<double> &sample);
    /**
     * @brief FitRateMLE
     * set rate, estimated via maximum-likelihood method
     * @param sample
     */
    void FitRateMLE(const std::vector<double> &sample);
    /**
     * @brief FitShapeAndRateMLE
     * set shape and rate, estimated via maximum-likelihood method
     * @param sample
     */
    void FitShapeAndRateMLE(const std::vector<double> &sample);
    /**
     * @brief FitShapeMM
     * set shape, estimated via method of moments
     * @param sample
     */
    void FitShapeMM(const std::vector<double> &sample);
    /**
     * @brief FitRateMM
     * set rate, estimated via method of moments
     * (it duplicates estimation of rate by maximum-likelihood)
     * @param sample
     */
    void FitRateMM(const std::vector<double> &sample);
    /**
     * @brief FitShapeAndRateMM
     * set shape and rate, estimated via method of moments
     * @param sample
     */
    void FitShapeAndRateMM(const std::vector<double> &sample);
    /**
     * @brief FitRateBayes
     * set rate, returned by bayesian estimation
     * @param sample
     * @param priorDistribution
     * @return posterior distribution
     */
    GammaRand FitRateBayes(const std::vector<double> &sample, const GammaRand &priorDistribution);
};


/**
 * @brief The ChiSquaredRand class
 * Chi-squared distribution
 *
 * Notation: X ~ Chi^2(n)
 *
 * Related distributions:
 * X ~ Gamma(0.5 * n, 0.5)
 */
class RANDLIBSHARED_EXPORT ChiSquaredRand : public GammaRand
{
public:
    explicit ChiSquaredRand(int degree = 1);
    std::string Name() const override;

    void SetDegree(int degree);
    inline int GetDegree() const { return static_cast<int>(2 * alpha); }

private:
    /// prohibit to use gamma's public setters
    using GammaRand::SetParameters;
    /// and all estimators
    void FitRateUMVU(const std::vector<double> &sample) = delete;
    void FitRateMLE(const std::vector<double> &sample) = delete;
    void FitShapeAndRateMLE(const std::vector<double> &sample) = delete;
    void FitShapeMM(const std::vector<double> &sample) = delete;
    void FitRateMM(const std::vector<double> &sample) = delete;
    void FitShapeAndRateMM(const std::vector<double> &sample) = delete;
    GammaRand FitRateBayes(const std::vector<double> &sample, const GammaRand &priorDistribution) = delete;
};


/**
 * @brief The ErlangRand class
 * Erlang distibution
 *
 * Notation: X ~ Erlang(k, β)
 *
 * Related distributions:
 * X ~ Y_1 + Y_2 + ... + Y_k, where Y_i ~ Exp(β)
 * X ~ Gamma(k, β)
 */
class RANDLIBSHARED_EXPORT ErlangRand : public GammaRand
{
public:
    ErlangRand(int shape = 1, double rate = 1);
    std::string Name() const override;

private:
    /// prohibit to use gamma's public setters
    using GammaRand::SetParameters;
    /// and estimators of shape
    void FitShapeAndRateMLE(const std::vector<double> &sample) = delete;
    void FitShapeMM(const std::vector<double> &sample) = delete;
    void FitShapeAndRateMM(const std::vector<double> &sample) = delete;
};


#endif // GAMMARAND_H
