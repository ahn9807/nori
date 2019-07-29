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
    virtual Color3f tr(const Ray3f &ray, const Sampler *sampler) const =0;
    virtual Color3f sample(const Ray3f &ray, Sampler &sampler) const =0;
    Color3f getSigmaT() {   return sigma_t; }
    Color3f getSigmaS() {   return sigma_s; }
    Color3f getSigmaA() {   return sigma_a; }
    EClassType getClassType() const { return EMedium; }
    
protected:
    Color3f sigma_a, sigma_s, sigma_t;
    Color3f albedo;
};

NORI_NAMESPACE_END
