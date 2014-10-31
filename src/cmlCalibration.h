#pragma once

#include "cmlConfig.h"
#include "cmlOpticalDevice.h"
#include "cv.h"

namespace cml
{
  class Calibration
  {
    public:

      Calibration( 
        cml::Config config,
        OpticalDevice::Config& proj_cfg,
        OpticalDevice::Config& depth_cfg,
        OpticalDevice::Config& rgb_cfg
        );
      ~Calibration();

    private:

      void device_config( 
          string calib_path, 
          string int_id,
          string size_id,
          string R_id, string T_id, 
          OpticalDevice::Config& dev_config );

      void printM( CvMat* M, 
          bool colmajor = true );
  };
};

