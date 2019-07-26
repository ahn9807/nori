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

#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/texture.h>

NORI_NAMESPACE_BEGIN

/// Ideal mirror BRDF
class Mirror : public BSDF {
public:
    Mirror(const PropertyList &props) {
        m_albedo = props.getColor("albedo",1);
        std::string textureAlbedo = props.getString("texture_albedo", "none");
        if(textureAlbedo != "none")
            m_texture_albedo = new Texture(m_albedo, textureAlbedo);
        else
            m_texture_albedo = new Texture(m_albedo);
    }

    Color3f eval(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return Color3f(0.0f);
    }

    float pdf(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return 0.0f;
    }

    Color3f sample(BSDFQueryRecord &bRec, const Point2f &) const {
        if (Frame::cosTheta(bRec.wi) <= 0) 
            return Color3f(0.0f);

        // Reflection in local coordinates
        bRec.wo = Vector3f(
            -bRec.wi.x(),
            -bRec.wi.y(),
             bRec.wi.z()
        );
        bRec.measure = EDiscrete;

        /* Relative index of refraction: no change */
        bRec.eta = 1.0f;
        
        /*
        if(m_texture_albedo != nullptr) {
            return m_texture_albedo->lookUp(bRec.uv);
        }
         */
        
        return m_texture_albedo->lookUp(bRec.uv);
    }

    std::string toString() const {
        return tfm::format(
                           "Mirro[\n"
                           "  albedo = %s\n"
                           "]", m_albedo.toString());
    }
private:
    Color3f m_albedo;
    Texture *m_texture_albedo;
};

NORI_REGISTER_CLASS(Mirror, "mirror");
NORI_NAMESPACE_END
