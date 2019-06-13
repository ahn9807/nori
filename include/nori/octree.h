//
//  octree.h
//  nori
//
//  Created by Junho on 13/06/2019.
//

#pragma once

#include <nori/bbox.h>

NORI_NAMESPACE_BEGIN

template <typename octreeData> struct OctNode {
    OctNode() {
        for(int i=0;i<8;i++) {
            children[i] = NULL;
        }
    }
    ~OctNode() {
        for(int i=0;i<8;i++) {
            delete children[i];
        }
    }
    OctNode* children[8];
}

NORI_NAMESPACE_END


