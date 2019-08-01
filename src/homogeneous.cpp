/*
 This file is part of Nori, a simple educational ray tracer
 
 Copyright (c) 2012 by Wenzel Jakob and Steve Marschner.
 
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

#include <nori/medium.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief Box-shaped homogeneous participating medium
 */
class HomogeneousMedium : public Medium {
public:
    HomogeneousMedium(const PropertyList &propList) {
        m_sigmaS = propList.getColor("sigmaS");
        m_sigmaT = propList.getColor("sigmaA") + m_sigmaS;
        // An (optional) transformation that converts between medium and world coordinates
        m_worldToMedium = propList.getTransform("toWorld", Transform()).inverse();
    }
    
    bool sampleDistance(const Ray3f &ray, Sampler *sampler, float &t, Color3f &weight) const {
        throw NoriException("HomogeneousMedium::sampleDistance(): not implemented!");
    }
    
    Color3f evalTransmittance(const Ray3f &ray, Sampler *sampler) const {
        throw NoriException("HomogeneousMedium::evalTransmittance(): not implemented!");
    }
    
    /// Return a human-readable summary
    std::string toString() const {
        return tfm::format(
                       "HomogeneousMedium[\n"
                       "  sigmaS = %1,\n"
                       "  sigmaT = %2,\n"
                       "]",
        m_sigmaS.toString(),
        m_sigmaT.toString());
    }
private:
    Color3f m_sigmaS;
    Color3f m_sigmaT;
    Transform m_worldToMedium;
};

NORI_REGISTER_CLASS(HomogeneousMedium, "homogeneous");
NORI_NAMESPACE_END
