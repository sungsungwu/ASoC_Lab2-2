#pragma once

#include <ac_int.h>
#include <ac_fixed.h>
#include <ac_channel.h>
#include <ac_math.h>
#include <ac_math/ac_sqrt_pwl.h>
#include <ac_math/ac_atan2_cordic.h>

  const int maxImageWidth  = 640; 
  const int maxImageHeight = 480; 
  
  const int kernel_2[3] = {1, 0, -1};

  // Define some bit-accurate types to use in this model
  typedef uint8                  pixelType;    // input pixel is 0-255
  typedef uint16                 pixelType2x;  // two pixels packed
  typedef uint32                 pixelType4x;  // four pixels packed
  typedef ac_int<64,false>       pixelType8x;  // eight pixels packed
  typedef int9                   gradType;     // Derivative is max range -255 to 255
  typedef int36                  gradType4x;   // four 9-bit Derivative is max range -255 to 255
  typedef uint18                 sqType;       // Result of 9-bit x 9-bit
  typedef ac_fixed<19,19,false>  sumType;      // Result of 18-bit + 18-bit fixed pt integer for squareroot
  //typedef uint9                  magType;    // 9-bit unsigned magnitute result
  typedef uint8                  magType;      // 8-bit unsigned magnitute result
  typedef uint32                 magType4x;    // four 8-bit unsigned magnitute result
  typedef ac_fixed<8,3,true>     angType;      // 3 integer bit, 5 fractional bits for quantized angle -pi to pi
    
  // Compute number of bits for max image size count, used internally and in testbench
  typedef ac_int<ac::nbits<maxImageWidth+1>::val,false> maxWType;
  typedef ac_int<ac::nbits<maxImageHeight+1>::val,false> maxHType;
  
  struct Stream_t {
    pixelType4x pix;
    bool        sof;
    bool        eol;
  };
  


