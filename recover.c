#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    int count = 0;
    int number;
    char filename[8];
    int flag = 0;
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "r");//open memory card
    FILE *img = NULL;//intialize img to NULL
    typedef uint8_t BYTE;
    BYTE buffer[512];
    while (!feof(file))//while end of memory card is reached
    {
        number = fread(buffer, sizeof(BYTE), 512, file);
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)//JPEG found
        {
            if (!flag)//first jpeg
            {
                flag = 1;//process started
                sprintf(filename, "%03i.jpg", count);
                count++;
                img = fopen(filename, "w");//create and open a jpg file to write
                fwrite(buffer, sizeof(BYTE), number, img);
                if (number < 512)//jpg terminated before end of block
                {
                    fclose(img);//close jpg
                    img = NULL;
                }
            }
            else//next jpg
            {
                if (img != NULL)
                {
                    fclose(img);//close previous jpg
                    img = NULL;
                }
                sprintf(filename, "%03i.jpg", count);
                count++;
                img = fopen(filename, "w");//start next jpeg
                fwrite(buffer, sizeof(BYTE), number, img);
                if (number < 512)//jpg terminated before end of block
                {
                    fclose(img);
                    img = NULL;
                }
            }
        }
        else//not found new jpeg
        {
            if (flag)//continue to copy
            {
                fwrite(buffer, sizeof(BYTE), number, img);
                if (number < 512)
                {
                    fclose(img);
                }
            }
        }
    }
    if (img != NULL)
    {
        fclose(img);
    }
    fclose(file);
}
