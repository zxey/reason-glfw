#include <stdio.h>

#include <caml/mlvalues.h>
#include <caml/bigarray.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/callback.h>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include <reglfw_image.h>

// From: https://stackoverflow.com/questions/23177229/how-to-cast-int-to-const-glvoid
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

extern "C" {

    void warn(const char *message) {
        printf("[WARNING]: %s\n", message);
    }

    GLenum variantToBlendFunc(value vVal) {
        switch (Int_val(vVal)) {
            case 0:
                return GL_ZERO;
            case 1:
                return GL_ONE;
            case 2:
                return GL_SRC_ALPHA;
            case 3:
                return GL_ONE_MINUS_SRC_ALPHA;
            default:
                warn("Unexpected option for glBlendFunc");
                return 0;
        }
    }

    GLenum variantToEnableOption(value vVal) {
        switch (Int_val(vVal)) {
            case 0:
                return GL_DEPTH_TEST;
            case 1:
                return GL_BLEND;
            case 2:
                return GL_SCISSOR_TEST;
            default:
                warn("Unexpected option for glEnable");
                return 0;
        }
    }

    GLenum variantToType(value vVal) {
        switch (Int_val(vVal)) {
            case 0:
                return GL_FLOAT;
            case 1:
                return GL_UNSIGNED_BYTE;
            case 2:
                return GL_UNSIGNED_SHORT;
            case 3:
                return GL_UNSIGNED_SHORT_5_6_5;
            case 4:
                return GL_UNSIGNED_SHORT_4_4_4_4;
            case 5:
                return GL_UNSIGNED_SHORT_5_5_5_1;
            default:
                warn("Unexpected GL type!");
                return 0;
        }
    }

    GLenum variantToBufferType(value vVal) {
        switch (Int_val(vVal)) {
            case 0:
                return GL_ARRAY_BUFFER;
            case 1:
                return GL_ELEMENT_ARRAY_BUFFER;
            default:
                warn("Unexpected buffer type!");
                return 0;
        }
    }

    GLenum variantToPixelAlignmentParameter(value vVal) {
        switch (Int_val(vVal)) {
            case 0:
                return GL_PACK_ALIGNMENT;
            case 1:
                return GL_UNPACK_ALIGNMENT;
            default:
                warn ("Unexpected pixel alignment parameter type!");
                return GL_PACK_ALIGNMENT;
        }
    }

    GLenum variantToTextureType(value vVal) {
        switch (Int_val(vVal)) {
            case 0:
                return GL_TEXTURE_2D;
            default:
                warn("Unexpected texture type!");
                return 0;
        }
    }

    GLenum variantToTextureParameter(value vVal) {
        switch (Int_val(vVal)) {
            case 0:
                return GL_TEXTURE_WRAP_S;
            case 1:
                return GL_TEXTURE_WRAP_T;
            case 2:
                return GL_TEXTURE_MIN_FILTER;
            case 3:
                return GL_TEXTURE_MAG_FILTER;
            default:
                warn("Unexpected texture parameter!");
                return 0;
        }
    }

    GLenum variantToTextureParameterValue(value vVal) {
        switch (Int_val(vVal)) {
            case 0:
                return GL_REPEAT;
            case 1:
                return GL_LINEAR;
            case 2:
                return GL_CLAMP_TO_EDGE;
            default:
                warn("Unexpected texture parameter value!");
                return 0;
        }
    }

    GLenum variantToDrawMode(value vDrawMode) {
        switch (Int_val(vDrawMode)) {
            case 0:
                return GL_TRIANGLES;
            case 1:
            default:
                return GL_TRIANGLE_STRIP;
        }
    }

    GLenum variantToFormat(value vFormat) {
        switch(Int_val(vFormat)) {
            case 0:
                return GL_ALPHA;
            case 1:
                return GL_LUMINANCE;
            case 2:
                return GL_LUMINANCE_ALPHA;
            case 3:
                return GL_RGB;
            case 4:
                return GL_RGBA;
            default:
                warn("Unsupported pixel format!");
                return 0;
        }
    }

    CAMLprim value
    caml_glClearColor(value vr, value vg, value vb, value va) {
        float r = Double_val(vr);
        float g = Double_val(vg);
        float b = Double_val(vb);
        float a = Double_val(va);
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
        return Val_unit;
    }

    CAMLprim value
    caml_glViewport(value vX, value vY, value vWidth, value vHeight) {
        int x = Int_val(vX);
        int y = Int_val(vY);
        int width = Int_val(vWidth);
        int height = Int_val(vHeight);
        glViewport(x, y, width, height);
        return Val_unit;
    }

    CAMLprim value
    caml_glClearDepth(value vd) {
        float d = Double_val(vd);
        glClearDepthf(d);
        glClear(GL_DEPTH_BUFFER_BIT);
        return Val_unit;
    }

    CAMLprim value
    caml_glEnable(value vEnableOptions) {
        glEnable(variantToEnableOption(vEnableOptions));
        return Val_unit;
    }

    CAMLprim value
    caml_glDisable(value vEnableOptions) {
        glDisable(variantToEnableOption(vEnableOptions));
        return Val_unit;
    }

    CAMLprim value
    caml_glScissor(value vX, value vY, value vWidth, value vHeight) {
        int x = Int_val(vX);
        int y = Int_val(vY);
        int width = Int_val(vWidth);
        int height = Int_val(vHeight);
        glScissor(x, y, width, height);
        return Val_unit;
    }

    CAMLprim value
    caml_glDepthFunc(value vDepthFunc) {
        glDepthFunc(GL_LEQUAL);
        return Val_unit;
    }

    CAMLprim value
    caml_glBlendFunc(value vSrcFunc, value vDestFunc) {
        glBlendFunc(variantToBlendFunc(vSrcFunc), variantToBlendFunc(vDestFunc));
        return Val_unit;
    }

    CAMLprim value
    caml_glCreateShader(value v) {
        int shaderType;
        switch (Int_val(v)) {
            case 0:
                shaderType = GL_VERTEX_SHADER;
                break;
            default:
            case 1:
                shaderType = GL_FRAGMENT_SHADER;
                break;
        }

        return (value) glCreateShader(shaderType);
    }

    CAMLprim value
    caml_glShaderSource(value vShader, value vSource) {
        GLuint shader = (GLuint)vShader;
        char *s;
        s = String_val(vSource);
        glShaderSource(shader, 1, &s, NULL);
        return Val_unit;
    }

    CAMLprim value
    caml_glCompileShader(value vShader) {
        CAMLparam1(vShader);
        GLuint shader = (GLuint)vShader;
        glCompileShader(shader);

        GLint result;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

        if (result == GL_TRUE) {
            CAMLreturn(Val_int(0));
        } else {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);

            CAMLlocal2(failure, log);
            failure = caml_alloc(1, 0);
            log = caml_copy_string(infoLog);
            Store_field(failure, 0, log);
            CAMLreturn(failure);
        }
    }

    CAMLprim value
    caml_glDeleteShader(value vShader) {
        GLuint shader = (GLuint)vShader;
        glDeleteShader(shader);
        return Val_unit;
    }

    CAMLprim value
    caml_glCreateProgram(value unit) {
        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();
        return (value)shaderProgram;
    }

    CAMLprim value
    caml_glAttachShader(value vProgram, value vShader) {
        unsigned int shaderProgram = (unsigned int)vProgram;
        unsigned int shader = (unsigned int)vShader;
        glAttachShader(shaderProgram, shader);
        return Val_unit;
    }

    CAMLprim value
    caml_glLinkProgram(value vProgram) {
        CAMLparam1(vProgram);
        unsigned int shaderProgram = (unsigned int)vProgram;
        glLinkProgram(shaderProgram);

        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

        if (success == GL_TRUE) {
            CAMLreturn(Val_int(0));
        } else {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

            CAMLlocal2(failure, log);
            failure = caml_alloc(1, 0);
            log = caml_copy_string(infoLog);
            Store_field(failure, 0, log);
            CAMLreturn(failure);
        }
    }

    CAMLprim value
    caml_glGetAttribLocation(value vProgram, value vAttributeName) {
        unsigned int shaderProgram = (unsigned int)vProgram;
        char *s;
        s = String_val(vAttributeName);
        int val = glGetAttribLocation(shaderProgram, s);
        return (value)val;
    }

    CAMLprim value
    caml_glGetUniformLocation(value vProgram, value vUniformName) {
        unsigned int shaderProgram = (unsigned int)vProgram;
        char *s;
        s = String_val(vUniformName);

        int val = glGetUniformLocation(shaderProgram, s);
        return (value)val;
    }

    CAMLprim value
    caml_glUniform1f(value vUniformLocation, value v0) {
        float f0 = Double_val(v0);
        int iUniformLocation = (int)vUniformLocation;

        glUniform1f(iUniformLocation, f0);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform2f(value vUniformLocation, value v0, value v1) {
        float f0 = Double_val(v0);
        float f1 = Double_val(v1);
        int iUniformLocation = (int)vUniformLocation;

        glUniform2f(iUniformLocation, f0, f1);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform3f(value vUniformLocation, value v0, value v1, value v2) {
        float f0 = Double_val(v0);
        float f1 = Double_val(v1);
        float f2 = Double_val(v2);
        int iUniformLocation = (int)vUniformLocation;

        glUniform3f(iUniformLocation, f0, f1, f2);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform4f(value vUniformLocation, value v0, value v1, value v2, value v3) {
        float f0 = Double_val(v0);
        float f1 = Double_val(v1);
        float f2 = Double_val(v2);
        float f3 = Double_val(v3);
        int iUniformLocation = (int)vUniformLocation;

        glUniform4f(iUniformLocation, f0, f1, f2, f3);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform1i(value vUniformLocation, value v0) {
        int i0 = Int_val(v0);
        int iUniformLocation = (int)vUniformLocation;

        glUniform1i(iUniformLocation, i0);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform2i(value vUniformLocation, value v0, value v1) {
        int i0 = Int_val(v0);
        int i1 = Int_val(v1);
        int iUniformLocation = (int)vUniformLocation;

        glUniform2i(iUniformLocation, i0, i1);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform3i(value vUniformLocation, value v0, value v1, value v2) {
        int i0 = Int_val(v0);
        int i1 = Int_val(v1);
        int i2 = Int_val(v2);
        int iUniformLocation = (int)vUniformLocation;

        glUniform3i(iUniformLocation, i0, i1, i2);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform4i(value vUniformLocation, value v0, value v1, value v2, value v3) {
        int i0 = Int_val(v0);
        int i1 = Int_val(v1);
        int i2 = Int_val(v2);
        int i3 = Int_val(v3);
        int iUniformLocation = (int)vUniformLocation;

        glUniform4i(iUniformLocation, i0, i1, i2, i3);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform2fv(value vUniformLocation, value vVec2) {
        float *vec2 = (float *)(Data_custom_val(vVec2));
        int uloc = (int)vUniformLocation;

        glUniform2fv(uloc, 1, vec2);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform3fv(value vUniformLocation, value vVec3) {
        float *vec3 = (float *)(Data_custom_val(vVec3));
        int uloc = (int)vUniformLocation;

        glUniform3fv(uloc, 1, vec3);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniform4fv(value vUniformLocation, value vVec4) {
        float *vec4 = (float *)(Data_custom_val(vVec4));
        int uloc = (int)vUniformLocation;

        glUniform4fv(uloc, 1, vec4);
        return Val_unit;
    }

    CAMLprim value
    caml_glUniformMatrix4fv(value vUniformLocation, value vMat4) {
        float *mat4 = (float *)(Data_custom_val(vMat4));
        int uloc = (int)vUniformLocation;

        glUniformMatrix4fv(uloc, 1, GL_FALSE, mat4);
        return Val_unit;
    }

    CAMLprim value
    caml_glPixelStorei(value vPixelAlignmentParameter, value vParam) {
        glPixelStorei(variantToPixelAlignmentParameter(vPixelAlignmentParameter), Int_val(vParam));
        return Val_unit;
    }

    CAMLprim value
    caml_glCreateTexture(value vUnit) {
        unsigned int texture;
        glGenTextures(1, &texture);
        return (value)texture;
    }

    CAMLprim value
    caml_glBindTexture(value vTextureType, value vTexture) {
        unsigned int texture = (unsigned int)vTexture;
        glBindTexture(variantToTextureType(vTextureType), texture);
        return Val_unit;
    }

    CAMLprim value
    caml_glTexImage2D(
            value vTextureType,
            value vLevel,
            value vInternalFormat,
            value vFormat,
            value vType,
            value vPixels) {
        CAMLparam5(vTextureType, vLevel, vInternalFormat, vFormat, vType);
        CAMLxparam1(vPixels);

        GLsizei width = Caml_ba_array_val(vPixels)->dim[0];
        GLsizei height = Caml_ba_array_val(vPixels)->dim[1];
        GLvoid *pPixels = (GLvoid *)Caml_ba_data_val(vPixels);

        printf("target: %u, level: %i, internalFormat: %u, width: %i, height: %i, format: %u, type: %u, pPixels: %p\n",
                variantToTextureType(vTextureType), Int_val(vLevel), variantToFormat(vInternalFormat), width, height,
                variantToFormat(vFormat), variantToType(vType), pPixels);

        glTexImage2D(
                variantToTextureType(vTextureType),
                Int_val(vLevel),
                variantToFormat(vInternalFormat),
                width,
                height,
                0,
                variantToFormat(vFormat),
                variantToType(vType),
                pPixels);

        CAMLreturn(Val_unit);
    }

    CAMLprim value
    caml_glTexImage2D_extension(value *argv, int argn)
    {
        return caml_glTexImage2D(
                argv[0],
                argv[1],
                argv[2],
                argv[3],
                argv[4],
                argv[5]);
    }

    CAMLprim value
    caml_glTexParameteri(value vTextureType, value vTextureParameter, value vTextureParameterValue) {
        glTexParameteri(
                variantToTextureType(vTextureType),
                variantToTextureParameter(vTextureParameter),
                variantToTextureParameterValue(vTextureParameterValue));
        return Val_unit;
    }

    CAMLprim value
    caml_glGenerateMipmap(value vTextureType) {
        glGenerateMipmap(variantToTextureType(vTextureType));
        return Val_unit;
    }

    CAMLprim value
    caml_glUseProgram(value vProgram) {
        unsigned int program = (unsigned int)vProgram;
        glUseProgram(program);
        return Val_unit;
    }

    CAMLprim value
    caml_glCreateBuffer(value unit) {
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        return (value)VBO;
    }

    CAMLprim value
    caml_glBindBuffer(value vBufferType, value vBuffer) {
        unsigned int VBO = (unsigned int)vBuffer;
        glBindBuffer(variantToBufferType(vBufferType), VBO);

        return Val_unit;
    }

    CAMLprim value
    caml_glBufferData(value vBufferType, value vData, value drawType) {
        int size = Caml_ba_array_val(vData)->dim[0];

        if ((Caml_ba_array_val(vData)->flags & CAML_BA_UINT16) == CAML_BA_UINT16) {
            unsigned short* elements = (unsigned short*)Caml_ba_data_val(vData);
            glBufferData(variantToBufferType(vBufferType), size * sizeof(unsigned short), elements, GL_STATIC_DRAW);
        } else if ((Caml_ba_array_val(vData)->flags & CAML_BA_FLOAT32) == CAML_BA_FLOAT32) {
            float* elements = (float*)Caml_ba_data_val(vData);
            glBufferData(variantToBufferType(vBufferType), size * sizeof(float), elements, GL_STATIC_DRAW);
        } else {
            warn("Unexpected Bigarray type!");
        }
        return Val_unit;
    }

    CAMLprim value
    caml_glDrawArrays(value vDrawMode, value vFirst, value vCount) {
        unsigned int first = Int_val(vFirst);
        unsigned int count = Int_val(vCount);
        glDrawArrays(variantToDrawMode(vDrawMode), first, count);
        return Val_unit;
    }

    CAMLprim value
    caml_glDrawElements(value vDrawMode, value vCount, value vGlType, value vFirst) {
       GLenum drawMode = variantToDrawMode(vDrawMode);
       GLenum dataType = variantToType(vGlType);
       unsigned int count = Int_val(vCount);
       unsigned int first = Int_val(vFirst);
       glDrawElements(drawMode, vCount, dataType, BUFFER_OFFSET(first));
       return Val_unit;
    }

    CAMLprim value
    caml_glEnableVertexAttribArray(value vIndex) {
        int index = (int)(vIndex);
        glEnableVertexAttribArray(index);
        return Val_unit;
    }

    CAMLprim value
    caml_glVertexAttribPointer(
            value vIndex,
            value vSize,
            value vGlType,
            value vNormalized,
            value vStride,
            value vOffset) {
        CAMLparam5(vIndex, vSize, vGlType, vNormalized, vStride);
        CAMLxparam1(vOffset);

        GLuint index = (GLuint)vIndex;
        GLint size = Int_val(vSize);
        GLenum glType = variantToType(vGlType);
        GLboolean normalized = Bool_val(vNormalized);
        GLsizei stride = Int_val(vStride);
        void* pointer = (void *) Int_val(vOffset);

        printf("index: %u, size: %i, type: %u, normalized: %c, stride: %i, pointer: %p\n",
                index, size, glType, normalized, stride, pointer);
        glVertexAttribPointer(index, size, glType, normalized, stride, pointer);

        CAMLreturn(Val_unit);
    }

    CAMLprim value
    caml_glVertexAttribPointer_extension(value *argv, int argn)
    {
        return caml_glVertexAttribPointer(
                argv[0],
                argv[1],
                argv[2],
                argv[3],
                argv[4],
                argv[5]);
    }

    CAMLprim value
    caml_glUnbindBuffer(value unit) {
        printf("TODO: glUnbindBuffer\n");
        return Val_unit;
    }

    /* 2 copies of this stub, since the OCaml runtime requires a native
       version & a bytecode version for functions with >5 parameters. */
    CAMLprim value
    caml_glReadPixels_native(value vX, value vY, value vWidth, value vHeight,
                             value vFormat, value vType, value vData) {
      CAMLparam5(vX, vY, vWidth, vHeight, vFormat);
      CAMLxparam2(vType, vData);

      GLint x = (GLint) Int_val(vX);
      GLint y = (GLint) Int_val(vY);

      GLsizei width = (GLsizei) Int_val(vWidth);
      GLsizei height = (GLsizei) Int_val(vHeight);

      GLenum format = variantToFormat(vFormat);
      GLenum type = variantToType(vType);

      void *data = (void *) vData;

      glReadPixels(x, y, width, height, format, type, data);

      // If we're on a little-endian system, the R/B channels are swapped
      // So let's determine endianness...
      unsigned int marker = 0x12345678;
      if (*(char *) &marker == 0x78 && type == GL_UNSIGNED_BYTE) {
        // We are little-endian. Onto the swap...
        int numChannels = format == GL_RGBA ? 4 : 3;
        for (int i = 0; i < width * height * numChannels; i += numChannels) {
          uint8_t tmp = *((uint8_t *) data + i);
          *((uint8_t *) data + i) = *((uint8_t *) data + i + 2);
          *((uint8_t *) data + i + 2) = tmp;
        }
      }

      CAMLreturn(Val_unit);
    }

    CAMLprim value
    caml_glReadPixels_bytecode(value * argv, int argc) {
      value vX = argv[0], vY = argv[1], vWidth = argv[2],
            vHeight = argv[3], vFormat = argv[4],
            vType = argv[5], vData = argv[6];
      CAMLparam5(vX, vY, vWidth, vHeight, vFormat);
      CAMLxparam2(vType, vData);

      GLint x = (GLint) Int_val(vX);
      GLint y = (GLint) Int_val(vY);

      GLsizei width = (GLsizei) Int_val(vWidth);
      GLsizei height = (GLsizei) Int_val(vHeight);

      GLenum format = variantToFormat(vFormat);
      GLenum type = variantToType(vType);

      void *data = (void *) vData;

      glReadPixels(x, y, width, height, format, type, data);

      // If we're on a little-endian system, the R/B channels are swapped
      // So let's determine endianness...
      unsigned int marker = 0x12345678;
      if (*(char *) &marker == 0x78 && type == GL_UNSIGNED_BYTE) {
        // We are little-endian. Onto the swap...
        int numChannels = format == GL_RGBA ? 4 : 3;
        for (int i = 0; i < width * height * numChannels; i += numChannels) {
          uint8_t tmp = *((uint8_t *) data + i);
          *((uint8_t *) data + i) = *((uint8_t *) data + i + 2);
          *((uint8_t *) data + i + 2) = tmp;
        }
      }

      CAMLreturn(Val_unit);
    }

    CAMLprim value
    caml_reglfwTexImage2D(value vTextureType, value vImage) {

        ReglfwImageInfo *pImage = (ReglfwImageInfo *)vImage;

        GLenum format;
        switch (pImage->numChannels) {
            case 1:
                format = GL_ALPHA;
                break;
            case 2:
                format = GL_LUMINANCE_ALPHA;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
            default:
                format = GL_RGBA;
        }

        glTexImage2D(
                variantToTextureType(vTextureType),
                0, // TODO: Support for specifying the level of detail
                format,
                pImage->width,
                pImage->height,
                0,
                format,
                GL_UNSIGNED_BYTE,  // TODO: Support for floating-point textures!
                pImage->data);
        return Val_unit;
    }
}
