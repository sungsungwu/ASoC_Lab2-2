#pragma once

#include "EdgeDetect_defs.h"
#include "EdgeDetect_VerDer.h"
#include "EdgeDetect_HorDer.h"
#include "EdgeDetect_MagAng.h"
#include <mc_scverify.h>

  #pragma hls_design top
  class EdgeDetect_Top
  {
    //instances
    EdgeDetect_VerDer VerDer_inst;
    EdgeDetect_HorDer HorDer_inst;
    EdgeDetect_MagAng MagAng_inst;
  
    // Static interconnect channels (FIFOs) between blocks
    ac_channel<gradType4x>       dy_chan;
    ac_channel<gradType4x>       dx_chan;
    ac_channel<pixelType4x>      pix_chan1; 
    ac_channel<pixelType4x>      pix_chan2; 
  
  public:
    EdgeDetect_Top() {}
  
    //--------------------------------------------------------------------------
    // Function: run
    //   Top interface for data in/out of class. Combines vertical and
    //   horizontal derivative and magnitude/angle computation.
    #pragma hls_design interface
    //#pragma busifc_cfg slv0 DataWidth=32 BaseAddress=0x7000000 Protocol=axi4lite
    void CCS_BLOCK(run)(maxWType              widthIn,
                        maxHType              heightIn,
                        bool                  sw_in,
                        uint32                &crc32_pix_in,
                        uint32                &crc32_dat_out,
                        ac_channel<Stream_t>  &dat_in,
                        ac_channel<Stream_t>  &dat_out)
    {
      //#pragma busifc widthIn        WordOffset=0 Slave=slv0
      //#pragma busifc heightIn       WordOffset=1 Slave=slv0
      //#pragma busifc sw_in          WordOffset=2 Slave=slv0
      //#pragma busifc crc32_pix_in   WordOffset=3 Slave=slv0
      //#pragma busifc crc32_dat_out  WordOffset=4 Slave=slv0
      VerDer_inst.run(dat_in, widthIn, heightIn, pix_chan1, dy_chan);
      HorDer_inst.run(pix_chan1, widthIn, heightIn, pix_chan2, dx_chan);
      MagAng_inst.run(dx_chan, dy_chan, pix_chan2, widthIn, heightIn, sw_in, crc32_pix_in, crc32_dat_out, dat_out);
    }
  };



