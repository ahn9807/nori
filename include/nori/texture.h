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
    Texture(std::string path);
    Color3f lookUp(Point2f uv);
    
private:
    std::vector<Color3f>* texture;
    int imageWidht;
    int imageHeight;
    Bitmap bitmap;
};

NORI_NAMESPACE_END
