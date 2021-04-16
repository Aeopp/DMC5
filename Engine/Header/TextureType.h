#ifndef _TEXTURETYPE_H
#define _TEXTURETYPE_H
#include "EngineDefine.h"
#include "EngineTypedef.h"


enum TextureType : uint8
{
    /** Dummy value.
     *
     *  No texture, but the value to be used as 'texture semantic'
     *  (#aiMaterialProperty::mSemantic) for all material properties
     *  *not* related to textures.
     */
    NONE = 0,

    /** LEGACY API MATERIALS
     * Legacy refers to materials which
     * Were originally implemented in the specifications around 2000.
     * These must never be removed, as most engines support them.
     */

     /** The texture is combined with the result of the diffuse
      *  lighting equation.
      */
      DIFFUSE = 1,

      /** The texture is combined with the result of the specular
       *  lighting equation.
       */
      SPECULAR = 2,

       /** The texture is combined with the result of the ambient
        *  lighting equation.
        */
        AMBIENT = 3,

        /** The texture is added to the result of the lighting
         *  calculation. It isn't influenced by incoming light.
         */
         EMISSIVE = 4,

         /** The texture is a height map.
          *
          *  By convention, higher gray-scale values stand for
          *  higher elevations from the base height.
          */
          HEIGHT = 5,

          /** The texture is a (tangent space) normal-map.
           *
           *  Again, there are several conventions for tangent-space
           *  normal maps. Assimp does (intentionally) not
           *  distinguish here.
           */
          NORMALS = 6,

           /** The texture defines the glossiness of the material.
            *
            *  The glossiness is in fact the exponent of the specular
            *  (phong) lighting equation. Usually there is a conversion
            *  function defined to map the linear color values in the
            *  texture to a suitable exponent. Have fun.
           */
          SHININESS = 7,

           /** The texture defines per-pixel opacity.
            *
            *  Usually 'white' means opaque and 'black' means
            *  'transparency'. Or quite the opposite. Have fun.
           */
         OPACITY = 8,

           /** Displacement texture
            *
            *  The exact purpose and format is application-dependent.
            *  Higher color values stand for higher vertex displacements.
           */
           DISPLACEMENT = 9,

           /** Lightmap texture (aka Ambient Occlusion)
            *
            *  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
            *  covered by this material property. The texture contains a
            *  scaling value for the final color value of a pixel. Its
            *  intensity is not affected by incoming light.
           */
          LIGHTMAP = 10,

           /** Reflection texture
            *
            * Contains the color of a perfect mirror reflection.
            * Rarely used, almost never for real-time applications.
           */
           REFLECTION = 11,

           /** PBR Materials
            * PBR definitions from maya and other modelling packages now use this standard.
            * This was originally introduced around 2012.
            * Support for this is in game engines like Godot, Unreal or Unity3D.
            * Modelling packages which use this are very common now.
            */

            BASE_COLOR = 12,
            NORMAL_CAMERA = 13,
            EMISSION_COLOR = 14,
            METALNESS = 15,
            DIFFUSE_ROUGHNESS = 16,
            AMBIENT_OCCLUSION = 17,

            /** Unknown texture
             *
             *  A texture reference that does not match any of the definitions
             *  above is considered to be 'unknown'. It is still imported,
             *  but is excluded from any further post-processing.
            */
            UNKNOWN = 18,
};

static inline std::string GetTextureTypeName(const uint8& _Type)
{
    switch (_Type)
    {
    case NONE:
        return "NONE";
        break;
    case DIFFUSE:
        return "DIFFUSE";
        break;
    case SPECULAR:
        return "SPECULAR";
        break;
    case AMBIENT:
        return "AMBIENT";
        break;
    case EMISSIVE:
        return "EMISSIVE";
        break;
    case HEIGHT:
        return "HEIGHT";
        break;
    case NORMALS:
        return "NORMALS";
        break;
    case SHININESS:
        return "SHININESS";
        break;
    case OPACITY:
        return "OPACITY";
        break;
    case DISPLACEMENT:
        return "DISPLACEMENT";
        break;
    case LIGHTMAP:
        return "LIGHTMAP";
        break;
    case REFLECTION:
        return "REFLECTION";
        break;
    case BASE_COLOR:
        return "BASE_COLOR";
        break;
    case NORMAL_CAMERA:
        return "NORMAL_CAMERA";
        break;
    case EMISSION_COLOR:
        return "EMISSION_COLOR";
        break;
    case METALNESS:
        return "METALNESS";
        break;
    case DIFFUSE_ROUGHNESS:
        return "DIFFUSE_ROUGHNESS";
        break;
    case AMBIENT_OCCLUSION:
        return "AMBIENT_OCCLUSION";
        break;
    case UNKNOWN:
        return "UNKNOWN";
        break;
    default:
        return "UNKNOWN";
        break;
    }
    return "UNKNOWN";
}

#endif // !_TEXTURETYPE_H

