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


int main(int argc, char **argv) {
    SP_CONFIG_MSG msg;
    SP_LOGGER_MSG logger_msg;
    SPConfig config;
    char queryPath[MAX_BUFFER_SIZE], loggerFilename[MAX_BUFFER_SIZE];
    KDTree *kdTree;
    SP_LOGGER_LEVEL logger_level;
    if (argc == 3) {
        if (strcmp(argv[1], "-c") != 0)
            printf("Invalid command line : use -c %s", argv[2]);
        config = spConfigCreate(argv[2], &msg);
        if (msg != SP_CONFIG_SUCCESS) {
            printf("The configuration file %s couldnt be open\n", argv[2]);
            return 1;
        }
    } else if (argc == 1) {
        config = spConfigCreate("./spcbir.config", &msg);
        if (msg != SP_CONFIG_SUCCESS) {
            printf("The default configuration file spcbir.config couldnt be open\n");
            return 1;
        }
    } else {
        printf("Invalid command line : use -c <config_filename>");
        return 1;
    }

    spConfigGetLoggerFilename(config, loggerFilename);
    logger_level = spConfigGetLoggerLevel(config);
    logger_msg = spLoggerCreate(loggerFilename, logger_level);
    if (logger_msg != SP_LOGGER_SUCCESS) {
        spConfigDestroy(config);
        return 1;
    }
    kdTree = extractKDTree(config);
    // Get query from user
    printf("Please enter an image path:\n");
    scanf("%s", queryPath);
    while (strcmp(queryPath, "<>") != 0) {
        if (!searchSimilarImages(config, queryPath, kdTree)) {
            destroyKDTree(kdTree);
            spConfigDestroy(config);
            spLoggerDestroy();
            return 1;
        }
        printf("Please enter an image path:\n");
        scanf("%s", queryPath);
    }
    printf("Exiting...\n");
    destroyKDTree(kdTree);
    spConfigDestroy(config);
    spLoggerDestroy();
    return 0;
}