//
// Created by Matan Sokolovsky on 4/11/17.
//

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "SPConfig.h"
#include "KDTree.h"

#define SP_CONFIG_OPEN_MODE "r"

#define BUFFER_LEN 1024


typedef struct sp_config_t {
    bool spExtractionMode;
    bool spMinimalGUI;
    int spNumOfImages;
    int spNumOfFeatures;
    int spPCADimension;
    int spNumOfSimilarImages;
    int spKNN;
    SP_LOGGER_LEVEL spLoggerLevel;
    char *spImagesDirectory;
    char *spImagesPrefix;
    char *spImagesSuffix;
    char *spPcaFilename;
    char *spLoggerFilename;
    SplitMethod spKDTreeSplitMethod;
};

typedef enum sp_config_line_t {
    CONFIG_LINE_SUCCESS,
    CONFIG_LINE_INVALID,
    CONFIG_LINE_EMPTY
} SP_CONFIG_LINE_MSG;

typedef enum special_bool {
    FALSE = 0,
    TRUE = 1,
    OTHER = 2
} SpecialBool;

SP_CONFIG_MSG setDefaults(SPConfig config) {
    config->spExtractionMode = true;
    config->spMinimalGUI = false;
    config->spNumOfImages = -1;
    config->spNumOfFeatures = 100;
    config->spPCADimension = 20;
    config->spNumOfSimilarImages = 1;
    config->spKNN = 1;
    config->spLoggerLevel = SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
    config->spImagesDirectory = malloc(BUFFER_LEN);
    config->spImagesPrefix = malloc(BUFFER_LEN);
    config->spImagesSuffix = malloc(BUFFER_LEN);
    config->spPcaFilename = malloc(BUFFER_LEN);
    config->spLoggerFilename = malloc(BUFFER_LEN);

    strcat(config->spPcaFilename, "pca.yml");
    strcat(config->spLoggerFilename, "stdout");

    config->spKDTreeSplitMethod = MAX_SPREAD;
}

SP_CONFIG_LINE_MSG spGetParamsFromLine(const char *line, char *variableName, char *value) {
    char *c = (char *) line;

    // Skip all whitespaces
    while (isspace(*c)) c++;

    // Empty or comment line
    if (*c == '#' || *c == '\0') return CONFIG_LINE_EMPTY;

    char *name = variableName;

    // Iterate over variable name chars
    while (*c != '=' && !isspace(*c)) {
        *name = *c;
        name++;
        c++;
        if (*c == '\0') return CONFIG_LINE_INVALID;
    }

    // Find equals sign
    while (*c != '=') {
        c++;
        if (*c == '\0') return CONFIG_LINE_INVALID;
    }

    // Skip whitespaces
    while (isspace(*c)) c++;

    if (*c == '\0') return CONFIG_LINE_INVALID;

    char *val = value;

    // Get variable value
    while (!isspace(*c) && *c != '\0') {
        *val = *c;
        val++;
        c++;
    }

    // Skip whitespaces
    while (isspace(*c)) c++;

    // String should end
    if (*c != '\0') return CONFIG_LINE_INVALID;

    return CONFIG_LINE_SUCCESS;
}

SpecialBool getBool(const char *value) {
    if (strcmp(value, "true")) {
        return TRUE;
    }
    if (strcmp(value, "false")) {
        return FALSE;
    }
    return OTHER;
}

SP_CONFIG_LINE_MSG spSetVariable(const char *name, const char *value, SPConfig config) {
    if (strcmp(name, "spExtractionMode")) {
        SpecialBool val = getBool(value);
        if (val == OTHER) return CONFIG_LINE_INVALID;
        config->spExtractionMode = (bool) val;
    }
}


SPConfig spConfigCreate(const char *filename, SP_CONFIG_MSG *msg) {
    if (filename == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return NULL;
    }

    FILE *file = fopen(filename, SP_CONFIG_OPEN_MODE);
    if (file == NULL) {
        *msg = SP_CONFIG_CANNOT_OPEN_FILE;
        return NULL;
    }

    SPConfig config = malloc(sizeof(SPConfig));
    if (config == NULL) {
        *msg = SP_CONFIG_ALLOC_FAIL;
        return NULL;
    }
    setDefaults(config);

    char line[BUFFER_LEN];
    while (fgets(line, BUFFER_LEN, file) != NULL) {
        char name[BUFFER_LEN], value[BUFFER_LEN];
        SP_CONFIG_LINE_MSG lineMsg = spGetParamsFromLine(line, name, value);

        if (lineMsg == CONFIG_LINE_INVALID) {
            spConfigDestroy(config);
            *msg = SP_CONFIG_INVALID_STRING;
            return NULL;
        };

        if (lineMsg == CONFIG_LINE_EMPTY) continue;


    }
}

/**
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG *msg) {
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spExtractionMode;
}

/**
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG *msg) {
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    return config->spMinimalGUI;
}

/**
 * Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG *msg) {
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfImages;
}

/**
 * Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG *msg) {
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfFeatures;
}

/**
 * Returns the dimension of the PCA. i.e the value of spPCADimension.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG *msg) {
    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }
    *msg = SP_CONFIG_SUCCESS;
    return config->spPCADimension;
}

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.png" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImagePath(char *imagePath, const SPConfig config,
                                   int index) {
    if (imagePath == NULL || config == NULL) {
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    if (index >= config->spNumOfImages) {
        return SP_CONFIG_INDEX_OUT_OF_RANGE;
    }

    sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory, config->spImagesPrefix, index, config->spImagesSuffix);
    return SP_CONFIG_SUCCESS;
}

/**
 * The function stores in pcaPath the full path of the pca file.
 * For example given the values of:
 *  spImagesDirectory = "./images/"
 *  spPcaFilename = "pca.yml"
 *
 * The functions stores "./images/pca.yml" to the address given by pcaPath.
 * Thus the address given by pcaPath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetPCAPath(char *pcaPath, const SPConfig config) {
    if (pcaPath == NULL || config == NULL) {
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    sprintf(pcaPath, "%s%s", config->spImagesDirectory, config->spPcaFilename);
    return SP_CONFIG_SUCCESS;
}

/**
 * Frees all memory resources associate with config.
 * If config == NULL nothing is done.
 */
void spConfigDestroy(SPConfig config) {
    if (config == NULL) return;

    free(config->spImagesDirectory);
    free(config->spImagesPrefix);
    free(config->spImagesSuffix);
    free(config->spLoggerFilename);
    free(config->spPcaFilename);

    free(config);
}