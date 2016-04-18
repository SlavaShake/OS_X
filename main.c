//
//  main.c
//  First_Project
//
//  Created by Admin on 14.04.16.
//  Copyright © 2016 Admin. All rights reserved.
//

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>  
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <zlib.h>


#ifndef BUF_SIZE /* Allow "cc -D" to override definition */
#define BUF_SIZE 4194304
#endif

void Copy_File(char Trace_File[]);
void Get_Time_DMY (char dmy_time[]);
void Get_Path_File (char Path_File[],char Trace_Backup_File[]);
void Get_Path_Folder (char Trace_Backup_Folder[]);
int  Create_Conteiner (char *infilename, char *outfilename);
unsigned long file_size(char *filename);



int main()
{
    
    char *F[4];
    F[0] =  "/Users/admin/Desktop/File/red.jpg";
    F[1] =  "/Users/admin/Desktop/File/red.txt";
    F[2] =  "/Users/admin/Desktop/File/redd.jpg";
    F[3] =  "/Users/admin/Desktop/File/rred.jpg";
    for(int i = 0; i < 4;i++)
    {
        Copy_File(F[i]);
        Create_Conteiner(F[i], "/Users/admin/Desktop/keddr.zip");

    }
    
   
    
}


void Copy_File(char Path_File[])
{
    
    
    char *F[2];
    char Path_Backup_Folder [256] ="";
    char Path_Backup_File [256] = "";
    
    Get_Path_File(Path_File, Path_Backup_File);
    Get_Path_Folder(Path_Backup_Folder);
    
    
    
    mkdir(Path_Backup_Folder,0755);
    F[0] = Path_File;
    F[1] = Path_Backup_File;
    
    
    int inputFd;
    int outputFd;
    int openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];
    
    
      /* open input and outpu files */
    inputFd = open(F[0], O_RDONLY);
    if (inputFd == -1)
        printf("opening file %s", F[0]);
    
    
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /*rw-rw-rw*/
    
    outputFd = open(F[1], openFlags, filePerms);
    
    if (outputFd == -1)
        printf("opening file %s", F[1]);
    
    /* transfer data until we encounter end of input or an error */
    
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
    {
        if (write(outputFd, buf, numRead) != numRead)
            printf("couldn't write whole buffer");
    }
    
    if (numRead == -1)
        printf("read");
    
    if (close(inputFd) == -1)
        printf("close input");
    if (close(outputFd) == -1)
        printf("close output");
    
    
    
    //exit(EXIT_SUCCESS);
   
    
    
}


void Get_Time_DMY (char dmy_time[])
{
    //Переменная для системного времени
    long int s_time;
    //Указатель на структуру с локальным временем
    struct tm *m_time;
    //Строка для сохранения преобразованного времени
    
    
    //Считываем системное время
    s_time = time (NULL);
    //Преобразуем системное время в локальное
    m_time = localtime (&s_time);
    
    //Преобразуем локальное время в текстовую строку
    strftime (dmy_time, 128, "_%d_%m_%Y", m_time);
    
}


void Get_Path_File (char Path_File[],char Trace_Backup_File[])
{
    char str_t[256]; // для строки времени
    Get_Time_DMY(str_t);//заполняем строку
    /* Ищем колличество слешей*/
    int N_sl = 0,i = 0,j=0;
    for( int i = 0;Path_File[i] != '\0';i++)
    {
        if(Path_File[i] == '/')
            N_sl++;
    }
    
    /*Определение путь к файлу и его имя*/
    char Pre_Name_File[150] ="";
    char Post_Name_File[150] = "";
    while (Path_File[i] != '\0')
    {
        if (N_sl > 0) Pre_Name_File[i] = Path_File[i];
        if (N_sl == 0)
        {
            Post_Name_File[j] = Path_File[i];
            j++;
        }
        if ( Path_File[i] == '/') N_sl--;
        i++;
    }
    
    strcat(Trace_Backup_File,"/Users/admin/Desktop/Backup");
    strcat(Trace_Backup_File, str_t);
    strcat(Trace_Backup_File, "/");
    strcat(Trace_Backup_File,Post_Name_File);
    
    //printf("%s\n",Trace_Backup_File);

    
}

void Get_Path_Folder (char Trace_Backup_Folder[])
{
    char str_t[256]; // для строки времени
    Get_Time_DMY(str_t);//заполняем строку
    strcat(Trace_Backup_Folder,"/Users/admin/Desktop/Backup");
    strcat(Trace_Backup_Folder, str_t);
    strcat(Trace_Backup_Folder, "/");
    
}

int Create_Conteiner (char *infilename, char *outfilename)
{
    FILE *infile = fopen(infilename, "rb");
    gzFile outfile = gzopen(outfilename, "wb");
    if (!infile || !outfile) return -1;
    
    
    char inbuffer[128];
    int num_read = 0;
    unsigned long total_read = 0, total_wrote = 0;
    while ((num_read = (fread(inbuffer, 1, sizeof(inbuffer), infile))) > 0) {
        total_read += num_read;
        gzwrite(outfile, inbuffer, num_read);
        
    }
    fclose(infile);
    gzclose(outfile);
    printf("Read %ld bytes, Wrote %ld bytes,Compression factor %4.2f%%\n",total_read, file_size(outfilename),(1.0-file_size(outfilename)*1.0/total_read)*100.0);
    return 1;
}


unsigned long file_size(char *filename)
{
    FILE *pFile = fopen(filename, "rb");
    fseek (pFile, 0, SEEK_END);
    unsigned long size = ftell(pFile);
    fclose (pFile);
    return size;
}








