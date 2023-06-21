//
// Created by shadad on 03/06/23.
//
#include "ShadowGenerator.h"




static  shadow ** initializeShadowArray(shadowGenerator * shadowGenerator, uint64_t shadowPoints);
static uint8_t evaluatePolynomial(shadowGenerator * shadowGenerator, uint8_t * coefficients, uint8_t value);
static void insertBits(uint8_t  *imagePixelPointer, uint8_t  *shadowPointer, uint8_t k);
static void hideShadow(uint8_t  k , bmpFile * image, shadow * hidingShadow);


shadowGenerator * initialize(struct params * params){
    struct shadowGenerator * generator = malloc(sizeof(shadowGenerator));
    generator->file = openBmpFile(params->file);
    generator->k = params->k;
    generator->n = params -> n;
    openDirectory(generator, params->directory);
    return generator;
}


void distributeSecret(shadowGenerator * shadowGenerator){
    uint64_t shadowPoints = (shadowGenerator->file->header->imageSize) / (shadowGenerator->k - 1);
    struct shadow ** shadowArray  = initializeShadowArray(shadowGenerator, shadowPoints);

    uint8_t * pixelPoints = shadowGenerator->file->pixels;
    uint8_t k = shadowGenerator->k;
    uint8_t * aCoefficients = malloc( k);
    uint8_t * bCoefficients= malloc( k );
    uint8_t a_0, a_1;

    for( uint64_t currentBlock = 0; currentBlock < shadowPoints;  currentBlock +=2){
        memcpy(aCoefficients, pixelPoints, k);
        memcpy(bCoefficients + 2, pixelPoints + k, k-2 );

        uint8_t random = (rand() % (P - 2)) + 1;
        a_0 = mod(aCoefficients[0]) == 0 ? 1: aCoefficients[0];
        a_1 = mod(aCoefficients[1]) == 0 ? 1: aCoefficients[1];
        bCoefficients[0] =  mod(mul(mod(- random) , a_0) );
        bCoefficients[1] =  mod(mul(mod(- random ), a_1));

        for (int j = 0; j < shadowGenerator->n; j++){
            shadowArray[j]->points[currentBlock] = evaluatePolynomial(shadowGenerator,
                                                                      aCoefficients,shadowArray[j]->shadowNumber);
            shadowArray[j]->points[currentBlock + 1] = evaluatePolynomial(shadowGenerator,
                                                                          bCoefficients, shadowArray[j]->shadowNumber);
        }

        pixelPoints += (2 * k) - 2;
    }

    shadowGenerator->generatedShadows =  shadowArray;
}

void hideSecret(shadowGenerator * shadowGenerator){
    for (int i = 0 ; i < shadowGenerator -> n ; i ++){
        bmpFile  * currentImageFile = openBmpFile(shadowGenerator->imageFiles[i]);
        shadow * currentShadow = shadowGenerator->generatedShadows[i];
        hideShadow(shadowGenerator->k,currentImageFile, currentShadow);

        //save the generated image.
        int headerSize = currentImageFile->header->fileSize - currentImageFile->header->imageSize;
        //re-write the entire file.
         lseek(currentImageFile->fd, 0, SEEK_SET);
        write(currentImageFile->fd , currentImageFile->header, headerSize);
        write(currentImageFile->fd , currentImageFile->pixels, currentImageFile->header->imageSize);
        close(currentImageFile->fd);
    }

}


static void hideShadow(uint8_t  k , bmpFile * image, shadow * hidingShadow){
    image->header->reserved1 = hidingShadow->shadowNumber; //save the shadow number.
    uint8_t * imagePixelPointer = image->pixels;
    uint8_t * shadowPointer = hidingShadow->points;
    for(uint32_t  i = 0; i < hidingShadow->pointNumber; i++ ){
            insertBits(imagePixelPointer, shadowPointer, k);
            shadowPointer += 1 ; //go to the next point;
            imagePixelPointer += (k == 3 || k == 4) ? 2 : 4;
    }
}

void openDirectory(shadowGenerator * generator, char * directoryPath){
    DIR * directory = opendir(directoryPath);

    if(directory == NULL){
        perror("Unable to open the given directory.\n");
        exit(EXIT_FAILURE);
    }
    char ** fileNames = malloc(generator->n * sizeof(char * )) ;

    int currentFile = 0;
    struct dirent * entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        uint64_t directoryLength = strlen(directoryPath);
        fileNames[currentFile] = malloc(directoryLength + 1 + strlen(entry->d_name) + 1);
        strncpy(fileNames[currentFile], directoryPath, directoryLength);
        strncpy(fileNames[currentFile] + directoryLength, "/", 2);
        strcpy(fileNames[currentFile] + directoryLength + 1, entry->d_name);
        currentFile ++;
    }

    if (currentFile  < generator->k){
        printf("Incorrect number of shadows provided.\n");
        exit(EXIT_FAILURE);
    }

    generator->imageFiles = fileNames;
}

static shadow ** initializeShadowArray(shadowGenerator * shadowGenerator, uint64_t shadowPoints){

     shadow ** shadowArray = malloc(shadowGenerator->n * sizeof(shadow *));
    //initialize all shadow structures.
    for (int i = 0; i < shadowGenerator -> n ; i++){
        shadowArray[i] = malloc(sizeof(struct shadow));
        shadowArray[i]->shadowNumber = i + 1;
        shadowArray[i]->pointNumber = shadowPoints;
        shadowArray[i]->points = malloc(shadowPoints);
    }
    return  shadowArray;
}

static uint8_t evaluatePolynomial(struct shadowGenerator * shadowGenerator, uint8_t * coefficients, uint8_t value){
    uint8_t result = 0;
    uint8_t power = 1;

    uint8_t x2 = mod(value);

    for (uint8_t i = 0; i < shadowGenerator->k ; i++) {
        result = sum( result , mul(coefficients[i] , power));
        power = mul(power ,x2);
    }

    return result;
}


uint8_t fourSignificant[] = {0xC0, 0x30, 0x0C, 0x03};
uint8_t twoSignificant[] = {0xF0, 0x0F};

static void insertBits(uint8_t  * imagePixelPointer, uint8_t  *shadowPointer, uint8_t  k){
    int lsb4 = ( k == 3 || k == 4 ) ? 1 : 0;
    int bytesUsedFromImage = ( lsb4 ) ? 2 : 4;

    uint8_t  lsb4Shifter[] = {4, 0};
    uint8_t  lsb2Shifter[] = {6,4,2, 0};
    int currentShifterIndex = 0 ;

    uint8_t  bits[bytesUsedFromImage];

    for(int i = 0; i < bytesUsedFromImage ; i++){
        bits[i] = lsb4 ? *shadowPointer & twoSignificant[i] : *shadowPointer & fourSignificant[i] ;
        bits[i] = lsb4 ? bits[i] >> lsb4Shifter[currentShifterIndex] : bits[i] >>lsb2Shifter[currentShifterIndex];
        currentShifterIndex++;
    }

    int and = lsb4 ? 0xF0 : 0xFC;  // 4 o 6 MSB.
    for (int i = 0 ; i < bytesUsedFromImage ; i++)
        imagePixelPointer[i] = (imagePixelPointer[i] & and) + bits[i];

}
