//
//  area.cpp
//  Half
//
//  Created by Junho on 19/06/2019.
//

#include <nori/emitter.h>
#include <nori/scene.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class AreaLight : public Emitter {
public:
    AreaLight(const PropertyList &props) {
        m_radiance = props.getColor("radiance");
    }
    
    std::string toString() const {
        return "Area Light";
    }
private:
    Color3f m_radiance;
};

NORI_REGISTER_CLASS(AreaLight, "simple");
NORI_NAMESPACE_END
