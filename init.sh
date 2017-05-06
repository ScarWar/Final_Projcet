#!/bin/sh

echo "Initializing system variables"
if (! $LD_LIBRARY_PATH) then       
  export LD_LIBRARY_PATH="/usr/local/lib/opencv-3.1.0/lib/"
else
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/lib/opencv-3.1.0/lib/"
fi
echo "Done!"
