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

#include <nori/warp.h>
#include <nori/vector.h>
#include <nori/frame.h>


NORI_NAMESPACE_BEGIN

Point2f Warp::squareToUniformSquare(const Point2f &sample) {
    return sample;
}

float Warp::squareToUniformSquarePdf(const Point2f &sample) {
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

Point2f Warp::squareToTent(const Point2f &sample) {
    Point2f returnPoint;
    for(int i=0;i<2;i++) {
        if(sample[i] < 0.5f)
            returnPoint[i] = sqrt(2*sample[i]) - 1;
        else
            returnPoint[i] = 1 - sqrt(2-2*sample[i]);
    }
    
    return returnPoint;
}

float Warp::squareToTentPdf(const Point2f &p) {
    float t[2];
    for(int i=0;i<2;i++) {
        if(p[i] <= 1 && p[i] >= -1)
            t[i] = p[i];
        else
            t[i] = 0;
    }
    return (1-abs(t[0])) * (1-abs(t[1]));
}

Point2f Warp::squareToUniformDisk(const Point2f &sample) {
    return Point2f(sqrt(sample[0]) * cos(2*M_PI*sample[1]), sqrt(sample[0]) * sin(2*M_PI*sample[1]));
}

float Warp::squareToUniformDiskPdf(const Point2f &p) {
    if(sqrt(p[0] * p[0] + p[1] * p[1]) > 1) {
        return 0;
    }
    else
        return 1.f/M_PI;
}

Vector3f Warp::squareToUniformSphere(const Point2f &sample) {
    double theta = 2 * M_PI * sample[0];
    double phi = acos(1 - 2 * sample[1]);
    
    return Vector3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

float Warp::squareToUniformSpherePdf(const Vector3f &v) {
    if(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) > 1)
        return 0.f;
    
    return 1.f/(4*M_PI);
}

Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) {
    double theta = 2 * M_PI * sample[0];
    double phi = acos(1 - sample[1]);
    
    return Vector3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

float Warp::squareToUniformHemispherePdf(const Vector3f &v) {
    if(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) > 1 || v[2] < 0)
        return 0.f;
    
    return 1.f/(2*M_PI);
}

Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) {
    Point2f d = squareToUniformDisk(sample);
    float z = sqrt(std::max(0.f, 1 - d.x() * d.x() - d.y() * d.y()));
    return Vector3f(d.x(),d.y(),z);
}

float Warp::squareToCosineHemispherePdf(const Vector3f &v) {
    if(v[2] < 0)
        return 0.f;
    
    return v[2] / M_PI;
}

Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    float phi = 2*M_PI*sample[0];
    float invAlpha2 = 1.f/alpha; invAlpha2 *= invAlpha2;
    float theta = acos(sqrt(1/(1-alpha*alpha*log(sample[1]))));
    
    return Vector3f(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
}

float Warp::squareToBeckmannPdf(const Vector3f &m, float alpha) {
    float cosTheta = Frame::cosTheta(m);
    float tanTheta = Frame::tanTheta(m);
    
    if(cosTheta <= 0)
        return 0.f;
    
    float azimuthal = 0.5 * INV_PI;
    float longitudinal = 2*exp((-tanTheta*tanTheta)/(alpha*alpha))/(alpha*alpha*pow(cosTheta,3));
    
    return azimuthal * longitudinal;
}

NORI_NAMESPACE_END
