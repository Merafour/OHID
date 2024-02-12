/*******************************************************
 Windows HID simplification

 Alan Ott
 Signal 11 Software

 8/22/2009

 Copyright 2009

 This contents of this file may be used by anyone
 for any reason without any conditions and may be
 used as a starting point for your own applications
 which use HIDAPI.
********************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include "hidapi/hidapi/hidapi.h"

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

int test_main2(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	int res;
	unsigned char buf[256];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	struct hid_device_info *devs, *cur_dev;

	printf("hidapi test/example tool. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
	if (hid_version()->major == HID_API_VERSION_MAJOR && hid_version()->minor == HID_API_VERSION_MINOR && hid_version()->patch == HID_API_VERSION_PATCH) {
		printf("Compile-time version matches runtime version of hidapi.\n\n");
	}
	else {
		printf("Compile-time version is different than runtime version of hidapi.\n]n");
	}

	if (hid_init())
		return -1;

	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;


	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	handle = hid_open(0x4d8, 0x3f, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n", wstr);

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);

	// Try to read from the device. There should be no
	// data here, but execution should not block.
	res = hid_read(handle, buf, 17);

	// Send a Feature Report to the device
	buf[0] = 0x2;
	buf[1] = 0xa0;
	buf[2] = 0x0a;
	buf[3] = 0x00;
	buf[4] = 0x00;
	res = hid_send_feature_report(handle, buf, 17);
	if (res < 0) {
		printf("Unable to send a feature report.\n");
	}

	memset(buf,0,sizeof(buf));

	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(handle, buf, sizeof(buf));
	if (res < 0) {
		printf("Unable to get a feature report.\n");
		printf("%ls", hid_error(handle));
	}
	else {
		// Print out the returned buffer.
		printf("Feature Report\n   ");
		for (i = 0; i < res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}

	memset(buf,0,sizeof(buf));

	// Toggle LED (cmd 0x80). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 17);
	if (res < 0) {
		printf("Unable to write()\n");
		printf("Error: %ls\n", hid_error(handle));
	}


	// Request state (cmd 0x81). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x81;
	hid_write(handle, buf, 17);
	if (res < 0)
		printf("Unable to write() (2)\n");

	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().
	res = 0;
	while (res == 0) {
		res = hid_read(handle, buf, sizeof(buf));
		if (res == 0)
			printf("waiting...\n");
		if (res < 0)
			printf("Unable to read()\n");
		#ifdef _WIN32
		Sleep(500);
		#else
		usleep(500*1000);
		#endif
	}

	printf("Data read:\n   ");
	// Print out the returned buffer.
	for (i = 0; i < res; i++)
		printf("%02hhx ", buf[i]);
	printf("\n");

	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef _WIN32
	system("pause");
#endif

	return 0;
}

#if 1
int test_main3(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    (void)argc;
    (void)argv;

    int res;
    unsigned char buf[1024*8];
    #define MAX_STR 255
    wchar_t wstr[MAX_STR];
    hid_device *handle;
    int i;
    int count;

    struct hid_device_info *devs, *cur_dev;

    printf("hidapi test/example tool. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
    if (hid_version()->major == HID_API_VERSION_MAJOR && hid_version()->minor == HID_API_VERSION_MINOR && hid_version()->patch == HID_API_VERSION_PATCH) {
        printf("Compile-time version matches runtime version of hidapi.\n\n");
    }
    else {
        printf("Compile-time version is different than runtime version of hidapi.\n]n");
    }

    if (hid_init())
        return -1;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev) {
        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        printf("\n");
        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printf("  Product:      %ls\n", cur_dev->product_string);
        printf("  Release:      %hx\n", cur_dev->release_number);
        printf("  Interface:    %d\n",  cur_dev->interface_number);
        printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
        printf("\n");
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    // Set up the command buffer.
    memset(buf,0x00,sizeof(buf));
    buf[0] = 0x01;
    buf[1] = 0x81;


    // Open the device using the VID, PID,
    // and optionally the Serial number.
    ////handle = hid_open(0x4d8, 0x3f, L"12345");
    //handle = hid_open(0x4d8, 0x3f, NULL);
    handle = hid_open(0x34D3, 0x1005, NULL);
    if (!handle) {
        printf("unable to open device\n");
        return 1;
    }

    // Read the Manufacturer String
    wstr[0] = 0x0000;
    res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
    if (res < 0)
        printf("Unable to read manufacturer string\n");
    printf("Manufacturer String: %ls\n", wstr);

    // Read the Product String
    wstr[0] = 0x0000;
    res = hid_get_product_string(handle, wstr, MAX_STR);
    if (res < 0)
        printf("Unable to read product string\n");
    printf("Product String: %ls\n", wstr);

    // Read the Serial Number String
    wstr[0] = 0x0000;
    res = hid_get_serial_number_string(handle, wstr, MAX_STR);
    if (res < 0)
        printf("Unable to read serial number string\n");
    printf("Serial Number String: (%d) %ls", wstr[0], wstr);
    printf("\n");

    // Read Indexed String 1
    wstr[0] = 0x0000;
    res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
    if (res < 0)
        printf("Unable to read indexed string 1\n");
    printf("Indexed String 1: %ls\n", wstr);

    // Set the hid_read() function to be non-blocking.
    hid_set_nonblocking(handle, 1);

    // Try to read from the device. There should be no
    // data here, but execution should not block.
    /*res = hid_read(handle, buf, 17);

    // Send a Feature Report to the device
    buf[0] = 0x2;
    buf[1] = 0xa0;
    buf[2] = 0x0a;
    buf[3] = 0x00;
    buf[4] = 0x00;
    res = hid_send_feature_report(handle, buf, 17);
    if (res < 0) {
        printf("Unable to send a feature report.\n");
    }

    memset(buf,0,sizeof(buf));

    // Read a Feature Report from the device
    buf[0] = 0x2;
    res = hid_get_feature_report(handle, buf, sizeof(buf));
    if (res < 0) {
        printf("Unable to get a feature report.\n");
        printf("%ls", hid_error(handle));
    }
    else {
        // Print out the returned buffer.
        printf("Feature Report\n   ");
        for (i = 0; i < res; i++)
            printf("%02hhx ", buf[i]);
        printf("\n");
    }*/

    for(count=0; count<10; count++)
    {
        memset(buf,0,sizeof(buf));

        // Toggle LED (cmd 0x80). The first byte is the report number (0x1).
        /*buf[0] = 0x1;
        buf[1] = 0x05;
        buf[2] = 0x05;
        buf[3] = 0x05;
        buf[4] = 0x80;
        res = hid_write(handle, buf, 521);
        if (res < 0) {
            printf("Unable to write()\n");
            printf("Error: %ls\n", hid_error(handle));
        }*/


        // Request state (cmd 0x81). The first byte is the report number (0x1).
        buf[0] = 0x1;
        buf[1] = 0x05;
        buf[2] = 0x05;
        buf[3] = 0x05;
        buf[4] = 0x81;
        hid_write(handle, buf, 522*2);
        if (res < 0)
            printf("Unable to write() (2)\n");

        // Read requested state. hid_read() has been set to be
        // non-blocking by the call to hid_set_nonblocking() above.
        // This loop demonstrates the non-blocking nature of hid_read().
        res = 0;
        Sleep(10);
        while (res == 0) {
            res = hid_read(handle, buf, sizeof(buf));
            if (res == 0)
                printf("waiting...%d ", count);//printf("waiting...\n");
            if (res < 0)
                printf("Unable to read()\n");
            if(res>0) break;
            #ifdef _WIN32
            Sleep(100);//Sleep(500);
            #else
            usleep(500*1000);
            #endif
        }

        printf("Data read:\n   ");
        // Print out the returned buffer.
        for (i = 0; i < res; i++)
            printf("%02hhx ", buf[i]);
        printf("\n");
        Sleep(40);
    }

    hid_close(handle);

    /* Free static HIDAPI objects. */
    hid_exit();

#ifdef _WIN32
    //system("pause");
#endif

    return 0;
}
