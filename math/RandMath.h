#ifndef RANDMATH_H
#define RANDMATH_H

#include <cmath>
#include <functional>
#include <complex>
#include <vector>

#include "Constants.h"

#include "randlib_global.h"

constexpr double MIN_POSITIVE = 1e-21;

/**
 * @brief The RandMath class
 */
class RANDLIBSHARED_EXPORT RandMath
{
public:
    RandMath() {}

    /**
     * @brief areClose
     * @param a
     * @param b
     * @param eps
     * @return |a - b| < eps * max(a, b)
     */
    static bool areClose(double a, double b, double eps = 1e-6);

    /**
     * @brief sign
     * @param x
     * @return sign of x
     */
    static int sign(double x);

    /**
     * @brief sum
     * @param sample
     * @return sum of all elements in a sample
     */
    static double sum(const std::vector<double> &sample);
    /**
     * @brief sampleMean
     * @param sample
     * @return arithmetic average
     */
    static double sampleMean(const std::vector<double> &sample);
    /**
     * @brief sampleVariance
     * @param sample
     * @param mean known (or sample) average
     * @return second central moment
     */
    static double sampleVariance(const std::vector<double> &sample, double mean);
    static double sampleVariance(const std::vector<double> &sample);
    /**
     * @brief sampleSkewness
     * @param sample
     * @param mean
     * @param stdev
     * @return
     */
    static double sampleSkewness(const std::vector<double> &sample, double mean, double stdev);
    static double sampleSkewness(const std::vector<double> &sample, double mean);
    static double sampleSkewness(const std::vector<double> &sample);
    /**
     * @brief rawMoment
     * @param sample
     * @return k-th raw moment
     */
    static double rawMoment(const std::vector<double> &sample, int k);
    /**
     * @brief centralMoment
     * @param sample
     * @param mean known (or sample) average
     * @return k-th central moment
     */
    static double centralMoment(const std::vector<double> &sample, int k, double mean);
    static double centralMoment(const std::vector<double> &sample, int k);
    /**
     * @brief normalisedMoment
     * @param sample
     * @param k
     * @param mean
     * @param stdev
     * @return
     */
    static double normalisedMoment(const std::vector<double> &sample, int k, double mean, double stdev);
    static double normalisedMoment(const std::vector<double> &sample, int k, double mean);
    static double normalisedMoment(const std::vector<double> &sample, int k);
    
private:

    /**
     * @brief maxFactorialTableValue maximum value for input parameter to use table methods
     */
    static constexpr int maxFactorialTableValue = 255;

