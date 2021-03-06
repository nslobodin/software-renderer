/*
 * material.h
 *
 *      Author: flamingo
 *      E-mail: epiforce57@gmail.com
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "../base/resource.h"

namespace rend
{

class Texture;

//! Surface properties.
/*!
  * Holds data about shading mode, diffuse,
  * ambient, specular and emissive reflection coefficients,
  * texture and the list goes on.
  */
struct Material : public base::Resource
{
    enum ShadeMode
    {
        SM_UNDEFINED,
        SM_WIRE,
        SM_PLAIN_COLOR,
        SM_FLAT,
        SM_GOURAUD,
        SM_PHONG,
        SM_TEXTURE
    };

    //! Triangle side type.
    enum SideType
    {
        ONE_SIDE,
        TWO_SIDE
    };

    ShadeMode shadeMode;
    SideType sideType;

    Color3 plainColor;
    Color3 ambientColor;
    Color3 diffuseColor;
    Color3 specularColor;
    Color3 emissiveColor;

    //! Alpha for material [0..255]
    int alpha;

    std::string textureName;
    sptr(Texture) texture;

    //! Default ctor.
    Material();

    sptr(Material) clone() const;
};

}

#endif // MATERIAL_H
