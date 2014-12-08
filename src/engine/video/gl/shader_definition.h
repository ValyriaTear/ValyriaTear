
////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shader_definition.h
*** \author  Authenticate, James Lammlein
*** \brief   Header file for shader definitions.
*** ***************************************************************************/

#ifndef __SHADER_DEFINITION_HEADER__
#define __SHADER_DEFINITION_HEADER__

namespace vt_video
{
namespace gl
{
namespace shader_definition
{
    const char PARTICLE_VERTEX[] =
        "#version 110\n"
        "\n"
        "//\n"
        "// Vertex shader for particle sprite rendering.\n"
        "//\n"
        "\n"
        "uniform mat4 u_Model;\n"
        "uniform mat4 u_View;\n"
        "uniform mat4 u_Projection;\n"
        "\n"
        "attribute vec3 in_Vertex;\n"
        "attribute vec4 in_Color;\n"
        "attribute vec2 in_TexCoords;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position       = u_Projection * (u_View * (u_Model * vec4(in_Vertex, 1.0)));\n"
        "    gl_FrontColor     = in_Color;\n"
        "    gl_TexCoord[0].xy = in_TexCoords.xy;\n"
        "};";

    const char SOLID_VERTEX[] =
        "#version 110\n"
        "\n"
        "//\n"
        "// Vertex shader for solid color sprite rendering.\n"
        "//\n"
        "\n"
        "uniform mat4 u_Model;\n"
        "uniform mat4 u_View;\n"
        "uniform mat4 u_Projection;\n"
        "\n"
        "attribute vec3 in_Vertex;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position       = u_Projection * (u_View * (u_Model * vec4(in_Vertex, 1.0)));\n"
        "};";

    const char SOLID_PER_VERTEX[] =
        "#version 110\n"
        "\n"
        "//\n"
        "// Vertex shader for solid color per vertex sprite rendering.\n"
        "//\n"
        "\n"
        "uniform mat4 u_Model;\n"
        "uniform mat4 u_View;\n"
        "uniform mat4 u_Projection;\n"
        "\n"
        "attribute vec3 in_Vertex;\n"
        "attribute vec4 in_Color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position       = u_Projection * (u_View * (u_Model * vec4(in_Vertex, 1.0)));\n"
        "    gl_FrontColor     = in_Color;\n"
        "};";

    const char SPRITE_VERTEX[] =
        "#version 110\n"
        "\n"
        "//\n"
        "// Vertex shader for textured sprite rendering.\n"
        "//\n"
        "\n"
        "uniform mat4 u_Model;\n"
        "uniform mat4 u_View;\n"
        "uniform mat4 u_Projection;\n"
        "\n"
        "attribute vec3 in_Vertex;\n"
        "attribute vec2 in_TexCoords;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position       = u_Projection * (u_View * (u_Model * vec4(in_Vertex, 1.0)));\n"
        "    gl_TexCoord[0].xy = in_TexCoords.xy;\n"
        "};";

    const char PARTICLE_FRAGMENT[] =
        "#version 110\n"
        "\n"
        "//\n"
        "// Colors a particle's fragment.\n"
        "//\n"
        "\n"
        "uniform sampler2D u_Texture;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "        gl_FragColor.rgba = vec4(texture2D(u_Texture, gl_TexCoord[0].xy));\n"
        "        gl_FragColor *= gl_Color;\n"
        "\n"
        "        // Alpha Test\n"
        "        if (gl_FragColor.a <= 0.0)\n"
        "        {\n"
        "            discard;\n"
        "        }\n"
        "\n"
        "}\n";

    const char SOLID_FRAGMENT[] =
        "#version 110\n"
        "\n"
        "//\n"
        "// Colors the fragment.\n"
        "//\n"
        "\n"
        "uniform vec4 u_Color;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "        gl_FragColor = u_Color;\n"
        "\n"
        "        // Alpha Test\n"
        "        if (gl_FragColor.a <= 0.0)\n"
        "        {\n"
        "            discard;\n"
        "        }\n"
        "\n"
        "}\n";

    const char SOLID_PER_FRAGMENT[] =
        "#version 110\n"
        "\n"
        "//\n"
        "// Colors the fragment.\n"
        "//\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "        gl_FragColor = gl_Color;\n"
        "\n"
        "        // Alpha Test\n"
        "        if (gl_FragColor.a <= 0.0)\n"
        "        {\n"
        "            discard;\n"
        "        }\n"
        "\n"
        "}\n";

    const char SPRITE_FRAGMENT[] =
        "#version 110\n"
        "\n"
        "//\n"
        "// Samples a texture for fragment's output.\n"
        "//\n"
        "\n"
        "uniform vec4 u_Color;\n"
        "uniform sampler2D u_Texture;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "        gl_FragColor.rgba = vec4(texture2D(u_Texture, gl_TexCoord[0].xy));\n"
        "        gl_FragColor *= u_Color;\n"
        "\n"
        "        // Alpha Test\n"
        "        if (gl_FragColor.a <= 0.0)\n"
        "        {\n"
        "            discard;\n"
        "        }\n"
        "\n"
        "}\n";

    const char SPRITE_GRAYSCALE_FRAGMENT[] =
        "\n"
        "#version 110\n"
        "\n"
        "//\n"
        "// Samples texture for fragment's output and converts to grayscale.\n"
        "//\n"
        "\n"
        "uniform vec4 u_Color;\n"
        "uniform sampler2D u_Texture;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "        gl_FragColor.rgba = vec4(texture2D(u_Texture, gl_TexCoord[0].xy));\n"
        "        gl_FragColor *= u_Color;\n"
        "\n"
        "        // Alpha Test\n"
        "        if (gl_FragColor.a <= 0.0)\n"
        "        {\n"
        "            discard;\n"
        "        }\n"
        "\n"
        "        // Grayscale filter\n"
        "        gl_FragColor.r *= 0.299;\n"
        "        gl_FragColor.g *= 0.587;\n"
        "        gl_FragColor.b *= 0.114;\n"
        "\n"
        "        float sum = gl_FragColor.r + gl_FragColor.g + gl_FragColor.b;\n"
        "\n"
        "        gl_FragColor.r = sum;\n"
        "        gl_FragColor.g = sum;\n"
        "        gl_FragColor.b = sum;\n"
        "}\n";

} // namespace shader_definition

} // namespace gl

} // namespace vt_video

#endif
