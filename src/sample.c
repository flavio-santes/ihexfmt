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

#include <ihexfmt.h>
#include <stdlib.h>
#include <stdio.h>

void print_data(struct ihexfmt_data *data)
{
    int i;

    printf("Record type:\t%s\n", ihexfmt_rtype_name(data->record_type));
    printf("Record addr:\t0x%04x\n", data->record_addr);
    printf("Data len:\t0x%02x\n", data->byte_count);
    if (data->record_type == IHEXFMT_RT_DATA)
    {
        printf("Upper addr:\t0x%04x\n", data->upper_addr);
    }

    printf("Data:\t\t");
    for (i = 0; i < data->byte_count; i++)
    {
        printf("%02x ", (int)data->data[i]);
    }
    printf("\n\n");
}

int on_data(struct ihexfmt_data *data)
{
    print_data(data);

    return EXIT_SUCCESS;
}

int on_eof(struct ihexfmt_data *data)
{
    print_data(data);

    return EXIT_SUCCESS;
}

int on_extended_seg_addr(struct ihexfmt_data *data)
{
    print_data(data);

    return EXIT_SUCCESS;
}

int on_start_seg_addr(struct ihexfmt_data *data)
{
    print_data(data);

    return EXIT_SUCCESS;
}

int on_extended_lin_addr(struct ihexfmt_data *data)
{
    print_data(data);

    return EXIT_SUCCESS;
}

int on_start_lin_addr(struct ihexfmt_data *data)
{
    print_data(data);

    return EXIT_SUCCESS;
}

int main(void)
{
    ihexfmt_cb cbs[IHEXFMT_RT_SIZE];
    int rc;

    cbs[IHEXFMT_RT_DATA] = on_data;
    cbs[IHEXFMT_RT_EOF] = on_eof;
    cbs[IHEXFMT_RT_EXTENDED_SEG_ADDR] = on_extended_seg_addr;
    cbs[IHEXFMT_RT_START_SEG_ADDR] = on_start_seg_addr;
    cbs[IHEXFMT_RT_EXTENDED_LIN_ADDR] = on_extended_lin_addr;
    cbs[IHEXFMT_RT_START_LIN_ADDR] = on_start_lin_addr;

    rc = ihexfmt_load("samples/sample01.hex", cbs);
    printf("Return code: %s\n", rc == EXIT_SUCCESS ? "success" : "error" );

    return 0;
}
