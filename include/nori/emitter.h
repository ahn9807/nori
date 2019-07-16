/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <nori/object.h>
#include <nori/mesh.h>

NORI_NAMESPACE_BEGIN

struct EmitterQueryRecord {
    /// Incident direction (in the local frame)
    Vector3f wi;
    
    /// Position for Light Sample
    Point3f pos;
    
    /// Positino for source
    Point3f ref;
    
    /// Normal for that light sample position
    Normal3f normal;
    
    /// Measure associated with the sample
    EMeasure measure;
    
    EmitterQueryRecord(const Point3f &ref)
    : ref(ref), measure(EUnknownMeasure) { }
    
    EmitterQueryRecord(const Point3f &ref, const Normal3f &normal, const Vector3f &wi)
    : ref(ref), normal(normal), wi(wi), measure(EUnknownMeasure) { }


};
/**
 * \brief Superclass of all emitters
 */
class Emitter : public NoriObject {
public:
    virtual void setMesh(Mesh* mesh) = 0;
    
    virtual Color3f sample(EmitterQueryRecord &eqr, Sampler *sampler) = 0;
    
    virtual float pdf(const EmitterQueryRecord &eqr) = 0;
    
    virtual Color3f Le(const EmitterQueryRecord &eqr) const = 0;
    
    virtual bool rayIntersect(const Scene* scene, Ray3f &shadowRay, Intersection &its) const = 0;
    
    virtual bool rayIntersect(const Scene* scene, Ray3f &shadowRay) const = 0;
    
    virtual bool isDeltaLight() const = 0;
    /**
     * \brief Return the type of object (i.e. Mesh/Emitter/etc.) 
     * provided by this instance
     * */
    
    EClassType getClassType() const { return EEmitter; }
    
protected:

};

NORI_NAMESPACE_END
