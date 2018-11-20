/*
 * dUQx firmware , requests file
 *
 * This file is part of dUQx.
 *
 * Copyright (c) 2018 Alexander López Parrado
 *
 * dUQx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dUQx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with dUQx.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef __REQUESTS_H_INCLUDED__
#define __REQUESTS_H_INCLUDED__


#define DUQX_ADC_READ_SINGLE		1
#define DUQX_ADC_READ_BUFFER		2
#define DUQX_ADC_PRESCALER_SET   	3
#define DUQX_ADC_ENABLED_SET		4
#define DUQX_ADC_RESOLUTION_SET 	5
#define DUQX_DAC_WRITE_SINGLE       6
#define DUQX_DIGITAL_CONFIGURE	    7
#define DUQX_DIGITAL_WRITE   	    8
#define DUQX_DIGITAL_READ    	    9

#endif
