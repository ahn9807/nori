//
//  texture.cpp
//  Half
//
//  Created by junho on 2019. 7. 24..
//

#include <nori/texture.h>

NORI_NAMESPACE_BEGIN

Texture::Texture(std::string filepath) {
    Bitmap textureMap = Bitmap(filepath);
    imageWidht = (int)textureMap.cols();
    imageHeight = (int)textureMap.rows();
    texture = new std::vector<Color3f>();
    texture->reserve(imageWidht * imageHeight);
    for(int y=0;y<imageHeight;y++) {
        for(int x=0;x<imageWidht;x++) {
            texture->push_back(textureMap.coeffRef(y, x));
        }
    }
    bitmap = textureMap;
}

Color3f Texture::lookUp(Point2f uv) {
    int imageU = (int)(uv.x() * imageWidht - 0.5) % imageWidht;
    int imageV = (int)(uv.y() * imageHeight - 0.5) % imageHeight;
    
    return bitmap.coeffRef(imageV, imageU);
}

NORI_NAMESPACE_END
