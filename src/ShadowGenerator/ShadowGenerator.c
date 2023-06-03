//
// Created by shadad on 03/06/23.
//
#include "ShadowGenerator.h"

uint8_t fourSignificant[] = {0xC0, 0x30, 0x0C, 0x03};
uint8_t twoSignificant[] = {0xF0, 0x0F};


static  shadow ** initializeShadowArray(shadowGenerator * shadowGenerator, uint32_t shadowPoints);
static uint8_t evaluatePolynomial(shadowGenerator * shadowGenerator, uint8_t * coefficients, uint8_t value);
static void insertBits(uint8_t  *imagePixelPointer, uint8_t  *shadowPointer, uint8_t k);
static void hideShadow(uint8_t  k , bmpFile * image, shadow * hidingShadow);


shadowGenerator * initialize(struct params * params){
    struct shadowGenerator * shadowGenerator = malloc(sizeof(shadowGenerator));
    shadowGenerator->file = openBmpFile(params->file);
    shadowGenerator->k = params->k;
    shadowGenerator->n = params -> n;
    openDirectory(shadowGenerator, params->directory);
    return shadowGenerator;
}

void distributeSecret(shadowGenerator * shadowGenerator){
    uint32_t shadowPoints = (shadowGenerator->file->header->imageSize) / (shadowGenerator->k - 1);
    struct shadow ** shadowArray  = initializeShadowArray(shadowGenerator, shadowPoints);

    uint8_t * pixelPoints = shadowGenerator->file->pixels;
    uint8_t k = shadowGenerator->k;
    uint32_t currentBlock = 0;
    uint8_t * aCoefficients = malloc( k * sizeof (uint8_t));
    uint8_t * bCoefficients= malloc( k * sizeof (uint8_t));

    srand(time(NULL)); //initialize the seed.
    uint8_t a_0, a_1;

    while(currentBlock < shadowPoints){

        memcpy(aCoefficients, pixelPoints, k);
        memcpy(bCoefficients + 2, pixelPoints + k, k-2 );
        uint8_t random = rand() + 1 ;  //avoid the random number to be zero.
        a_0 = Z_p(aCoefficients[0]) == 0 ? 1: aCoefficients[0];
        a_1 = Z_p(aCoefficients[1]) == 0 ? 1: aCoefficients[1];
        bCoefficients[0] =  Z_p(- random * a_0);
        bCoefficients[1] =  Z_p(- random * a_1);

        for (int j = 0; j < shadowGenerator->n; j++){
            shadowArray[j]->points[currentBlock] = evaluatePolynomial(shadowGenerator,
                                                                      aCoefficients,shadowArray[j]->shadowNumber);
            shadowArray[j]->points[currentBlock + 1] = evaluatePolynomial(shadowGenerator,
                                                                          bCoefficients, shadowArray[j]->shadowNumber);
        }

        pixelPoints += (2 * k) - 2;
        currentBlock +=2;
    }

    shadowGenerator->generatedShadows =  shadowArray;
}

void hideSecret(shadowGenerator * shadowGenerator){
    for (int i = 0 ; i < shadowGenerator -> n ; i ++){
        bmpFile  * currentImageFile = openBmpFile(shadowGenerator->imageFiles[i]);
        shadow * currentShadow = shadowGenerator->generatedShadows[i];
        hideShadow(shadowGenerator->k,currentImageFile, currentShadow);
        write(currentImageFile->fd , currentImageFile, currentImageFile->header->fileSize);
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
        perror("Unable to open the given directory");
        return;
    }
    char ** fileNames = malloc(generator->n * sizeof(char * )) ;

    int currentFile = 0;
    struct dirent * entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        uint64_t directoryLength = strlen(directoryPath);
        fileNames[currentFile] = malloc(directoryLength + 1 + strlen(entry->d_name));
        strcpy(fileNames[currentFile], directoryPath);
        strcpy(fileNames[currentFile] + directoryLength, "/");
        strcpy(fileNames[currentFile] + directoryLength + 1, entry->d_name);
        currentFile ++;
    }
    generator->imageFiles = fileNames;
}

static shadow ** initializeShadowArray(shadowGenerator * shadowGenerator, uint32_t shadowPoints){

     shadow ** shadowArray = malloc(shadowGenerator->n * sizeof(shadow *));
    //initialize all shadow structures.
    for (int i = 0; i < shadowGenerator -> n ; i++){
        shadowArray[i] = malloc(sizeof(struct shadow));
        shadowArray[i]->shadowNumber = i + 1;
        shadowArray[i]->pointNumber = shadowPoints;
        shadowArray[i]->points = malloc(shadowPoints * sizeof (uint8_t));
    }
    return  shadowArray;
}

static uint8_t evaluatePolynomial(struct shadowGenerator * shadowGenerator, uint8_t * coefficients, uint8_t value){
    int x = 1;
    int evaluation = 0;
    for(int i = shadowGenerator->k - 1 ; i >= 0   ; i--){
        evaluation += coefficients[i] * x ;
        x *= value;
    }
    return Z_p(evaluation);
}

static void insertBits(uint8_t  * imagePixelPointer, uint8_t  *shadowPointer, uint8_t  k){
    int lsb4 = ( k == 3 || k == 4 ) ? 1 : 0;
    int bytesUsedFromImage = ( lsb4 ) ? 2 : 4;

    uint8_t  bits[bytesUsedFromImage];

    for(int i = 0; i < bytesUsedFromImage ; i++){
        bits[i] = !lsb4 ? ((*shadowPointer & fourSignificant[i]) >> ((bytesUsedFromImage - i -1) * 2)) : ((*shadowPointer & twoSignificant[i]) >> ((bytesUsedFromImage - i -1) * 4));
    }

    int and = lsb4 ? 0xF0 : 0xFC;  // 4 o 6 MSB.
    for (int i = 0 ; i < bytesUsedFromImage ; i++)
        imagePixelPointer[i] = Z_p((imagePixelPointer[i] & and) + bits[i]);


}
