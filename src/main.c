typedef struct global global_t;
#define usb_callback_data_t global_t
#define fat_callback_usr_t msd_t

#include <usbdrvce.h>
#include <msddrvce.h>
#include <fatdrvce.h>
#include <tice.h>
#include <graphx.h>

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "gfx/gfx.h"

#define MAX_PARTITIONS 32
#define BLOCK_SIZE 512

struct global
{
    usb_device_t usb;
    msd_t msd;
};

enum { USB_RETRY_INIT = USB_USER_ERROR };

static void putstr(char *str)
{
    os_PutStrFull(str);
    os_NewLine();
}

static usb_error_t handleUsbEvent(usb_event_t event, void *event_data,
                                  usb_callback_data_t *global)
{
    switch (event)
    {
        case USB_DEVICE_DISCONNECTED_EVENT:
            putstr("usb device disconnected");
            if (global->usb)
                msd_Close(&global->msd);
            global->usb = NULL;
            break;
        case USB_DEVICE_CONNECTED_EVENT:
            putstr("usb device connected");
            return usb_ResetDevice(event_data);
        case USB_DEVICE_ENABLED_EVENT:
            global->usb = event_data;
            putstr("usb device enabled");
            break;
        case USB_DEVICE_DISABLED_EVENT:
            putstr("usb device disabled");
            return USB_RETRY_INIT;
        default:
            break;
    }

    return USB_SUCCESS;
}

static fat_t fat;

int main(void)
{
    static char buffer[212];
    fat_dir_t dir;
    static msd_partition_t partitions[MAX_PARTITIONS];
    static global_t global;
    uint8_t num_partitions;
    uint24_t blocks_read;
    fat_file_t image_file;
    msd_info_t msdinfo;
    usb_error_t usberr;
    msd_error_t msderr;
    fat_error_t faterr;

    memset(&global, 0, sizeof(global_t));
    os_SetCursorPos(1, 0);

    // usb initialization loop; waits for something to be plugged in
    do
    {
        global.usb = NULL;

        usberr = usb_Init(handleUsbEvent, &global, NULL, USB_DEFAULT_INIT_FLAGS);
        if (usberr != USB_SUCCESS)
        {
            putstr("usb init error.");
            goto usb_error;
        }

        while (usberr == USB_SUCCESS)
        {
            if (global.usb != NULL)
                break;

            // break out if a key is pressed
            if (os_GetCSC())
            {
                putstr("exiting demo, press a key");
                goto usb_error;
            }

            usberr = usb_WaitForInterrupt();
        }
    } while (usberr == USB_RETRY_INIT);
   
    if (usberr != USB_SUCCESS)
    {
        putstr("usb enable error.");
        goto usb_error;
    }

    // initialize the msd device
    msderr = msd_Open(&global.msd, global.usb);
    if (msderr != MSD_SUCCESS)
    {
        putstr("failed opening msd");
        goto usb_error;
    }

    putstr("opened msd");

    // get block count and size
    msderr = msd_Info(&global.msd, &msdinfo);
    if (msderr != MSD_SUCCESS)
    {
        putstr("error getting msd info");
        goto msd_error;
    }

    // print msd sector number and size
    sprintf(buffer, "block size: %u bytes", (uint24_t)msdinfo.bsize);
    putstr(buffer);
    sprintf(buffer, "num blocks: %u", (uint24_t)msdinfo.bnum);
    putstr(buffer);

    // locate the first fat partition available
    num_partitions = msd_FindPartitions(&global.msd, partitions, MAX_PARTITIONS);
    if (num_partitions < 1)
    {
        putstr("no paritions found");
        goto msd_error;
    }

    // attempt to open the first found fat partition
    // it is not required to use a MSD to access a FAT filesystem if the
    // appropriate callbacks are configured.
    for (uint8_t p = 0;;)
    {
        uint32_t base_lba = partitions[p].first_lba;
        fat_callback_usr_t *usr = &global.msd;
        fat_read_callback_t read = &msd_Read;
        fat_write_callback_t write = &msd_Write;

        faterr = fat_Open(&fat, read, write, usr, base_lba);
        if (faterr == FAT_SUCCESS)
        {
            sprintf(buffer, "opened fat partition %u", p - 1);
            putstr(buffer);
            break;
        }

        p++;
        if (p >= num_partitions)
        {
            putstr("no suitable patitions");
            goto msd_error;
        }
    }

    os_ClrHome();   

    faterr = fat_OpenDir(&fat, "/", &dir);
    if (faterr != FAT_SUCCESS)
    {
        putstr("error opening root dir");
        goto fat_error;
    }

    faterr = fat_OpenFile(&fat, "/image.bin", 0, &image_file);
    if (faterr != FAT_SUCCESS)
    {
        putstr("error opening image file");
        goto fat_error;
    }

    if (fat_GetFileSize(&image_file) != GFX_LCD_WIDTH * GFX_LCD_HEIGHT)
    {
        putstr("image file is wrong size");
        while (!os_GetCSC());
        goto fat_error;
    }

    // graphx
    {
        gfx_Begin();
        gfx_SetPalette(global_palette, sizeof_global_palette, 0);

        blocks_read = fat_ReadFile(&image_file, (GFX_LCD_WIDTH * GFX_LCD_HEIGHT) / BLOCK_SIZE, gfx_vbuffer);
        if (blocks_read != ((GFX_LCD_WIDTH * GFX_LCD_HEIGHT) / BLOCK_SIZE))
        {
            putstr("error reading image file");
            goto fat_error;
        }

        while (!os_GetCSC());

        gfx_End();
    }

    faterr = fat_CloseFile(&image_file);
    if (faterr != FAT_SUCCESS)
    {
        putstr("error closing image file");
        goto fat_error;
    }

    fat_Close(&fat);
    msd_Close(&global.msd);
    usb_Cleanup();
    return 0;


fat_error:
    // close the filesystem
    fat_Close(&fat);

msd_error:
    // close the msd device
    msd_Close(&global.msd);

usb_error:
    // cleanup usb
    usb_Cleanup();

    while (!os_GetCSC());

    return 0;
}
