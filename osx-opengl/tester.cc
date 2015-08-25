#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <stdio.h>
#include <iostream>
#include <fstream>


int main(int argc, char **argv) 
{
     std::cout << "Setting up opengl context." << std::endl;
     CGLContextObj     ctx;
     CGLPixelFormatObj pix;
     GLint             npix;

     CGLPixelFormatAttribute attribs[13] =
     {
         /* This generates a core context */
         kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core,
         kCGLPFAColorSize,     (CGLPixelFormatAttribute)24,
         kCGLPFAAlphaSize,     (CGLPixelFormatAttribute)8,
         kCGLPFAAccelerated,
         kCGLPFADoubleBuffer,
         kCGLPFASampleBuffers, (CGLPixelFormatAttribute)1,
         kCGLPFASamples,       (CGLPixelFormatAttribute)4,
         (CGLPixelFormatAttribute)0
     };

     CGLChoosePixelFormat( attribs, &pix, &npix );
     CGLCreateContext( pix, NULL, &ctx );
     CGLSetCurrentContext( ctx );

     printf("        Vendor:   %s\n", glGetString(GL_VENDOR)                  );
     printf("        Renderer: %s\n", glGetString(GL_RENDERER)                );
     printf("        Version:  %s\n", glGetString(GL_VERSION)                 );
     printf("        GLSL:     %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


    std::size_t i = 2;
    float f = 1.333f;
    std::cout << i * f << std::endl;

    std::ofstream file("tester.txt", std::ios::binary);
    int myi = 100;
    file << myi << std::endl;
    

    
  return 0; 
}