    /**
     * @brief factorialTable (n * 10)! for n from 0 to 25
     */
    static constexpr long double factorialTable[] =
    {
        1.l,
        3628800.l,
        2432902008176640000.l,
        265252859812191058636308480000000.l,
        815915283247897734345611269596115894272000000000.l,
        30414093201713378043612608166064768844377641568960512000000000000.l,
        8320987112741390144276341183223364380754172606361245952449277696409600000000000000.l,
        11978571669969891796072783721689098736458938142546425857555362864628009582789845319680000000000000000.l,
        71569457046263802294811533723186532165584657342365752577109445058227039255480148842668944867280814080000000000000000000.l,
        1485715964481761497309522733620825737885569961284688766942216863704985393094065876545992131370884059645617234469978112000000000000000000000.l,
        93326215443944152681699238856266700490715968264381621468592963895217599993229915608941463976156518286253697920827223758251185210916864000000000000000000000000.l,
        15882455415227429404253703127090772871724410234473563207581748318444567162948183030959960131517678520479243672638179990208521148623422266876757623911219200000000000000000000000000.l,
        6689502913449127057588118054090372586752746333138029810295671352301633557244962989366874165271984981308157637893214090552534408589408121859898481114389650005964960521256960000000000000000000000000000.l,
        6466855489220473672507304395536485253155359447828049608975952322944781961185526165512707047229268452925683969240398027149120740074042105844737747799459310029635780991774612983803150965145600000000000000000000000000000000.l,
        13462012475717524605876073858941615558355851148193967190051391468057460367090535696797920946629681836680869097041958983702264048370902871114013579941370766400374327741701139895604871545254810788060989321379840000000000000000000000000000000000.l,
        57133839564458545904789328652610540031895535786011264182548375833179829124845398393126574488675311145377107878746854204162666250198684504466355949195922066574942592095735778929325357290444962472405416790722118445437122269675520000000000000000000000000000000000000.l,
        471472363599206132240694321176194377951192623045460204976904578317542573467421580346978030238114995699562728104819596262106947389303901748942909887857509625114880781313585012959529941660203611234871833992565791817698209861793313332044734813700096000000000000000000000000000000000000000.l,
        7257415615307998967396728211129263114716991681296451376543577798900561843401706157852350749242617459511490991237838520776666022565442753025328900773207510902400430280058295603966612599658257104398558294257568966313439612262571094946806711205568880457193340212661452800000000000000000000000000000000000000000.l,
        200896062499134299656951336898466838917540340798867777940435335160044860953395980941180138112097309735631594101037399609671032132186331495273609598531966730972945653558819806475064353856858157445040809209560358463319644664891114256430017824141796753818192338642302693327818731986039603200000000000000000000000000000000000000000000.l,
        9680322675255249156123346514615331205418161260462873360750859919944104623425228207640470674933540169424682360525991982916161596983449594045525553704253602287443197783274656957056546338783001340434094795097553229620273057440272298773179365935914105128629426348958748638226084106818484328004851174161755668480000000000000000000000000000000000000000000000.l,
        788657867364790503552363213932185062295135977687173263294742533244359449963403342920304284011984623904177212138919638830257642790242637105061926624952829931113462857270763317237396988943922445621451664240254033291864131227428294853277524242407573903240321257405579568660226031904170324062351700858796178922222789623703897374720000000000000000000000000000000000000000000000000.l,
        105823620292236563784274284243348353057589905787169019562352737522144487532400210147849369011714673954768265316577892528273760626189481169051055226066650741189573897273684791411180134039439160066561895838501000817711682625725670477616267598661259194975646029749546282594356217374097544153589482020891750774735012558313460846824864172030239122128896000000000000000000000000000000000000000000000000000.l,
        22838603359146414573972658651153337270429730715462287017736347161260276926030248458777765497919211029457065581960747795750095505232241970499561769723020565876672261660609763234049775547325430135571331468257475537994508495233770658945310210552725163342784668756149049213658078338458534285571551800849578848226429898670032945513859929938621783523490272646966918544936140800000000000000000000000000000000000000000000000000000.l,
        7758587304686725201813174298892781442413952130995533365303964524344944412641389739603152000644515957408814002319492032321234250506968028455594445689972313374305301019340949789291189972149450405025159624155827152329676580440959428615802893638146558163235483142136540783687811997927615346859658417205832954125915861983307177232587595821512723429698627780530255874167602077755356592824804966400000000000000000000000000000000000000000000000000000000.l,
        4067885363647058120493575921486885310172051259182827146069755969081486918925585104009100729728348522923820890245870098659147156051905732563147381599098459244752463027688115705371704628286326621238456543307267608612545168337779669138759451760395968217423617954330737034164596496963986817722252221059768080852489940995605579171999666916004042965293896799800598079985264195119506681577622056215044851618236292136960000000000000000000000000000000000000000000000000000000000.l,
        3232856260909107732320814552024368470994843717673780666747942427112823747555111209488817915371028199450928507353189432926730931712808990822791030279071281921676527240189264733218041186261006832925365133678939089569935713530175040513178760077247933065402339006164825552248819436572586057399222641254832982204849137721776650641276858807153128978777672951913990844377478702589172973255150283241787320658188482062478582659808848825548800000000000000000000000000000000000000000000000000000000000000.l,
    };

    /**
     * @brief factorialForSmallValue
     * Get n! using product method with table
     * @param n non-negative integer number
     * @return
     */
    static long double factorialForSmallValue(int n);

public:
    /**
     * @brief factorial
     * Calculate n! using table values for small n <= 255
     * and Moivre-Stirling formula for large n > 255
     * @param n non-negative integer number
     * @return n!
     */
    static long double factorial(double n);

    /**
     * @brief doubleFactorial
     * Calculate n!!
     * @param n
     * @return
     */
    static long double doubleFactorial(int n);

    /**
     * @brief binomialCoef
     * Calculate binomial coefficient C(n,k)
     * @param n
     * @param k
     * @return C(n,k) = n! / (k! * (n - k)!)
     */
    static long double binomialCoef(int n, int k);

    /**
     * @brief digamma
     * @param x
     * @return psi(x) = d(ln(Gamma(x)))/dx = d(Gamma(x))/Gamma(x)
     */
    static double digamma(double x);

    /**
     * @brief trigamma
     * @param x
     * @return psi'(x) = d(psi(x))/dx
     */
    static double trigamma(double x);

    /**
     * @brief lowerIncGamma
     * Calculate lower incomplete gamma function
     * @param a
     * @param x
     * @return
     */
    static long double lowerIncGamma(double a, double x);

