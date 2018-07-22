/*
 * Intel Hex Format library
 * Copyright (C) 2018 flavio santes, flavio dot santes at 1byt3 dot com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ihexfmt.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

const char *ihexfmt_rtype_name(int record_type)
{
    static const char *name[] = { "Data",
                                  "End Of File",
                                  "Extended Segment Address",
                                  "Start Segment Address",
                                  "Extended Linear Address",
                                  "Start Linear Address" };

    if (record_type <= IHEXFMT_RT_ERR_MIN || record_type >= IHEXFMT_RT_ERR_MAX)
    {
        return "";
    }

    return name[record_type];
}

static int hex_num(int c)
{
    c = tolower(c);
    if (c >= 'a' && c <= 'f')
    {
        return 0x0A + c - 'a';
    }
    if (c >= '0' && c<= '9')
    {
        return c - '0';
    }

    return -1;
}

static int get_hex_num(FILE *f, int bytes, int *checksum)
{
    int num = 0;
    int byte;
    int i;
    int j;

    for (i = bytes - 1; i >= 0; i--)
    {
        byte = 0x00;
        for (j = 1; j >= 0; j--)
        {
            int c = fgetc(f);
            if (c == EOF)
            {
                return -1;
            }

            c = hex_num(c);
            if (c == -1)
            {
                return -1;
            }

            byte += (c << (4 * j));
        }

        if (checksum != NULL)
        {
            *checksum += byte;
        }

        num += (byte << (8 * i));
    }

    return num;
}

static int get_nums(FILE *f, unsigned char *bytes, int elements, int *checksum)
{
    int i;

    for (i = 0; i < elements; i++)
    {
        int c = get_hex_num(f, 1, checksum);
        if (c < 0)
        {
            return c;
        }
        bytes[i] = c;
    }

    return EXIT_SUCCESS;
}

static int no_op(struct ihexfmt_data *data)
{
    (void)data;

    return EXIT_SUCCESS;
}

static void cb_test(ihexfmt_cb *cb)
{
    int i;

    for (i = 0; i < IHEXFMT_RT_SIZE; i++)
    {
        if (cb[i] == NULL)
        {
            cb[i] = no_op;
        }
    }
}

int ihexfmt_load(const char *fn, ihexfmt_cb *cbs)
{
    ihexfmt_cb cb[IHEXFMT_RT_SIZE];
    struct ihexfmt_data app_data;
    int computed_checksum;
    int upper_addr = 0x00;
    int rc = EXIT_FAILURE;
    int checksum;
    FILE *f;
    int c;

    memset(&app_data, 0x00, sizeof(app_data));
    memcpy(cb, cbs, sizeof(cb));
    cb_test(cb);

    f = fopen(fn, "r");
    if (f == NULL)
    {
        return -EIO;
    }

    while (!feof(f))
    {
        computed_checksum = 0x00;

        c = fgetc(f);
        if (c != ':')
        {
            rc = -EINVAL;
            goto clean_exit;
        }

        app_data.byte_count = get_hex_num(f, 1, &computed_checksum);
        app_data.record_addr = get_hex_num(f, 2, &computed_checksum);
        app_data.record_type = get_hex_num(f, 1, &computed_checksum);
        rc = get_nums(f,
                      app_data.data,
                      app_data.byte_count,
                      &computed_checksum);
        checksum = get_hex_num(f, 1, NULL);

        if (app_data.byte_count < 0 ||
            app_data.record_addr < 0 ||
            app_data.record_type <= IHEXFMT_RT_ERR_MIN ||
            app_data.record_type >= IHEXFMT_RT_ERR_MAX ||
            rc < 0)
        {
            rc = -EINVAL;
            goto clean_exit;
        }

        computed_checksum = (~computed_checksum + 1) & 0xFF;
        if (computed_checksum != checksum)
        {
            rc = -EINVAL;
            goto clean_exit;
        }

        switch (app_data.record_type)
        {
        case IHEXFMT_RT_EXTENDED_LIN_ADDR:
            upper_addr = (app_data.data[0] << 8) + app_data.data[1];
        case IHEXFMT_RT_EXTENDED_SEG_ADDR:
            if (app_data.byte_count != 2)
            {
                rc = -EINVAL;
                goto clean_exit;
            }
            break;
        case IHEXFMT_RT_DATA:
            app_data.upper_addr = upper_addr;
            break;
        case IHEXFMT_RT_START_SEG_ADDR:
        case IHEXFMT_RT_START_LIN_ADDR:
            if (app_data.byte_count != 4)
            {
                rc = -EINVAL;
                goto clean_exit;
            }
            break;
        case IHEXFMT_RT_EOF:
            if (app_data.byte_count != 0)
            {
                rc = -EINVAL;
                goto clean_exit;
            }
            break;
        }

        if (app_data.record_type != IHEXFMT_RT_EXTENDED_LIN_ADDR)
        {
            app_data.upper_addr = 0x00;
        }

        rc = cb[app_data.record_type](&app_data);
        if (rc != EXIT_SUCCESS)
        {
            goto clean_exit;
        }

        while (!feof(f))
        {
            c = fgetc(f);
            if (!isspace(c))
            {
                ungetc(c, f);
                break;
            }
        }

    }

    if (rc == EXIT_SUCCESS)
    {
        if (app_data.record_type != IHEXFMT_RT_EOF)
        {
            rc = -EIO;
        } else
        {
            rc = EXIT_SUCCESS;
        }
    }

clean_exit:
    fclose(f);

    return rc;
}

