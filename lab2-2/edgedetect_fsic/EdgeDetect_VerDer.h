#pragma once

#include "EdgeDetect_defs.h"
#include <mc_scverify.h>

  class EdgeDetect_VerDer
  {
  private:
  
  public:
    EdgeDetect_VerDer() {}
  
    #pragma hls_design interface
    void CCS_BLOCK(run)(ac_channel<Stream_t>    &dat_in,
                        maxWType                &widthIn,
                        maxHType                &heightIn,
                        ac_channel<pixelType4x> &pix_out,
                        ac_channel<gradType4x>  &dy)
    {
      // Line buffers store pixel line history - Mapped to RAM
      pixelType8x line_buf0[maxImageWidth / 8];
      pixelType8x line_buf1[maxImageWidth / 8];
      pixelType8x rdbuf0_pix, rdbuf1_pix;
      pixelType8x wrbuf0_pix, wrbuf1_pix;
      pixelType4x pix0, pix1, pix2;
      gradType4x pix4;
      Stream_t dat;
      maxWType A;
  
      // Remove loop upperbounds for RTL code coverage
      // Use bit accurate data types on loop iterator
      VROW: for (maxHType y = 0; ; y++) { // VROW has one extra iteration to ramp-up window
        #pragma hls_pipeline_init_interval 1
        VCOL: for (maxWType x = 0; ; x += 4) {
          A = x / 4;
          if (y <= heightIn - 1) {
            dat = dat_in.read(); // Read streaming interface
            pix0 = dat.pix;
          }
          // Write data cache, write lower 8 on even iterations of COL loop, upper 8 on odd
          if ( (A & 1) == 0 ) {
            wrbuf0_pix.set_slc( 0, pix0);
          } else {
            wrbuf0_pix.set_slc(32, pix0);
          }
          // Read line buffers into read buffer caches on even iterations of COL loop
          if ( (A & 1) == 0 ) {
            // vertical window of pixels
            rdbuf1_pix = line_buf1[A / 2];
            rdbuf0_pix = line_buf0[A / 2];
          } else { // Write line buffer caches on odd iterations of COL loop
            line_buf1[A / 2] = rdbuf0_pix; // copy previous line
            line_buf0[A / 2] = wrbuf0_pix; // store current line
          }
          // Get 8-bit data from read buffer caches, lower 8 on even iterations of COL loop
          pix2 = ((A & 1) == 0) ? rdbuf1_pix.slc<32>(0) : rdbuf1_pix.slc<32>(32);
          pix1 = ((A & 1) == 0) ? rdbuf0_pix.slc<32>(0) : rdbuf0_pix.slc<32>(32);
  
          // Boundary condition processing
          if (y == 1) {
            pix2 = pix1; // top boundary (replicate pix1 up to pix2)
          }
          if (y == heightIn) {
            pix0 = pix1; // bottom boundary (replicate pix1 down to pix0)
          }
  
          // Calculate derivative
          #pragma hls_unroll yes
          for(int i = 0; i < 4; i++)
          {
            gradType pix_tmp;
            pix_tmp = pix2.slc<8>(i * 8) * kernel_2[0] + pix1.slc<8>(i * 8) * kernel_2[1] + pix0.slc<8>(i * 8) * kernel_2[2];
            pix4.set_slc(i * 9, pix_tmp);
          }

          if (y != 0) { // Write streaming interfaces
            pix_out.write(pix1); // Pass thru original data
            dy.write(pix4); // derivative output
          }
          // programmable width exit condition
          if (A == maxWType(widthIn / 4 - 1)) { // cast to maxWType for RTL code coverage
            break;
          }
        }
        // programmable height exit condition
        if (y == heightIn) {
          break;
        }
      }
    }
  };
//}


