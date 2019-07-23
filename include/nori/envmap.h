#if !defined(__NORI_ENVMAP_H)
#define __NORI_ENVMAP_H

#include <nori/bitmap.h>
#include <nori/frame.h>
#include <nori/object.h>

typedef Eigen::Array<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> floatmat;

NORI_NAMESPACE_BEGIN

class Envmap : public Bitmap {
public:
    // ---------- CONSTRUCTORS
    Envmap(const std::string &path);
    Envmap();
    
    // ---------- MAIN FUNCTIONS
    //eval
    Color3f eval(const Vector3f &dir) const;
    //sample
    Color3f sample(Vector3f &dir, Sampler *sampler) const;
    //pdf
    float pdf(const Vector3f &dir) const;
    
    // ---------- HELPER FUNCTIONS
    // samples a point
    Point2f samplePixel(const Point2f &sample) const;
    // finds pdf of a point
    float findPdf(const Point2f &point) const;
    // point (u,v) to color on map
    Color3f findColor(const Point2f &point) const;
    //find pixel from direction
    Point2f dirToPixel(const Vector3f &vec) const;
    //find direction from pixel
    Vector3f pixelToDir(const Point2f &p) const;
    
private:
    floatmat m_luminance;
    floatmat m_pdf;
    floatmat m_cdf;
    //those matrices are vectors so only one row
    floatmat m_pmarg;
    floatmat m_cmarg;
};


NORI_NAMESPACE_END

#endif /* __NORI_ENVMAP_H */
