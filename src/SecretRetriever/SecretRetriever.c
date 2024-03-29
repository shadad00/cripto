//
// Created by shadad on 03/06/23.
//
#include "SecretRetriever.h"

//inverse the lsbx method and create the shadows from the image passed as parameters.
static shadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile);

// take the aPoints and bPoints and make a coefficient vector with the corresponding polynomial.
static uint8_t  * interpolate(uint8_t  k ,uint8_t * xCoordinates,  uint8_t * aPoints, uint8_t * bPoints);

//aux function for interpolate one array of points.
static uint8_t  * interpolatePolynomial(uint8_t n , uint8_t * points, uint8_t * xCoordinates) ;

//check if all shadows are valid by looking the random value that
// validates the equation on the generation.
static void checkCoefficients(uint8_t  k ,uint8_t * coefficients);


shadowGenerator * initRetriever(struct params * params){
    struct shadowGenerator * generator = malloc(sizeof(shadowGenerator));
    generator->k = params->k;
    generator->n = params -> n;
    openDirectory(generator, params->directory);
    generator->creatingFileName = malloc(strlen(params->file) + 1 );
    strcpy(generator->creatingFileName, params->file);
    return generator;
}

void initializeShadows(shadowGenerator * generator){
    shadow ** parsedShadows = malloc(generator->k * (sizeof(shadow *) ));

    bmpFile  * currentImageFile;
    for (int i = 0 ; i < generator -> k ; i ++){
        currentImageFile = openBmpFile(generator->imageFiles[i]);
        parsedShadows[i] = fromImageToShadow(generator->k, currentImageFile);
    }

    //save the shadows.
    generator->generatedShadows = parsedShadows;

    //copy the header file for the generating image.
    generator->file = malloc(sizeof(bmpFile ));

    int headerSize = currentImageFile->header->fileSize - currentImageFile->header->imageSize;
    generator->file->header = malloc(headerSize);
    memcpy(generator->file->header, currentImageFile->header, headerSize);

    generator->file->pixels = calloc(generator->file->header->imageSize,1 );

}

void retrieveSecret(shadowGenerator * generator){

    uint8_t  k = generator->k ;
    uint8_t  * xCoordinates = malloc(k);
    uint8_t  * aPoints = malloc(k);
    uint8_t  * bPoints = malloc(k);

    uint8_t  * imagePointer = generator->file->pixels;
    uint64_t totalBlock = ((generator->file->header->imageSize) / (k - 1));

    for(uint64_t currentBlock = 0; currentBlock < totalBlock; currentBlock+=2, imagePointer += (2 * k) - 2){

        for (int i = 0; i < k ; i ++){
            xCoordinates[i] = generator->generatedShadows[i]->shadowNumber;
            aPoints[i] = generator->generatedShadows[i]->points[currentBlock];
            bPoints[i] = generator->generatedShadows[i]->points[currentBlock + 1];
        }

        //get the polynomial that matches the shadows evaluations.
        uint8_t * coefficients = interpolate(k, xCoordinates,  aPoints, bPoints);

        //check if there is some valid nonce value.
        checkCoefficients(k, coefficients);

        //save the coefficients on the image.
        memcpy(imagePointer, coefficients, k); // saving a_0 .... a_k-1 coeff
        memcpy(imagePointer + k, coefficients + k + 2, k - 2); //save b_2 .. b_k-1 coeff
    }


    int fd = open(generator->creatingFileName, O_WRONLY | O_CREAT);
    if (fd == -1) {
        printf("Unable to create the file for the retrieved image.\n");
        exit(EXIT_FAILURE) ;
    }

    //save the retrieved image.
    int headerSize = generator->file->header->fileSize - generator->file->header->imageSize;
    lseek(fd, 0, SEEK_SET);
    write(fd, generator->file->header, headerSize);
    write(fd, generator->file->pixels, generator->file->header->imageSize);
    close(fd);
}



static shadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile){
    shadow * newShadow = malloc(sizeof (shadow));
    newShadow->shadowNumber = imageFile->header->reserved1;
    newShadow->pointNumber = imageFile->header->imageSize / (k - 1);
    newShadow->points = calloc(newShadow->pointNumber, 1);

    int lsb4 = ( k == 3 || k == 4 ) ? 1 : 0;
    int ImageBytesToShadowByte= ( lsb4 ) ? 2 : 4; // if lsb4 you need two uint8_t from image to generate a newShadow uint8_t
    int bitOperator = lsb4 ? 0x0f:0x03; // four or two least significant bits.
    uint8_t shifter = lsb4 ? 4 : 2;

    for (uint64_t currentShadowBlock = 0; currentShadowBlock < newShadow->pointNumber ; currentShadowBlock++)
       for(uint64_t i = (ImageBytesToShadowByte) * currentShadowBlock ;
       i < (ImageBytesToShadowByte* ( currentShadowBlock + 1)); i++){
           newShadow->points[currentShadowBlock] += imageFile->pixels[i] & bitOperator;
            if (i + 1 != ((ImageBytesToShadowByte* ( currentShadowBlock + 1))) )
                newShadow->points[currentShadowBlock] = newShadow->points[currentShadowBlock] << shifter;
        }

    return newShadow;
}


static uint8_t  * interpolate(uint8_t  k , uint8_t * xCoordinates, uint8_t * aPoints, uint8_t * bPoints){
    uint8_t  * coefficient = malloc( 2* k);
    memcpy(coefficient, interpolatePolynomial(k, aPoints, xCoordinates), k);
    memcpy(coefficient + k , interpolatePolynomial(k , bPoints, xCoordinates) , k);
    return  coefficient;
}


void checkCoefficients(uint8_t  k ,uint8_t * coefficients){
    uint8_t  a_0 = mod(coefficients[0]) == 0 ? 1 : coefficients[0];
    uint8_t  a_1 = mod(coefficients[1]) == 0  ?  1 :  coefficients[1];

    for (int i = 1 ; i < 251; i++){
        if ( (coefficients[k] == mul(mod(-i) , a_0 )) &&
                (coefficients[k+1] == mul(mod(-i),  a_1) )
        )
            return;
    }

    printf("One invalid shadow was provided. ");
    exit(EXIT_FAILURE);
}


static uint8_t  * interpolatePolynomial(uint8_t k , uint8_t * points, uint8_t * xCoordinates){

    uint8_t  * coefficients = calloc(k , 1);
    uint8_t yPrimes[k];

    for( uint8_t S_i = 0; S_i < k ; S_i ++ ) {
        uint8_t currentCoefficient = 0;
        uint8_t neededPoints = k - S_i;

        for (uint8_t i = 0; i < neededPoints; i++) {
            yPrimes[i] = (S_i == 0) ? mod(points[i]) : modDiv(sub(yPrimes[i], coefficients[S_i - 1]), xCoordinates[i]);
            uint8_t li = 1;
            for (int j = 0; j < neededPoints; j++)
                if (j != i)
                    li = mul(li, mul(-1, modDiv(xCoordinates[j], sub(xCoordinates[i], xCoordinates[j]))));
            currentCoefficient = sum(currentCoefficient, mul(yPrimes[i], li));
        }
        coefficients[S_i] = currentCoefficient;
    }

    return coefficients;
}

