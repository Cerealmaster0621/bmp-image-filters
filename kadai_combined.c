/*
    ディジタル信号処理 課題1
    1, 2, 3, 4, 5 統合プログラム
    21D8101001H YoungJun Kang

    .BMP file consists of 14 bytes of headers and the rests are actual pixel datas.
    
*/

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include<stdbool.h>

#define WIDTH 256
#define HEIGHT 256
#define BYTES_PER_PIXEL 3

// in BMP File format, pixel datas in every row should be divided by 4 Bytes
// ex - when single line is 3, 6, 9,,, bytes, so not divided by 4, fill the line
//      with padding so it will return 4,8,12 
// int calculateRowSize -> calculate the row size including padding.
int calculateRowSize(int width, int bytesPerPixel){
    int rowSize = width * bytesPerPixel;

    int padding = 0;
    if (rowSize % 4 != 0){
        padding = 4 - (rowSize % 4);
    }

    return rowSize + padding;
}



// <---------------------------->
// INITIAL FILE HANDLING PROCESS
// <---------------------------->
int initFileProcess(char* inputFileName, char* outputFileName, FILE** inputFile, FILE** outputFile){
    printf("input file Name : ");
    scanf("%s", inputFileName);

    printf("output file Name : ");
    scanf("%s", outputFileName);

    *inputFile = fopen(inputFileName, "rb");
    if(*inputFile == NULL){
        printf("input file name %s does not exist!", inputFileName);
        return 0;
    }
    
    *outputFile = fopen(outputFileName, "wb");
    if(*outputFile == NULL){
        printf("output file %s cannot be created", outputFileName);
        fclose(*inputFile);
        return 0;
    }

    uint8_t header[54]; 
    fread(header, sizeof(uint8_t), 54, *inputFile);
    fwrite(header, sizeof(uint8_t), 54, *outputFile);

    fseek(*inputFile, 54, SEEK_SET);
    fseek(*outputFile, 54, SEEK_SET);

    return 1; // for success
}



// <-------------------------------------------->
//          KADAI 1 - NEGATIVE INVERT
// <-------------------------------------------->
void negativeInvert(FILE* inputFile, FILE* outputFile, int rowSize){
    uint8_t row[rowSize];
    
    for(int y = 0; y < HEIGHT; y++){
        fread(row, sizeof(uint8_t), rowSize, inputFile);
        for(int x = 0; x < WIDTH * BYTES_PER_PIXEL; x++){
            row[x] = 255 - row[x]; // invert
        }
        fwrite(row, sizeof(uint8_t), rowSize, outputFile);
    }
}



// <-------------------------------------------->
//          KADAI 2 - GAMMA CORRECTION
// <-------------------------------------------->
void gammaCorrection(FILE* inputFile, FILE* outputFile, int rowSize, double gamma){
    uint8_t row[rowSize];
    
    for(int y = 0; y < HEIGHT; y++){
        fread(row, sizeof(uint8_t), rowSize, inputFile);
        for(int x = 0; x < WIDTH * BYTES_PER_PIXEL; x++){
            double normal = row[x] / 255.0;
            double val = pow(normal, 1.0 / gamma);
            row[x] = round(val * 255.0);
        }
        fwrite(row, sizeof(uint8_t), rowSize, outputFile);
    }
}



