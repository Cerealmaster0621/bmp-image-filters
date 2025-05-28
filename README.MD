# Digital Signal Processing Assignment

## Assignment Overview

This program implements various image processing operations on BMP files:

1. Negative Inversion
2. Gamma Correction
3. 90-degree Rotation
4. Rectangle Filling
5. Embossing Effect

## Function Descriptions

### 1. Negative Inversion (negativeInvert)

- Inverts the color values of each pixel (255 - current_value)
- Processes the image row by row
- Maintains the original image structure

### 2. Gamma Correction (gammaCorrection)

- Applies gamma correction to adjust image brightness
- Uses the formula: output = (input/255)^(1/gamma) \* 255
- Allows user to input gamma value

### 3. 90-degree Rotation (matrixRotator)

- Rotates the image 90 degrees clockwise or counter-clockwise
- Uses matrix transposition and row/column reversal
- Maintains color information (BGR format)
- Allows user to choose rotation direction

### 4. Rectangle Filling (fillRectangle)

- Fills a specified rectangular area with a chosen color
- Takes coordinates for top-left and bottom-right corners
- Allows RGB color specification
- Includes boundary checking

### 5. Embossing Effect (emboss)

- Applies a 3x3 emboss kernel to create a raised effect
- Uses a 128 offset for effect emphasis
- Processes each color channel separately
- Includes boundary condition handling

## Technical Specifications

- Image Format: BMP
- Color Depth: 24-bit (8-bit per channel)
- Image Size: 256x256 pixels
