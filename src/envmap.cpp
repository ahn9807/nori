#include "nori/envmap.h"

NORI_NAMESPACE_BEGIN

//helper function
float precompute1D(int row, const floatmat &f, floatmat &pf, floatmat &Pf) {
    float nf = f.cols();
    
    float I = 0;
    for (int i = 0; i < nf; ++i)
        I += f(row, i);
    
    // otherwise we divide by 0
    if (I == 0)
        return I;
    
    for (int i = 0; i < nf; ++i)
        pf(row, i) = f(row, i) / I;
    
    Pf(row, 0) = 0;
    for (int i = 1; i < nf; ++i)
        Pf(row, i) = Pf(row, i-1) + pf(row, i-1);
    Pf(row, nf) = 1;
    
    return I;
}

//helper function
void sample1D(int row, const floatmat &pf, const floatmat &Pf, const float &unif, float &x, float &p) {
    //binary search
    int i = 0;
    
    for (; i < Pf.cols(); ++i) {
        //std::cout << Pf(row, i) << "\n";
        if (unif >= Pf(row, i) && unif < Pf(row, i+1))
            break;
    }
    
    float t = (Pf(row, i+1) - unif) / (Pf(row, i+1) - Pf(row, i));
    
    x = (1-t) * i + t * (i+1);
    p = pf(row, i);
}

//helper function
/// Linearly interpolate between two colors
Color3f colLerp(float t, Color3f v1, Color3f v2) {
    return (1 - t) * v1 + t * v2;
}

// bilinear interpolation
Color3f bilerp(float tx, float ty, Color3f v00, Color3f v01, Color3f v10, Color3f v11) {
    Color3f up = colLerp(tx, v00, v01);
    Color3f bottom = colLerp(tx, v10, v11);
    return colLerp(ty, up, bottom);
}

// ---------- CONSTRUCTORS
Envmap::Envmap(const std::string &path)
: Bitmap(path)
{
    //resizing
    m_luminance = floatmat(rows(), cols());
    m_pdf = floatmat(rows(), cols());
    m_cdf = floatmat(rows(), cols() + 1);
    m_pmarg = floatmat(1, rows());
    m_cmarg = floatmat(1, rows() + 1);
    
    //compute luminance matrix
    for (int i = 0; i < rows(); ++i) {
        for (int j = 0; j < cols(); ++j) {
            Color3f color = (*this)(i,j);
            m_luminance(i,j) = .3 * color.x() + .6 * color.y() + .1 * color.z() + Epsilon/10000000;
        }
    }
    
    //precomputing the pdf and cdf values
    floatmat colsum(1, rows());
    for (int i = 0; i < m_pdf.rows(); ++i) {
        colsum(0, i) = precompute1D(i, m_luminance, m_pdf, m_cdf);
    }
    precompute1D(0, colsum, m_pmarg, m_cmarg);
}

Envmap::Envmap()
:Bitmap()
{}

// ---------- MAIN FUNCTIONS
//eval
Color3f Envmap::eval(const Vector3f & dir) const {
    //std::cout << dir << "||";
    Point2f pixel = dirToPixel(dir);
    
    //std::cout << pixelToDir(pixel) << "\n \n \n";
    
    return findColor(pixel);
}

//sample
Color3f Envmap::sample(Vector3f & dir, Sampler *sampler) const {
    Point2f sample = Point2f(drand48(), drand48());
    Point2f pixel = samplePixel(sample);
    dir = pixelToDir(pixel);
    float jac = (cols() -1) * (rows() -1) / (2 * std::pow(M_PI, 2) * Frame::sinTheta(dir));
    float pdf = findPdf(pixel) * jac;
    return findColor(pixel) / pdf;
}

//pdf
float Envmap::pdf(const Vector3f &dir) const {
    Point2f pixel = dirToPixel(dir);
    return findPdf(pixel);
}

// ---------- HELPER FUNCTIONS
Point2f Envmap::samplePixel(const Point2f &sample) const{
    float u, v;
    float pdfu, pdfv;
    sample1D(0, m_pmarg, m_cmarg, sample.x(), u, pdfu);
    sample1D(std::min(roundf(u), (float) rows() - 1 ), m_pdf, m_cdf, sample.y(), v, pdfv);
    
    return Point2f(std::min(u, (float) rows() - 1 ), v);
}

float Envmap::findPdf(const Point2f &point) const{
    /*
     float u = point.x();
     float theta = u * M_PI / (rows() -1);
     float jac = (cols() -1) * (rows() -1) / (2 * std::pow(M_PI, 2) * std::sin(theta));
     */
    int i = floorf(point.x());
    int j = floorf(point.y());
    
    //std::cout << "(" << point.x() << "," << point.y() <<")  -- pmarg: " << m_pmarg(0, i) << " pdf: " << m_pdf(i, j) << " jac: " << jac <<" final: " << m_pmarg(0, i) * m_pdf(i, j) * jac << " \n";
    
    return m_pmarg(0, i) * m_pdf(i, j) /** jac*/;
    //float theta = M_PI * point.x() / rows();
    //return m_pmarg(0, i) * m_pdf(i, j) * rows() * cols() * std::pow(INV_PI, 2) / (2 * std::sin(theta));
}

Color3f Envmap::findColor(const Point2f &point) const {
    int u = floorf(point.x());
    int v = floorf(point.y());
    
    //in corners and on borders
    if (u >= rows() - 1 || v >= cols() - 1) {
        return (*this)(u,v);
    }
    
    float du = point.x() - u;
    float dv = point.y() - v;
    
    Color3f v00 = (*this)(u, v);
    Color3f v01 = (*this)(u, v+1);
    Color3f v10 = (*this)(u+1, v);
    Color3f v11 = (*this)(u+1, v+1);
    
    return bilerp(du, dv, v00, v01, v10, v11);
}

Point2f Envmap::dirToPixel(const Vector3f &vec) const{
    //float theta = std::acos(Frame::cosTheta(vec));
    //float phi = std::atan2(vec.y(), vec.x());
    
    Point2f coords = sphericalCoordinates(vec);
    
    float theta = coords.x();
    float phi = coords.y();
    
    float u = theta * INV_PI * (rows() -1);
    float v = phi  * INV_PI * 0.5 * (cols() - 1);
    
    //std::cout << u << "," << v << "\n";
    
    //if (u < 0 || v < 0)
    //    std::cout << "error negative pixel \n";
    
    if(std::isnan(u) || std::isnan(v)) {
        std::cerr << "\n No corresponding pixel for direction \n";
        return Point2f(0,0);
    }
    
    return Point2f(u,v);
}

Vector3f Envmap::pixelToDir(const Point2f &p) const {
    float u = p.x();
    float v = p.y();
    
    float theta = u * M_PI / (rows() -1);
    float phi = v * 2 * M_PI / (cols() - 1);
    
    return Vector3f(std::sin(theta) * std::cos(phi), std::sin(theta)* std::sin(phi), std::cos(theta)).normalized();
}


NORI_NAMESPACE_END
