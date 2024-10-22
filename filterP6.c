#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define RGB_COMPONENT_COLOR 255

typedef struct
{
    unsigned char r, g, b;
} PPMPixel;

typedef struct
{
    int width, height;
    PPMPixel *pixel;
} PPMImage;

/*Return RGB value of pixel*/
unsigned char getPixel(int px, int py, int l, PPMImage *img)
{
    switch (l)
    {
    // Red
    case (0):
        return (img->pixel + py * img->width + px)->r;
    // Green
    case (1):
        return (img->pixel + py * img->width + px)->g;
    // Blue
    case (2):
    default:
        return (img->pixel + py * img->width + px)->b;
    }
}

/*Zero padding*/
unsigned char getPixel_check(int px, int py, int l, PPMImage *img)
{
    if ((px < 0) || (px >= img->width) || (py < 0) || py >= img->height)
        return 0;

    return getPixel(px, py, l, img);
}

/*read input file*/
PPMImage *readPPM(char *filename)
{
    FILE *fp;
    char buf[16];
    int rgb_comp_color;

    fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("Unable to open file  '%s' \n", filename);
        exit(-1);
    }

    /*File type*/
    fgets(buf, sizeof(buf), fp);
    if (buf[0] != 'P' || buf[1] != '6')
    {
        printf("Invalid image format\n");
        exit(-1);
    }

    /*Reading past comments*/
    int c = getc(fp);
    while (c == '#')
    {
        while (getc(fp) != '\n')
            c = getc(fp);
    }
    ungetc(c, fp);

    PPMImage *img = (PPMImage *)malloc(sizeof(PPMImage) + 1);
    if (!img)
    {
        printf("Unable to allocate memory\n");
        exit(-1);
    }

    if (fscanf(fp, "%d %d", &img->width, &img->height) != 2)
    {
        printf("Invalid image size\n");
        exit(-1);
    }

    if (fscanf(fp, "%d", &rgb_comp_color) != 1)
    {
        printf("Invalid rgb component\n");
        exit(-1);
    }

    if (rgb_comp_color != RGB_COMPONENT_COLOR)
    {
        printf(" '%s' doesnot have 8-bits component \n", filename);
        exit(-1);
    }

    while (fgetc(fp) != '\n');

    img->pixel = (PPMPixel *)malloc(img->width * img->height * sizeof(PPMPixel) + 1);
    if (!img->pixel)
    {
        printf("Unable to allocate memory \n");
        exit(-1);
    }

    /*read pixel data from file*/
    fread(img->pixel, sizeof(PPMPixel), img->width * img->height, fp);
    fclose(fp);
    return img;
}

/*write to output file*/
void writePPM(PPMImage *img, char *filename)
{
    FILE *fp;
    fp = fopen(filename, "wb");
    if (!fp)
    {
        printf("Unable to open file '%s' \n", filename);
        exit(-1);
    }

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", img->width, img->height);
    fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

    fwrite(img->pixel, sizeof(PPMPixel), img->width * img->height, fp);
    fclose(fp);
}

/*apply convolution kernel to input file*/
PPMImage *applyKernel(PPMImage *img, char *kernelfile)
{

    FILE *fp = fopen(kernelfile, "rb");
    char buf[16];
    if (!fp)
        return NULL;
    int Ks;
    int div;
    fscanf(fp, "%d", &Ks);
    fscanf(fp, "%d", &div);

    double *K = (double *)malloc(Ks * Ks * sizeof(double)); /*Kernel parameters*/
    if (!K)
    {
        printf("Unable to allocate memory \n");
        exit(-1);
    }

    for (int i = 0; i < Ks; i++)
    {
        for (int j = 0; j < Ks; j++)
        {
            fscanf(fp, "%s", buf);
            K[i * Ks + j] = atof(buf);
        }
    }

    fclose(fp);

    PPMImage *out_image = (PPMImage *)malloc(sizeof(PPMImage));
    if (!out_image)
    {
        printf("Unable to allocate memory \n");
        exit(-1);
    }
    out_image->height = img->height;
    out_image->width = img->width;
    out_image->pixel = (PPMPixel *)malloc(out_image->height * out_image->width *
                                          sizeof(PPMPixel));
    if (!out_image->pixel)
    {
        printf("Unable to allocate memory \n");
        exit(-1);
    }

    Ks /= 2;
    double cp[3];
    /* iterate through image */
    for (int ix = 0; ix < img->width; ix++)
    {
        for (int iy = 0; iy < img->height; iy++)
        {
            cp[0] = cp[1] = cp[2] = 0.0;
            for (int kx = -Ks; kx <= Ks; kx++)
            { /* Apply the kernel */
                for (int ky = -Ks; ky <= Ks; ky++)
                {
                    for (int l = 0; l < 3; l++)
                    {
                        cp[l] += (K[kx + Ks + (ky + Ks) * (Ks * 2 + 1)] *
                                  (double)(getPixel_check(ix + kx, iy + ky, l, img)));
                    }
                }
            }
            for (int i = 0; i < 3; i++)
            {
                cp[i] /= (double)div;
                /*Fit to bounds*/
                cp[i] = (cp[i] > RGB_COMPONENT_COLOR) ? 255 : (cp[i] < 0) ? 0 : cp[i];
            }
            /*Assigning to image*/
            (out_image->pixel[ix + iy * out_image->width]).r = (unsigned char)cp[0];
            (out_image->pixel[ix + iy * out_image->width]).g = (unsigned char)cp[1];
            (out_image->pixel[ix + iy * out_image->width]).b = (unsigned char)cp[2];
        }
    }
    free(K);
    return out_image;
}
