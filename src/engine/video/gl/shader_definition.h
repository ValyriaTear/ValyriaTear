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
    const char SOLID_VERTEX[] =
        "#version 120\n"
        "\n"
        "//\n"
        "// Vertex shader for solid sprite rendering.\n"
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

    const char SPRITE_VERTEX[] =
        "#version 120\n"
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
        "    gl_TexCoord[0].xy = in_TexCoords.xy;\n"
        "    gl_Position       = u_Projection * (u_View * (u_Model * vec4(in_Vertex, 1.0)));\n"
        "};";

    const char TEXT_VERTEX[] =
        "#version 120\n"
        "\n"
        "//\n"
        "// Vertex shader for text rendering.\n"
        "//\n"
        "\n"
        "uniform mat4 u_Model;\n"
        "uniform mat4 u_View;\n"
        "uniform mat4 u_Projection;\n"
        "\n"
        "attribute vec3 in_Vertex;\n"
        "attribute vec2 in_TexCoords;\n"
        "attribute vec4 in_Color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_TexCoord[0].xy = in_TexCoords.xy;\n"
        "    gl_FrontColor     = in_Color;\n"
        "    gl_Position       = u_Projection * (u_View * (u_Model * vec4(in_Vertex, 1.0)));\n"
        "};";

    const char SOLID_FRAGMENT[] =
        "#version 120\n"
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
        "        if (gl_FragColor.a <= 0.0f)\n"
        "        {\n"
        "            discard;\n"
        "        }\n"
        "\n"
        "}\n";

    const char SPRITE_FRAGMENT[] =
        "#version 120\n"
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
        "        if (gl_FragColor.a <= 0.0f)\n"
        "        {\n"
        "            discard;\n"
        "        }\n"
        "\n"
        "}\n";

    const char SPRITE_GRAYSCALE_FRAGMENT[] =
        "\n"
        "#version 120\n"
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
        "        if (gl_FragColor.a <= 0.0f)\n"
        "        {\n"
        "            discard;\n"
        "        }\n"
        "\n"
        "        // Greyscale filter\n"
        "        gl_FragColor.r *= 0.299f;\n"
        "        gl_FragColor.g *= 0.587f;\n"
        "        gl_FragColor.b *= 0.114f;\n"
        "\n"
        "        float sum = gl_FragColor.r + gl_FragColor.g + gl_FragColor.b;\n"
        "\n"
        "        gl_FragColor.r = sum;\n"
        "        gl_FragColor.g = sum;\n"
        "        gl_FragColor.b = sum;\n"
        "}\n";

    const char TEXT_FRAGMENT[] =
        "\n"
        "#version 120\n"
        "\n"
        "//\n"
        "// Colors the fonts.\n"
        "//\n"
        "\n"
        "uniform vec4 u_Color;\n"
        "uniform sampler2D u_Texture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 color = gl_Color * u_Color;\n"
        "\n"
        "    float a = texture2D(u_Texture, gl_TexCoord[0].xy).a;\n"
        "    gl_FragColor = vec4(color.rgb, color.a * a);\n"
        "}\n";

} // namespace shader_definition

} // namespace gl

} // namespace vt_video

#endif
