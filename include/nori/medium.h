//
//  medium.h
//  nori
//
//  Created by Junho on 25/07/2019.
//

#pragma once

#include <nori/object.h>

NORI_NAMESPACE_BEGIN

class Medium : public NoriObject {
public:
    virtual Color3f tr(const Ray3f &ray, Sampler &sampler) const =0;
    virtual Color3f sample(const Ray3f &ray, Sampler &sampler) const =0;
    
protected:
    
};

NORI_NAMESPACE_END