// <-------------------------------------------->
//         KADAI 3 - 90 DEGREES ROTATOR
// <-------------------------------------------->
void matrixRotator(FILE* inputFile, FILE* outputFile, bool isClockwise, int rowSize) {
    const int imageSize = HEIGHT * rowSize;
    // create buffer var copying entire image input
    // rotate **entire** image matrix, not every single pixel.
    uint8_t* buffer = (uint8_t*)malloc(imageSize);
    uint8_t* row = (uint8_t*)malloc(imageSize);

    // Read entire image
    fread(buffer, sizeof(uint8_t), imageSize, inputFile);
    
    // Transpose matrix
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            // Copy all 3 color components (BGR)
            for(int c = 0; c < BYTES_PER_PIXEL; c++) {
                int fromPosition = y * rowSize + x * BYTES_PER_PIXEL + c;
                int toPosition = x * rowSize + y * BYTES_PER_PIXEL + c;
                row[toPosition] = buffer[fromPosition];
            }
        }
    }
    
    // Reverse rows/columns
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            for(int c = 0; c < BYTES_PER_PIXEL; c++) {
                int fromPosition = y * rowSize + x * BYTES_PER_PIXEL + c;
                int toPosition;
                
                if(isClockwise) {
                    // Reverse rows
                    toPosition = y * rowSize + (WIDTH - 1 - x) * BYTES_PER_PIXEL + c;
                } else {
                    // Reverse columns
                    toPosition = (HEIGHT - 1 - y) * rowSize + x * BYTES_PER_PIXEL + c;
                }
                
                buffer[toPosition] = row[fromPosition];
            }
        }
    }
    
    fwrite(row, sizeof(uint8_t), imageSize, outputFile);
    
    free(buffer);
    free(row);
}



// <-------------------------------------------->
//         KADAI 4 - Fill Rectangle
// <-------------------------------------------->
void fillRectangle(FILE* inputFile, FILE* outputFile, int rowSize, int R, int G, int B, int x1, int x2, int y1, int y2) {
    uint8_t* buffer = (uint8_t*)malloc(HEIGHT * rowSize);
    
    fread(buffer, sizeof(uint8_t), HEIGHT * rowSize, inputFile);

    // Fill the rectangle with specified color
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            int position = i * rowSize + j * BYTES_PER_PIXEL;
            
            if(i >= y1 && i <= y2 && j >= x1 && j <= x2) { // change color
                buffer[position] = (uint8_t)B;     // Blue
                buffer[position + 1] = (uint8_t)G; // Green
                buffer[position + 2] = (uint8_t)R; // Red
            }
        }
    }
    
    fwrite(buffer, sizeof(uint8_t), HEIGHT * rowSize, outputFile);
    
    free(buffer);
}

// <-------------------------------------------->
//               KADAI 5 - Embossing effect
// <-------------------------------------------->
void emboss(FILE* inputFile, FILE* outputFile, int rowSize) {
    uint8_t* buffer = (uint8_t*)malloc(HEIGHT * rowSize);
    uint8_t* outputBuffer = (uint8_t*)malloc(HEIGHT * rowSize);
    
    fread(buffer, sizeof(uint8_t), HEIGHT * rowSize, inputFile);
    
    // Emboss kernel
    int kernel[3][3] = {
        {-2, -1, 0},
        {-1,  1, 1},
        { 0,  1, 2}
    };
    
    const int offset = 128;
    
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            // Process each color channel (BGR)
            for(int c = 0; c < BYTES_PER_PIXEL; c++) {
                int sum = 0;
                
                // Apply kernel
                for(int ky = -1; ky <= 1; ky++) {
                    for(int kx = -1; kx <= 1; kx++) {
                        int pixelX = x + kx;
                        int pixelY = y + ky;
                        
                        // Check boundaries
                        if(pixelX >= 0 && pixelX < WIDTH && pixelY >= 0 && pixelY < HEIGHT) {
                            int pos = pixelY * rowSize + pixelX * BYTES_PER_PIXEL + c;
                            sum += buffer[pos] * kernel[ky+1][kx+1];
                        }
                    }
                }
                
                // Add offset and clamp to 0-255
                int finalValue = sum + offset;
                if(finalValue < 0) finalValue = 0;
                if(finalValue > 255) finalValue = 255;
                
                // Store result in output buffer
                int outPos = y * rowSize + x * BYTES_PER_PIXEL + c;
                outputBuffer[outPos] = (uint8_t)finalValue;
            }
        }
    }
    
    fwrite(outputBuffer, sizeof(uint8_t), HEIGHT * rowSize, outputFile);
    
    free(buffer);
    free(outputBuffer);
}

