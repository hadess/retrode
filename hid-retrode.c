/*
 *  HID driver for Retrode 2 controller adapter and plug-in extensions
 *
 *  Copyright (c) 2017 Bastien Nocera <hadess@hadess.net>
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/input.h>
#include <linux/slab.h>
#include <linux/hid.h>
#include <linux/module.h>
#include "hid-ids.h"

#define CONTROLLER_NAME_BASE "Retrode"

static int retrode_input_configured(struct hid_device *hdev,
					struct hid_input *hi)
{
	struct hid_field *field = hi->report->field[0];
	const char *suffix;
	int number = 0;
	unsigned int len;
	char *name;

	switch (field->report->id) {
	case 0:
		suffix = "SNES Mouse";
		break;
	case 1:
	case 2:
		suffix = "SNES";
		number = field->report->id;
		break;
	case 3:
	case 4:
		suffix = "Mega Drive";
		number = field->report->id - 2;
		break;
	default:
		hid_err(hdev, "Got unhandled report id %d\n", field->report->id);
		suffix = "Unknown";
	}

	len = strlen(CONTROLLER_NAME_BASE) + strlen(suffix) +
			number ? 5 : 2;
	name = devm_kzalloc(&hi->input->dev, len, GFP_KERNEL);
	if (name) {
		if (number)
			sprintf(name, "%s %s #%d", CONTROLLER_NAME_BASE,
				suffix, number);
		else
			sprintf(name, "%s %s", CONTROLLER_NAME_BASE,
				suffix);
		hi->input->name = name;
	}

	return 0;
}

static int retrode_probe(struct hid_device *hdev,
			const struct hid_device_id *id)
{

	int ret;

	/* Has no effect on the mouse device */
	hdev->quirks |= HID_QUIRK_MULTI_INPUT;

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "parse failed\n");
		return ret;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		return ret;
	}

	return 0;
}

static const struct hid_device_id retrode_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_FUTURE_TECHNOLOGY, USB_DEVICE_ID_RETRODE2) },
	{ }
};
MODULE_DEVICE_TABLE(hid, retrode_devices);

static struct hid_driver retrode_driver = {
	.name             = "hid-retrode",
	.id_table         = retrode_devices,
	.input_configured = retrode_input_configured,
	.probe            = retrode_probe,
};

module_hid_driver(retrode_driver);

MODULE_LICENSE("GPL");
