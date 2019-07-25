/*
 This file is part of Nori, a simple educational ray tracer
 Copyright (c) 2015 by Romain Prévost
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

#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/envmap.h>
#include <nori/frame.h>


NORI_NAMESPACE_BEGIN

class EnvironmentLight : public Emitter {
public:
    EnvironmentLight(const PropertyList &props) {
        //load an exr file
        m_envmap = Envmap(props.getString("path"));
    }
    
    void setMesh(Mesh *mesh) {
        return;
    }
    
    Color3f sample(EmitterQueryRecord &lRec, Sampler *sampler) {
        Color3f col = m_envmap.sample(lRec.wi, sampler);
        lRec.shadowRay = Ray3f(lRec.ref, lRec.wi);
        lRec.shadowRay = Ray3f(lRec.ref, lRec.wi, Epsilon, 10000);
        return col;
    }
    
    float pdf(const EmitterQueryRecord &lRec) {
        return m_envmap.pdf(lRec.wi);
    }
    
    Color3f Le(const EmitterQueryRecord &lRec) const {
        return m_envmap.eval(lRec.wi);
    }
    
    bool rayIntersect(const Scene* scene, Ray3f &shadowRay, Intersection &its) const {
        if(!scene->rayIntersect(shadowRay, its)) {
            return false;
        }
        if(its.mesh->getEmitter() == this)
            return false;
        
        return true;
    }
    
    bool rayIntersect(const Scene* scene, Ray3f &shadowRay) const {
        Intersection its;
        return rayIntersect(scene, shadowRay, its);
    }
    
    bool isDeltaLight() const {
        return false;
    }
    
    std::string toString() const {
        return tfm::format(
                           "EnvironmentLight[\n"
                           "]"
                           );
    }
    
private:
    Envmap m_envmap;
};

NORI_REGISTER_CLASS(EnvironmentLight, "environment")
NORI_NAMESPACE_END
