#include <stdlib.h>
#include <stdio.h>

#include "main_aux.h"

extern "C" {
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPPoint.h"
#include "KDArray.h"
#include "KDTree.h"
}

int main() {
    SP_CONFIG_MSG msg;
    SP_LOGGER_MSG logger_msg;

    SPConfig config = spConfigCreate("./spcbir.config", &msg);
    if (msg != SP_CONFIG_SUCCESS) return 1;

    logger_msg = spLoggerCreate(NULL, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL);
    if(logger_msg != SP_LOGGER_SUCCESS) return 1;

    extractKDTree(config);
}