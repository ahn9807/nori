//
//  Denoiser.hpp
//  Half
//
//  Created by junho on 2019. 7. 8..
//

#pragma once

#include <nori/object.h>
#include <nori/color.h>
#include <nori/block.h>
#include <nori/bitmap.h>
#include <nori/rfilter.h>
#include <nori/bbox.h>
#include <tbb/tbb.h>

#define DENOISER_FILTER_SIZE 21
#define DENOISER_FILTER_NUM 8

NORI_NAMESPACE_BEGIN
class Denoiser {
public:
    void initialize(int imageWidht, int imageHeight, int sampleCount);
    void setColor(Bitmap* source);
    void setGamma(float gamma) {    this->gamma = gamma;    }
    void calculate();
    void setVariance(int x, int y, Color3f value);
    void getVarianceFinal();
    Bitmap* getResult();
private:
    float eval(int x, int y, float stddev, float radius) const ;
    Color3f pushToFilter(int x, int y, int step = -1);
    int calculateMSE(int x, int y, int step);
    void filterSelection();
    
    Bitmap *input;
    Bitmap *result;
    Vector2i size;
    std::vector<float*>* filters;
    std::vector<float>* filterPdfs;
    std::vector<std::vector<Color3f>*>* samples;
    std::vector<float>* variances;
    std::vector<int>* filterScales;
    float gamma = 0.04f;
    float biasFactor;
    float initialStddev = 0.2f;
    float stddevStep = sqrt(2);
    int sampleCount;
};
NORI_NAMESPACE_END
