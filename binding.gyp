{
  "targets": [
    {
      "target_name": "addon",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions", "-fno-rtti" ],
      "sources": [ "tkdnn.cc", "addon.cc" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/usr/local/cuda/include",
        "/usr/local/include/tkDNN",
        "/usr/local/include/eigen3",
        "/usr/local/include/opencv4",
        "/usr/include/opencv4",
      ],
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      "ldflags": [ ], 
      "link_settings": {
        "libraries": [
          "-lcudart",
          "-ltkDNN",
          "-lkernels",
          "-lpthread",
          "-ldl",
          "-lyaml-cpp",
          "-lopencv_core",
          "-lopencv_highgui",
          "-lopencv_imgproc",
          "-lopencv_videoio",
        ],
        "library_dirs": [
          "/usr/local/cuda/lib64",
        ]
      }      
    }
  ]
}
