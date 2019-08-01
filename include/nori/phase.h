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

#if !defined(__PHASE_H)
#define __PHASE_H

#include <nori/object.h>

NORI_NAMESPACE_BEGIN


/**
 * \brief Convenience data structure used to pass multiple
 * parameters to the evaluation and sampling routines in \ref PhaseFunction
 */
struct PhaseFunctionQueryRecord {
    /// Incident direction (in the world frame)
    Vector3f wi;
    
    /// Outgoing direction (in the world frame)
    Vector3f wo;
    
    /// Create a new record for sampling the phase function
    inline PhaseFunctionQueryRecord(const Vector3f &wi)
    : wi(wi) { }
    
    /// Create a new record for querying the phase function
    inline PhaseFunctionQueryRecord(const Vector3f &wi,
                                    const Vector3f &wo) : wi(wi), wo(wo) { }
};

/**
 * \brief Superclass of all phase functions
 */
class PhaseFunction : public NoriObject {
public:
    /**
     * \brief Sample the phase function and return the importance weight (i.e. the
     * value of the phase function divided by the probability density of the sample
     * with respect to solid angles).
     *
     * \param pRec    A phase function query record
     * \param sample  A uniformly distributed sample on \f$[0,1]^2\f$
     *
     * \return The phase function value divided by the probability density of the sample
     *         sample. A zero value means that sampling failed.
     */
    virtual float sample(PhaseFunctionQueryRecord &pRec, const Point2f &sample) const = 0;
    
    /**
     * \brief Evaluate the phase function for a pair of directions
     * specified in \code pRec
     *
     * \param pRec
     *     A record with detailed information on the PHASE query
     * \return
     *     The phase function value, evaluated for each color channel
     */
    virtual float eval(const PhaseFunctionQueryRecord &pRec) const = 0;
    
    /**
     * \brief Compute the probability of sampling \c pRec.wo
     * (conditioned on \c pRec.wi).
     *
     * This method provides access to the probability density that
     * is realized by the \ref sample() method.
     *
     * \param pRec
     *     A record with detailed information on the PHASE query
     *
     * \return
     *     A probability/density value expressed with respect
     *     to the solid angle measure
     */
    
    virtual float pdf(const PhaseFunctionQueryRecord &pRec) const = 0;
    
    /**
     * \brief Return the type of object (i.e. Mesh/PHASE/etc.)
     * provided by this instance
     * */
    EClassType getClassType() const { return EPhaseFunction; }
};

NORI_NAMESPACE_END

#endif /* __PHASE_H */
