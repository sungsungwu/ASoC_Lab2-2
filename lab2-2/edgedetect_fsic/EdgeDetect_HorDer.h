#pragma once

#include "EdgeDetect_defs.h"
#include <mc_scverify.h>

  class EdgeDetect_HorDer
  {
  public:
    EdgeDetect_HorDer() {}
  
    #pragma hls_design interface
    void CCS_BLOCK(run)(ac_channel<pixelType4x> &pix_in,
                        maxWType                &widthIn,
                        maxHType                &heightIn,
                        ac_channel<pixelType4x> &pix_out,
                        ac_channel<gradType4x>  &dx)
    {
      pixelType pix_buf0;
      pixelType pix_buf1;

      pixelType4x pix4; 
      pixelType   pixels[9];
       
      gradType4x grad;
      maxWType   A; // x for 4 pixels
  
      // 4pixels
      HROW: for (maxHType y = 0; ; y++) {
        #pragma hls_pipeline_init_interval 1
        HCOL: for (maxWType x = 0; ; x += 4) { 
          A = x / 4; // 4 pixels

          if (A <= (widthIn / 4) - 1)
            pix4 = pix_in.read();

          if (A == 1) // left boundary
          {
            #pragma hls_unroll yes
            for (int i = 1; i < 5; i++) {
              pixels[i] = pixels[i + 4];
            }
            #pragma hls_unroll yes
            for (int i = 0; i < 4; i++) {
              pixels[i + 5] = pix4.slc<8>(i * 8);
            }
            pixels[0] = pixels[2];
          }
          else if (A == (widthIn / 4)) //right boundary
          {
            #pragma hls_unroll yes
            for (int i = 0; i < 5; i++) {
              pixels[i] = pixels[i + 4];
            }
            pixels[5] = pixels[3];
          }
          else
          {
            #pragma hls_unroll yes
            for (int i = 0; i < 5; i++) {
              pixels[i] = pixels[i + 4];
            }
            #pragma hls_unroll yes
            for (int i = 0; i < 4; i++) {
              pixels[i + 5] = pix4.slc<8>(i * 8); 
            }
          }

          // Calculate derivative
          #pragma hls_unroll yes
          for(int i = 0; i < 4; i++)
          {
            gradType pix_tmp;
            pix_tmp = pixels[i] * kernel_2[0] + pixels[i + 1] * kernel_2[1] + pixels[i + 2] * kernel_2[2];
            grad.set_slc(i * 9, pix_tmp); 
          }
          // compress 4 pixels
          // Write streaming interface
          if (x != 0) { 
            pixelType4x pix_tmp; 
            pix_tmp.set_slc( 0, pixels[1]);
            pix_tmp.set_slc( 8, pixels[2]);
            pix_tmp.set_slc(16, pixels[3]);
            pix_tmp.set_slc(24, pixels[4]);
            pix_out.write(pix_tmp);
            dx.write(grad); // derivative out
          }
          // programmable width exit condition
          if ( A == widthIn / 4) {
            break;
          }
        }
        // programmable height exit condition
        if (y == maxHType(heightIn - 1)) { 
          break;
        }
      }
    }
  };


