#pragma once

#include <vector>

namespace pathtrace {

class IHittable;

/**
 * The bvh construction is based on simple binary tree
 * ref: https://zhuanlan.zhihu.com/p/479241523?utm_id=0
*/
IHittable* build_bvh(std::vector<IHittable*>& objects);

/**
 * clear all non-leaf nodes in the bvh (scene), leaf nodes are not touched
*/
void clear_bvh(IHittable* root);

}
