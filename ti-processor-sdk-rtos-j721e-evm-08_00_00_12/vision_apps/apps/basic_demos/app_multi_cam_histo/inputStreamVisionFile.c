/* Copyright (C) D3 Engineering - All Rights Reserved
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "inputStream.h"

//#define INPUT_STREAM_DEBUG

#ifdef INPUT_STREAM_DEBUG
#define DEBUG_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(f_, ...)
#endif

static vx_status openCurFile(struct InputStreamStruct *self, int32_t channel);
static vx_status readDataFromFile(struct InputStreamStruct *self, int32_t channel, uint8_t *dataPtr, uint32_t numBytes, uint32_t *numBytesRead);
static vx_status closeCurFile(struct InputStreamStruct *self, int32_t channel);
static vx_status nextFile(struct InputStreamStruct *self);
static vx_status resetFile(struct InputStreamStruct *self);
static int filterFiles(const struct dirent *d);
static vx_status getCurFileIdx(struct InputStreamStruct *self, int32_t channel, uint32_t *curFileIdx);
static vx_status getNumFiles(struct InputStreamStruct *self, int32_t channel, uint32_t *streamLen);

/* Initialize the inputStream module for vision sensor with file  as physical medium. The function scans the content of each directory for files and store the names in self->_.file.fileList*/
vx_status inputStreamVisionFileInit(InputStream *self, InputStream_InitParams *initParams)
{
    vx_status status;
    int32_t channel;

    status = VX_SUCCESS;

    struct stat sb;
    char *path;

    assert(self->numChannels <= INPUT_STREAM_MAX_CHANNELS);

    self->open = openCurFile;
    self->close = closeCurFile;
    self->next = nextFile;
    self->reset = resetFile;
    self->read = readDataFromFile;
    self->getStreamLen = getNumFiles;
    self->getCurStreamPos = getCurFileIdx;

    for (channel = 0; channel < self->numChannels; channel++)
    {
        self->_.file.numFiles[channel] = 0;
        path = &self->_.file.chanPathList[channel][0];
        strcpy(path, initParams->_.file.path[channel]);
        DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM>>>>: Initialized channel %d with path %s\n", channel, self->_.file.chanPathList[channel]);
        if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
        {
            self->_.file.numFiles[channel] = scandir(path, &self->_.file.fileList[channel], &filterFiles, alphasort);
            self->_.file.curFileIdx[channel] = 0;
            self->_.file.fileDesc[channel] = NULL;
#ifdef INPUT_STREAM_DEBUG
            for (int32_t file = 0; file < self->_.file.numFiles[channel]; file++)
            {
                DEBUG_PRINTF("%u) - %s\n", file, (self->_.file.fileList[channel])[file]->d_name);
            }
#endif
        }
        else
        {
            printf("The channel %d with path %s cannot be opened or is not a directory. Please verify that the files on the media exist\n", channel, path);
            status = VX_FAILURE;
        }
    }

    return status;
}

vx_status inputStreamVisionFileDeInit(InputStream *self)
{
    vx_status status;
    int32_t channel, file;

    status= VX_SUCCESS;

    for (channel = 0; channel < self->numChannels; channel++)
    {
        for (file = 0; file < self->_.file.numFiles[channel]; file++)
        {
            free((self->_.file.fileList[channel])[file]);
        }
        free(self->_.file.fileList[channel]);

        DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM>>>>: De-initialized channel %d with path %s\n", channel, self->_.file.chanPathList[channel]);
    }

    return status;
}

static vx_status openCurFile(struct InputStreamStruct *self, int32_t channel)
{
    vx_status status;
    uint32_t numFiles;
    char file_name[2 * INPUT_STREAM_MAX_FILE_PATH];
    status = VX_SUCCESS;

    getNumFiles(self, channel, &numFiles);

    if (numFiles != 0)
    {
        snprintf(file_name, 2 * INPUT_STREAM_MAX_FILE_PATH, "%s/%s", self->_.file.chanPathList[channel], (self->_.file.fileList[channel])[self->_.file.curFileIdx[channel]]->d_name);
        if ((self->_.file.fileDesc[channel] = fopen(file_name, "rb")) == NULL)
        {
            DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM ERROR>>>>: Unable to open channel %d file %s\n", channel, file_name);
            status = VX_FAILURE;
        }
        else
        {
            DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM>>>>: Opened channel %d file %s\n", channel, file_name);
        }
    }
    else
    {
        DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM ERROR>>>>: No file to stream in channel %d\n", channel);
        status = VX_FAILURE;
    }
    return status;
}

static vx_status readDataFromFile(struct InputStreamStruct *self, int32_t channel, uint8_t *dataPtr, uint32_t numBytes, uint32_t *numBytesRead)
{
    vx_status status;
    uint32_t actualNumBytes;
    status = VX_SUCCESS;

    if ((actualNumBytes=fread(dataPtr, 1, numBytes, self->_.file.fileDesc[channel])) != numBytes)
    {
        DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM ERROR>>>>: Unable to read channel %d, read %d bytes, expected %d bytes\n", channel, actualNumBytes, numBytes);
        status= VX_FAILURE;
    }
    else
    {
        DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM>>>>: Read channel %d, %d bytes\n", channel, numBytes);
    }

    *numBytesRead= actualNumBytes;

    return status;
};

static vx_status closeCurFile(struct InputStreamStruct *self, int32_t channel)
{
    vx_status status;
    status = VX_SUCCESS;

    fclose(self->_.file.fileDesc[channel]);
    DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM>>>>: Closed channel %d file\n", channel);

    return status;
}

static vx_status nextFile(struct InputStreamStruct *self)
{
    vx_status status;
    int32_t channel;
    status = VX_SUCCESS;

    for (channel = 0; channel < self->numChannels; channel++)
    {
        if (self->_.file.curFileIdx[channel] != (self->_.file.numFiles[channel] - 1))
        {
            self->_.file.curFileIdx[channel]++;
            DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM>>>>: Advanced channel %d to file index=%d\n", channel, self->_.file.curFileIdx[channel]);
        }
        else
        {
            if (self->mode == INPUT_STREAM_INFINITE_LOOP)
            {
                self->_.file.curFileIdx[channel] = 0;
            }
            else
            {
                DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM ERROR>>>>: Unable to Advance channel %d to file index=%d\n", channel, self->_.file.curFileIdx[channel] + 1);
                status = VX_FAILURE;
            }
        }
    }
    return status;
}

static vx_status resetFile(struct InputStreamStruct *self)
{
    vx_status status;
    int32_t channel;
    status = VX_SUCCESS;

    for (channel = 0; channel < self->numChannels; channel++)
    {
        self->_.file.curFileIdx[channel] = 0;
        DEBUG_PRINTF("<<<<VIDEO INPUT_STREAM>>>>: Reset channel %d to file index 0\n", channel);
    }
    return status;
}

static int filterFiles(const struct dirent *d)
{
    return !((strcmp(d->d_name,".")==0) || (strcmp(d->d_name,"..")==0) || (d->d_type== DT_DIR));
}

static vx_status getCurFileIdx(struct InputStreamStruct *self, int32_t channel, uint32_t *curFileIdx)
{
    vx_status status= VX_SUCCESS;
    *curFileIdx= self->_.file.curFileIdx[channel];
    return status;
}


static vx_status getNumFiles(struct InputStreamStruct *self, int32_t channel, uint32_t *streamLen)
{
    vx_status status= VX_SUCCESS;
    *streamLen= self->_.file.numFiles[channel];
    return status;
}