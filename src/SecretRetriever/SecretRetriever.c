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
// valids the equation on the generation.
static void checkCoefficients(uint8_t  k ,uint8_t * coefficients);



shadowGenerator * initRetriever(struct params * params){
    struct shadowGenerator * shadowGenerator = malloc(sizeof(shadowGenerator));
    shadowGenerator->k = params->k;
    shadowGenerator->n = params -> n;
    openDirectory(shadowGenerator, params->directory);
    shadowGenerator->creatingFileName = malloc(strlen(params->file));
    strcpy(shadowGenerator->creatingFileName, params->file);
    return shadowGenerator;
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
    generator->file = malloc(sizeof(bmpFile * ));
    int headerSize = currentImageFile->header->fileSize - currentImageFile->header->imageSize;
    generator->file->header = malloc(headerSize * sizeof (uint8_t));
    memcpy(generator->file->header, currentImageFile->header, headerSize);
    generator->file->pixels = calloc(generator->file->header->imageSize,1 );

}

void retrieveSecret(shadowGenerator * generator){
    uint8_t  k = generator->k ;
    uint8_t  * imagePointer = generator->file->pixels;
    uint32_t currentBlock = 0;

    uint8_t  * xCoordinates = malloc(generator->k);
    uint8_t  * aPoints = malloc(generator->k);
    uint8_t  * bPoints = malloc(generator->k);

    while( currentBlock < generator->generatedShadows[0]->pointNumber){

        for (int i = 0; i < k ; i ++){
            xCoordinates[i] = generator->generatedShadows[i]->shadowNumber;
            aPoints[i] = generator->generatedShadows[i]->points[currentBlock];
            bPoints[i] = generator->generatedShadows[i]->points[currentBlock + 1];
        }
        uint8_t * coefficients = interpolate(k, xCoordinates,  aPoints, bPoints);
        checkCoefficients(k, coefficients);
        memcpy(imagePointer, coefficients, k); // saving a_0 .... a_k-1 coeff
        memcpy(imagePointer + k, coefficients + k + 2, k - 2); //save b_2 .. b_k-1 coeff

        imagePointer += (2*k) - 2;
        currentBlock += 2 ;
    }


    int fd = open(generator->creatingFileName, O_WRONLY | O_CREAT);
    if (fd == -1) {
        perror("open");
        return ;
    }

    //save the retrieved image.
    int headerSize = generator->file->header->fileSize - generator->file->header->imageSize;
    write(fd, generator->file->header, headerSize);
    write(fd, generator->file->pixels, generator->file->header->imageSize);
    close(fd);
}






static shadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile){
    shadow * shadow = malloc(sizeof (shadow));
    shadow->shadowNumber = imageFile->header->reserved1;
    shadow->pointNumber = imageFile->header->imageSize / (k -1);
    shadow->points = malloc(shadow->pointNumber);

    int lsb4 = ( k == 3 || k == 4 ) ? 1 : 0;
    int ImageBytesToShadowByte= ( lsb4 ) ? 2 : 4; // if lsb4 you need two uint8_t from image to generate a shadow uint8_t
    int bitOperator = lsb4 ? 0x0f:0x03; // four or two least significant bits.
    uint8_t  lsb4Shifter[] = {4, 0};
    uint8_t  lsb2Shifter[] = {6,4,2, 0};



    uint64_t currentShadowBlock = 0;
    while(currentShadowBlock < shadow->pointNumber){
        shadow->points[currentShadowBlock] = 0;
        uint8_t  currentShifter = 0;
        for(uint64_t i = 2 * currentShadowBlock ; i < (2* currentShadowBlock + ImageBytesToShadowByte); i++){
            shadow->points[currentShadowBlock] += imageFile->pixels[i]  & bitOperator;
            uint8_t shifter = lsb4 ? lsb4Shifter[currentShifter] : lsb2Shifter[currentShifter];
            currentShifter++;
            shadow->points[currentShadowBlock] = shadow->points[currentShadowBlock] << shifter;
        }
        currentShadowBlock++;
    }

    return shadow;
}


static uint8_t  * interpolate(uint8_t  k , uint8_t * xCoordinates, uint8_t * aPoints, uint8_t * bPoints){
    uint8_t  * coefficient = malloc( 2* k* sizeof(uint8_t));
    memcpy(coefficient, interpolatePolynomial(k, aPoints, xCoordinates), k);
    memcpy(coefficient + k , interpolatePolynomial(k , bPoints, xCoordinates) , k);
    return  coefficient;
}


void checkCoefficients(uint8_t  k ,uint8_t * coefficients){
    int valid = 0;
    uint8_t  a_0 = Z_p(coefficients[0]) == 0 ? 1 : Z_p(coefficients[0]);
    uint8_t  a_1= Z_p(coefficients[1]) == 0  ?  1 :  Z_p(coefficients[1]);


    for (int i = 0; i < 251; i++){
        if ( (coefficients[k] = Z_p(- i * a_0 )) &&
                (coefficients[k+1] = Z_p(- i * a_1) )
        )
            valid = 1;
    }
    if (! valid){
        printf("One invalid shadow was provided. ");
        exit(-1);
    }

    return ;
}


static uint8_t  * interpolatePolynomial(uint8_t k , uint8_t * points, uint8_t * xCoordinates){

    uint8_t  * coefficients = malloc(k * sizeof(uint8_t));

    int currentCoeff = 0;
    int yPrimes[k];

    while (currentCoeff < k) {
        int currentCoefficient = 0;
        int top = k-currentCoeff;

        for (int i = 0; i<top; i++) {
            int y = currentCoeff == 0 ? points[i] : (yPrimes[i] - coefficients[currentCoeff-1]) * inverses[Z_p(xCoordinates[i])];
            yPrimes[i]  = Z_p(y);
            int li = 1;
            for (int j=0; j<top; j++){
                if( j != i)
                    li *= Z_p(-1*xCoordinates[j]*inverses[Z_p(xCoordinates[i]- xCoordinates[j])]);
            }
            currentCoefficient += Z_p(yPrimes[i]*li);
        }

        coefficients[currentCoeff++] = Z_p(currentCoefficient);
    }

    return coefficients;
}

