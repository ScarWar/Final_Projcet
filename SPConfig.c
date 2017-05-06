#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "SPConfig.h"

#define SP_CONFIG_OPEN_MODE "r"

#define BUFFER_LEN 1024

#define INVALID_LINE_MSG "Invalid configuration line"
#define INVALID_VALUE_MSG "Invalid value - constraint not met"

struct sp_config_t {
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

void setDefaults(SPConfig config) {
    config->spExtractionMode = true;
    config->spMinimalGUI = false;
    config->spNumOfImages = -1;
    config->spNumOfFeatures = 100;
    config->spPCADimension = 20;
    config->spNumOfSimilarImages = 1;
    config->spKNN = 1;
    config->spLoggerLevel = SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
    config->spImagesDirectory = malloc(BUFFER_LEN);
    if (!config->spImagesDirectory) {
        // TODO error
        printf("spImagesDirectory\n");
        return;
    }
    config->spImagesPrefix = malloc(BUFFER_LEN);
    if (!config->spImagesPrefix) {
        // TODO error
        printf("spImagesPrefix\n");
        return;
    }    
    config->spImagesSuffix = malloc(BUFFER_LEN);
    if (!config->spImagesSuffix) {
        // TODO error
        printf("spImagesSuffix\n");
        return;
    }    
    config->spPcaFilename = malloc(BUFFER_LEN);
    if (!config->spPcaFilename) {
        // TODO error
        printf("spPcaFilename\n");
        return;
    }
    config->spLoggerFilename = malloc(BUFFER_LEN);
    if (!config->spLoggerFilename) {
        // TODO error
        printf("spLoggerFilename\n");
        return;
    }
    
    *config->spImagesDirectory = '\0';
    *config->spImagesPrefix = '\0';
    *config->spImagesSuffix = '\0';

    strcat(config->spPcaFilename, "pca.yml");
    strcat(config->spLoggerFilename, "stdout");

    config->spKDTreeSplitMethod = MAX_SPREAD;
}

void printError(const char *filename, int line, char *message) {
    printf("File: %s\nLine: %d\nMessage: %s\n", filename, line, message);
}

SP_CONFIG_LINE_MSG spGetParamsFromLine(const char *line, char *variableName, char *value) {
    char *c = (char *) line;

    // Skip all whitespaces
    while (isspace(*c)) c++;

    // Empty or comment line
    if (*c == '#' || *c == '\0' || *c == '\n') return CONFIG_LINE_EMPTY;

    char *name = variableName;

    // Iterate over variable name chars
    while (*c != '=' && !isspace(*c)) {
        *name = *c;
        name++;
        c++;
        if (*c == '\0') return CONFIG_LINE_INVALID;
    }
    *name = '\0';

    // Find equals sign
    while (*c != '=') {
        c++;
        if (*c == '\0') return CONFIG_LINE_INVALID;
    }
    c++;

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
    *val = '\0';

    // Skip whitespaces
    while (isspace(*c)) c++;

    // String should end
    if (*c != '\0') return CONFIG_LINE_INVALID;

    return CONFIG_LINE_SUCCESS;
}

bool getBool(const char *value, SP_CONFIG_LINE_MSG *msg) {
    *msg = CONFIG_LINE_SUCCESS;
    if(!strcmp(value, "true")) return true;
    if(strcmp(value, "false")) *msg = CONFIG_LINE_INVALID;
    return false;
}

SP_CONFIG_LINE_MSG spSetVariable(const char *name, const char *value, SPConfig config) {
    SP_CONFIG_LINE_MSG msg;

    if (!strcmp(name, "spExtractionMode")) {
        config->spExtractionMode = getBool(value, &msg);
        return msg;
    }

    if (!strcmp(name, "spMinimalGUI")) {
        config->spExtractionMode = getBool(value, &msg);
        return msg;
    }

    if (!strcmp(name, "spNumOfImages")) {
        int num = atoi(value);
        if (num < 1) {
            return CONFIG_LINE_INVALID;
        }
        config->spNumOfImages = num;
        return CONFIG_LINE_SUCCESS;
    }

    if (!strcmp(name, "spNumOfFeatures")) {
        int num = atoi(value);
        if (num < 1) {
            return CONFIG_LINE_INVALID;
        }
        config->spNumOfFeatures = num;
        return CONFIG_LINE_SUCCESS;
    }

    if (!strcmp(name, "spPCADimension")) {
        int dim = atoi(value);
        if (dim < 10 || dim > 28) {
            return CONFIG_LINE_INVALID;
        }
        config->spPCADimension = dim;
        return CONFIG_LINE_SUCCESS;
    }

    if (!strcmp(name, "spNumOfSimilarImages")) {
        int num = atoi(value);
        if (num < 1) {
            return CONFIG_LINE_INVALID;
        }
        config->spNumOfSimilarImages = num;
        return CONFIG_LINE_SUCCESS;
    }

    if (!strcmp(name, "spKNN")) {
        int knn = atoi(value);
        if (knn < 1) {
            return CONFIG_LINE_INVALID;
        }
        config->spKNN = knn;
        return CONFIG_LINE_SUCCESS;
    }

    if (!strcmp(name, "spLoggerLevel")) {
        int level = atoi(value);
        if (level < 1 || level > 4)
        {
            return CONFIG_LINE_INVALID;
        }
        config->spLoggerLevel = (SP_LOGGER_LEVEL) level;
        return CONFIG_LINE_SUCCESS;
    }

    if (!strcmp(name, "spImagesDirectory")) {
        strcpy(config->spImagesDirectory, value);
        return CONFIG_LINE_SUCCESS;
    }
    if (!strcmp(name, "spImagesPrefix")) {
        strcpy(config->spImagesPrefix, value);
        return CONFIG_LINE_SUCCESS;
    }
    if (!strcmp(name, "spImagesSuffix")) {
        strcpy(config->spImagesSuffix, value);
        return CONFIG_LINE_SUCCESS;
    }
    if (!strcmp(name, "spPcaFilename")) {
        strcpy(config->spPcaFilename, value);
        return CONFIG_LINE_SUCCESS;
    }
    if (!strcmp(name, "spLoggerFilename")) {
        strcpy(config->spLoggerFilename, value);
        return CONFIG_LINE_SUCCESS;
    }
    if (!strcmp(name, "spKDTreeSplitMethod")) {
        SplitMethod method;
        if (!strcmp(value, "RANDOM")) method = RANDOM;
        else if (!strcmp(value, "INCREMENTAL")) method = INCREMENTAL;
        else if (!strcmp(value, "MAX_SPREAD")) method = MAX_SPREAD;
        else return CONFIG_LINE_INVALID;

        config->spKDTreeSplitMethod = method;
        return CONFIG_LINE_SUCCESS;
    }

    return CONFIG_LINE_INVALID;
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

    SPConfig config = malloc(sizeof(struct sp_config_t));
    if (config == NULL) {
        *msg = SP_CONFIG_ALLOC_FAIL;
        return NULL;
    }
    setDefaults(config);

    int line_num = 0;

    char line[BUFFER_LEN];
    while (fgets(line, BUFFER_LEN, file) != NULL) {
        line_num++;
        char name[BUFFER_LEN], value[BUFFER_LEN];
        SP_CONFIG_LINE_MSG lineMsg = spGetParamsFromLine(line, name, value);

        if (lineMsg == CONFIG_LINE_INVALID) {
            printError(filename, line_num, INVALID_LINE_MSG);
            spConfigDestroy(config);
            *msg = SP_CONFIG_INVALID_STRING;
            return NULL;
        };

        if (lineMsg == CONFIG_LINE_EMPTY) continue;

        SP_CONFIG_LINE_MSG line_msg = spSetVariable(name, value, config);
        if (line_msg != CONFIG_LINE_SUCCESS) {
            printError(filename, line_num, INVALID_VALUE_MSG);
            spConfigDestroy(config);
            *msg = SP_CONFIG_INVALID_ARGUMENT;
            return NULL;
        }

    }

    if (config->spNumOfImages == -1) {
        printError(filename, line_num, "Parameter spNumOfImages is not set");
    }

    if (*config->spImagesDirectory == '\0') {
        printError(filename, line_num, "Parameter spImagesDirectory is not set");
    }

    if (*config->spImagesSuffix == '\0') {
        printError(filename, line_num, "Parameter spImagesSuffix is not set");
    }

    if (*config->spImagesPrefix == '\0') {
        printError(filename, line_num, "Parameter spImagesPrefix is not set");
    }

    *msg = SP_CONFIG_SUCCESS;
    return config;
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
    if (config == NULL || pcaPath == NULL) {
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

SplitMethod spConfigGetKDTreeSplitMethod(const SPConfig config) {
    return config->spKDTreeSplitMethod;
}

int spConfigGetKNN(const SPConfig config) {
    return config->spKNN;
}

int spConfigGetNumOfSimilarImages(const SPConfig config) {
    return config->spNumOfSimilarImages;
}

void spConfigGetLoggerFilename(const SPConfig config, char *loggerFilename) {
    strcpy(loggerFilename, config->spLoggerFilename);
}

SP_LOGGER_LEVEL spConfigGetLoggerLevel(const SPConfig config) {
    return config->spLoggerLevel;
}

SP_CONFIG_MSG spConfigGetImageFeaturesPath(char *imagePath, const SPConfig config,
                                           int index) {
    if (imagePath == NULL || config == NULL) {
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    if (index >= config->spNumOfImages) {
        return SP_CONFIG_INDEX_OUT_OF_RANGE;
    }

    sprintf(imagePath, "%s%s%d", config->spImagesDirectory, config->spImagesPrefix, index);
    return SP_CONFIG_SUCCESS;
}
