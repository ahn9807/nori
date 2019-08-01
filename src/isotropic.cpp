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

#include <nori/phase.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief Isotropic phase function model
 */
class Isotropic : public PhaseFunction {
public:
    Isotropic(const PropertyList &) {
        /* No parameters */
    }
    
    /// Evaluate the phase function
    float eval(const PhaseFunctionQueryRecord &) const {
        return INV_FOURPI;
    }
    
    /// Compute the density of \ref sample() wrt. solid angles
    float pdf(const PhaseFunctionQueryRecord &) const {
        return INV_FOURPI;
    }
    
    /// Draw a a sample from the phase function
    float sample(PhaseFunctionQueryRecord &pRec, const Point2f &sample) const {
        pRec.wo = squareToUniformSphere(sample);
        
        /* eval() / pdf() = 1.0. There
         is no need to call these functions. */
        return 1.0f;
    }
    
    /// Return a human-readable summary
    QString toString() const {
        return QString("Isotropic[]");
    }
    
    EClassType getClassType() const { return EPhaseFunction; }
};

NORI_REGISTER_CLASS(Isotropic, "isotropic");
NORI_NAMESPACE_END
