//
//  texture.h
//  nori
//
//  Created by junho on 2019. 7. 24..
//

#pragma once

#include <nori/object.h>
#include <nori/bitmap.h>

NORI_NAMESPACE_BEGIN

class Texture {
public:
    Texture(Color3f defaultVal, std::string path = "none");
    Color3f lookUp(Point2f uv);
    
private:
    std::vector<Color3f>* texture;
    int imageWidht;
    int imageHeight;
    Bitmap *bitmap;
    Color3f defaultVal;
};

NORI_NAMESPACE_END
