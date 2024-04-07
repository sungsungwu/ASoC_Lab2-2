#pragma once

#include "EdgeDetect_defs.h"
#include <mc_scverify.h>

  class EdgeDetect_MagAng
  {
  public:
    EdgeDetect_MagAng() {}
  
    #pragma hls_design interface
    void CCS_BLOCK(run)(ac_channel<gradType4x>  &dx_in,
                        ac_channel<gradType4x>  &dy_in,
                        ac_channel<pixelType4x> &pix_in,
                        maxWType                &widthIn,
                        maxHType                &heightIn,
                        bool                    &sw_in,
                        uint32                  &crc32_pix_in,
                        uint32                  &crc32_dat_out,
                        ac_channel<Stream_t>    &dat_out)
                        
    {
        gradType4x dx, dy;
        pixelType dx_abs, dy_abs;
        maxWType A;  //for 4 pixel 
        //sqType dx_sq;
        //sqType dy_sq;
        sumType sum; // fixed point integer for sqrt
        //angType at;
        //ac_fixed<16,9,false> sq_rt; // square-root return type
        pixelType4x pix4;
        magType4x magn;
        magType4x magn_out;
        Stream_t dat;
        uint32 crc32_pix_in_tmp  = 0XFFFFFFFF;
        uint32 crc32_dat_out_tmp = 0XFFFFFFFF;
  
            MROW: for (maxHType y = 0; ; y++) {
              #pragma hls_pipeline_init_interval 1
              MCOL: for (maxWType x = 0; ; x += 4) {
                A   = x / 4; // x should be divided by 4
                dx   = dx_in.read();
                dy   = dy_in.read();
                pix4 = pix_in.read();
                //sum = dx_sq + dy_sq;
                for(int i = 0; i < 4; i++){
                  ac_math::ac_abs(dx.slc<9>(i * 9), dx_abs); // get the absolute value of dx
                  ac_math::ac_abs(dy.slc<9>(i * 9), dy_abs);
                  //diff = dx - dy;  //abs diff?
                  //ac_math::ac_abs(diff,SAD);  //abs
                  sum = dx_abs + dy_abs;
                  ac_fixed<8, 8, false, AC_TRN,AC_SAT> abs_sum_clip = sum; // Fixed type
                  magType tmp = (magType) abs_sum_clip.to_uint(); // Convert to unsigned int
                  magn.set_slc(i * 8, tmp);
                  }
                //ac_fixed<8, 8, false, AC_TRN,AC_SAT> abs_sum_clip = abs_sum; // Fixed type
                //magType tmp = (magType) abs_sum_clip.to_uint(); // Convert to unsigned int
                //magn.set_slc(i * 8, tmp);
                //dat_out.write(SAD);
                //mux
                
       //         if (sw_in==0)
                  //*(dat_out + y * imageWidth + x) = pix_chan2[y * imageWidth + x];
         //         magn_out = pix4;   //from input image
         //       else
                  //*(dat_out + y * imageWidth + x) = SAD(sum);
         //         magn_out = magn;    //from calc mag
                  if (!sw_in)                                                         
                    magn_out = pix4;                                                  
                  else                                                                
                    magn_out = magn;
                  
                  dat.pix = magn_out;
                  dat.sof = (A == 0 && y == 0);
                  dat.eol = (A == maxWType(widthIn / 4 - 1));
        
                  dat_out.write(dat);
                  
                  //crc32 for pix_in
 //               uint32 crc32_pix_in_tmp = 0XFFFFFFFF;
                //crc32 algorithm//
                //for (maxHType y = 0; ; y++) {
                  //for (maxHType x = 0; ; x+=4)
                  //{/*
                                         
                   // pixelType4x  pix4 = (pix_chan2[y * widthIn + x + 0] <<  0) |
                                        // (pix_chan2[y * widthIn + x + 1] <<  8) |
                                        // (pix_chan2[y * widthIn + x + 2] << 16) |
                                        // (pix_chan2[y * widthIn + x + 3] << 24) ;
            
            
                crc32_pix_in_tmp = calc_crc32<32>(crc32_pix_in_tmp, pix4);     //crc32 for pix in
                  //}
            
                crc32_pix_in = ~crc32_pix_in_tmp; 
               // }
                
              
                //crc32 for magn(datout)
 //               uint32 crc32_dat_out_tmp = 0XFFFFFFFF;
                //for (maxHType y = 0; ; y++) {
                 // for (maxHType x = 0; ; x+=4)
                 // {
                                          
                    // magType4x magn_out = (magn_out[y * widthIn + x + 0] <<  0) |
                             //             (magn_out[y * widthIn + x + 1] <<  8) |
                             //             (magn_out[y * widthIn + x + 2] << 16) |
                             //             (magn_out[y * widthIn + x + 3] << 24) ;
                     
                crc32_dat_out_tmp = calc_crc32<32>(crc32_dat_out_tmp, magn_out);   //crc32 for out
                  //}
            
                crc32_dat_out = ~crc32_dat_out_tmp;

          // programmable width exit condition
          if (A == maxWType(widthIn / 4 - 1)) { // cast to maxWType for RTL code coverage
            break;
          }
        }
        // programmable height exit condition
        if (y == maxHType(heightIn - 1)) { // cast to maxHType for RTL code coverage
          break;
        }
      }
 
      crc32_pix_in  = ~crc32_pix_in_tmp;
      crc32_dat_out = ~crc32_dat_out_tmp;
    }

    //crc32
    private:
      template <int len>
      uint32 calc_crc32(uint32 crc_in, ac_int<len, false> dat_in)
      {
        const uint32 CRC_POLY = 0xEDB88320;
        uint32 crc_tmp = crc_in;
  
        #pragma hls_unroll yes
        for(int i = 0; i < len; i++)
        {
          uint1 tmp_bit = crc_tmp[0] ^ dat_in[i];
  
          uint31 mask;
  
          #pragma hls_unroll yes
          for(int i = 0; i < 31; i++)
            mask[i] = tmp_bit & CRC_POLY[i];
  
          uint31 crc_tmp_h31 = crc_tmp.slc<31>(1);
     
          crc_tmp_h31 ^= mask;
          
          crc_tmp.set_slc(31, tmp_bit);
          crc_tmp.set_slc(0, crc_tmp_h31);
        }
        return crc_tmp;
      }
      
    };


