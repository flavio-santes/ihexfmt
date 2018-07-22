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

#ifndef _IHEX_FMT_H_
#define _IHEX_FMT_H_

enum ihexfmt_rtype
{
    IHEXFMT_RT_ERR_MIN = -1,
    IHEXFMT_RT_DATA = 0,
    IHEXFMT_RT_EOF,
    IHEXFMT_RT_EXTENDED_SEG_ADDR,
    IHEXFMT_RT_START_SEG_ADDR,
    IHEXFMT_RT_EXTENDED_LIN_ADDR,
    IHEXFMT_RT_START_LIN_ADDR,
    IHEXFMT_RT_SIZE = 6,
    IHEXFMT_RT_ERR_MAX = 6
};

const char *ihexfmt_rtype_name(int record_type);

struct ihexfmt_data
{
    int record_type;
    int record_addr;
    int upper_addr;
    unsigned char data[256];
    int byte_count;
};

typedef int (*ihexfmt_cb)(struct ihexfmt_data *data);

int ihexfmt_load(const char *fn, ihexfmt_cb *cb);

#endif
