/*
 * Copyright (C) 2012, Nils Asmussen <nils@os.inf.tu-dresden.de>
 * Economic rights: Technische Universitaet Dresden (Germany)
 *
 * This file is part of NRE (NOVA runtime environment).
 *
 * NRE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NRE is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details.
 */

#pragma once

#include <arch/Types.h>

namespace nre {

/**
 * Helper class to count the number of digits in a number
 */
class Digits {
public:
    /**
     * @param n the unsigned number
     * @param base the base to use for digit-counting
     * @return the number of digits the number has when represented in base <base>
     */
    template<typename T>
    static uint count_unsigned(T n, uint base) {
        uint width = 1;
        while(n >= base) {
            n /= base;
            width++;
        }
        return width;
    }

    /**
     * @param n the signed number
     * @param base the base to use for digit-counting
     * @return the number of digits the number has when represented in base <base>
     */
    template<typename T>
    static uint count_signed(T n, uint base) {
        // we have at least one char
        uint width = 1;
        if(n < 0) {
            width++;
            n = -n;
        }
        while(n >= base) {
            n /= base;
            width++;
        }
        return width;
    }

private:
    Digits();
};

}