int main(int argc, char** argv){
    char inputFileName[100];
    char outputFileName[100];
    FILE* inputFile;
    FILE* outputFile;

    int ROW_SIZE = calculateRowSize(WIDTH, BYTES_PER_PIXEL);
    int modeInt;

    printf("select mode\n");
    printf("1. negative invertor    2. gamma corrector\n");
    printf("3. 90 degrees rotator   4. rectangle painter\n");
    printf("5. embossing effect\n");
    while(1){
        scanf("%d", &modeInt);
        if(5 < modeInt || 1 > modeInt){
            printf("please select 1 - 5\n");
        }else {
            break;
        }
    }


    // Display selected mode
    switch(modeInt){
        case 1:
            printf("==========================\n");
            printf("BMP FILE NEGATIVE INVERTOR\n");
            printf("==========================\n");
            break;
        case 2:
            printf("========================\n");
            printf("BMP FILE GAMMA CORRECTOR\n");
            printf("========================\n");
            break;
        case 3:
            printf("===========================\n");
            printf("BMP FILE 90 DEGREES ROTATOR\n");
            printf("===========================\n");
            break;
        case 4:
            printf("==========================\n");
            printf("BMP FILE RECTANGLE PAINTER\n");
            printf("==========================\n");
            break;
        case 5:
            printf("=========================\n");
            printf("BMP FILE EMBOSSING EFFECT\n");
            printf("=========================\n");
            break;
    }

    // Initialization
    if(!initFileProcess(inputFileName, outputFileName, &inputFile, &outputFile)){
        printf("something wrong happened while processing File i/o\n");
        return 1;
    }

    int x1, y1, x2, y2;
    int R, G, B;

    // SELECTED MODE PROCESS
    switch(modeInt){
        case 1: // negative invert
            negativeInvert(inputFile, outputFile, ROW_SIZE);
            printf("finished negative invert. saved %s\n", outputFileName);
            break;
            
        case 2: // gamma correction
            {
                double gamma;
                printf("enter the gamma value: ");
                scanf("%lf", &gamma);
                gammaCorrection(inputFile, outputFile, ROW_SIZE, gamma);
                printf("finished gamma correction. saved %s\n", outputFileName);
            }
            break;
            
        case 3: // 90 degrees rotation
            {
                char isClockwise;
                printf("rotate clockwise? y/n\n");
                while(1) {
                    scanf(" %c", &isClockwise);
                    if(isClockwise == 'y' || isClockwise == 'Y') {
                        matrixRotator(inputFile, outputFile, true, ROW_SIZE);
                        printf("finished 90 degrees clockwise rotation. saved %s\n", outputFileName);
                        break;
                    } else if(isClockwise == 'n' || isClockwise == 'N') {
                        matrixRotator(inputFile, outputFile, false, ROW_SIZE);
                        printf("finished 90 degrees counter-clockwise rotation. saved %s\n", outputFileName);
                        break;
                    } else {
                        printf("Please enter y or n: ");
                    }
                }
            }
            break;
        case 4: // rectangle painting
            printf("please enter the left-upper side coordination(x1 y1): \n");
            scanf("%d %d", &x1, &y1);
            printf("please enter the right-bottom side coordination(x2 y2): \n");
            scanf("%d %d", &x2, &y2);


            printf("please enter the desirable square color: (R G B)\n");
            scanf("%d %d %d", &R, &G, &B);

            fillRectangle(inputFile, outputFile, ROW_SIZE, R,G,B, x1, x2, y1, y2);
            printf("finished painting rectangle. saved %s\n", outputFileName);
            break;
        case 5: // embossing effect
            emboss(inputFile, outputFile, ROW_SIZE);
            printf("finished embossing effect. saved %s\n", outputFileName);
            break;
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}