    /**
     * @brief logLowerIncGamma
     * Calculate logarithm of lower incomplete gamma function
     * @param a
     * @param x
     * @return
     */
    static long double logLowerIncGamma(double a, double x);

    /**
     * @brief upperIncGamma
     * Calculate upper incomplete gamma function
     * @param a
     * @param x
     * @return
     */
    static long double upperIncGamma(double a, double x);

    /**
     * @brief upperIncGamma
     * Calculate logarithm of upper incomplete gamma function
     * @param a
     * @param x
     * @return
     */
    static long double logUpperIncGamma(double a, double x);

    /**
     * @brief betaFun
     * Calculate Beta function
     * @param a
     * @param b
     * @return Gamma(a) * Gamma(b) / Gamma(a + b)
     */
    static double betaFun(double a, double b);

    /**
     * @brief regularizedBetaFun
     * @param x
     * @param a
     * @param b
     * @return
     */
    static double regularizedBetaFun(double x, double a, double b);

    /**
     * @brief incompleteBetaFun
     * @param x
     * @param a
     * @param b
     * @return
     */
    static double incompleteBetaFun(double x, double a, double b);

    /**
     * @brief gammaHalf
     * Calculate Gamma function of half integer: gamma(k/2)
     * @param k
     * @return
     */
    static long double gammaHalf(size_t k);

private:

    /**
     * @brief adaptiveSimpsonsAux
     * auxiliary function for calculation of integral
     * @param funPtr
     * @param a lower boundary
     * @param b upper boundary
     * @param epsilon
     * @param S
     * @param fa
     * @param fb
     * @param fc
     * @param bottom
     * @return
     */
    static long double adaptiveSimpsonsAux(const std::function<double (double)> &funPtr, double a, double b,
                                           double epsilon, double S, double fa, double fb, double fc, int bottom);

public:

    /**
     * @brief integral
     * @param funPtr integrand
     * @param a lower boundary
     * @param b upper boundary
     * @param epsilon tolerance
     * @param maxRecursionDepth how deep should the algorithm go
     * @return
     */
    static long double integral(const std::function<double (double)> &funPtr, double a, double b,
                                double epsilon = 1e-11, int maxRecursionDepth = 10);

    /**
     * @brief findRoot
     * Newton's root-finding procedure
     * @param funPtr
     * @param root starting point
     * @param epsilon
     * @return
     */
    static bool findRoot(const std::function<double (double)> &funPtr, const std::function<double (double)> &derPtr, double & root, double epsilon = 1e-10);

    /**
     * @brief findRoot
     * Quasi-Newton's root-finding procedure
     * @param funPtr
     * @param root starting point
     * @param epsilon
     * @return
     */
    static bool findRoot(const std::function<double (double)> &funPtr, double &root, double epsilon = 1e-10);

    /**
     * @brief findRoot
     * Brent's root-finding procedure
     * @param funPtr
     * @param a lower boundary
     * @param b upper boundary
     * @param root such x that funPtr(x) = 0
     * @param epsilon tolerance
     * @return true if success, false otherwise
     */
    static bool findRoot(const std::function<double (double)> &funPtr, double a, double b, double & root,
                         double epsilon = 1e-10);

    /**
     * @brief findMin
     * Combined Brent's method
     * @param funPtr
     * @param a lower boundary
     * @param b upper boundary
     * @param root such x that funPtr(x) is min
     * @param epsilon tolerance
     * @return
     */
    static bool findMin(const std::function<double (double)> &funPtr, double a, double b, double & root,
                        double epsilon = 1e-10);

    /**
     * @brief linearInterpolation
     * @param a left boundary
     * @param b right boundary
     * @param fa is equal to f(a)
     * @param fb is qual to f(b)
     * @param x the interpolated function parameter
     * @return f(x)
     */
    static double linearInterpolation(double a, double b, double fa, double fb, double x);

    /**
     * @brief harmonicNumber
     * @param exponent
     * @param number
     * @return sum_{i=1}^{number} i^{-exponent}
     */
    static double harmonicNumber(double exponent, int number);

    /**
     * @brief modifiedBessel
     * @param x
     * @param n
     * @return I_n(x)
     */
    static double modifiedBesselFirstKind(double x, double n);

    /**
     * @brief BernoulliNumber
     * @param n
     * @return Bernoulli number, calculated by Akiyama–Tanigawa algorithm
     */
    static double BernoulliNumber(int n);

    /**
     * @brief zetaRiemann
     * @param s
     * @return Riemann zeta function
     */
    static double zetaRiemann(double s);
};

#endif // RANDMATH_H
