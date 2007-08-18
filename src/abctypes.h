/*
    Ming, an SWF output library
    Copyright (C) 2007 Klaus Rechert

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __ABCTYPES_H_
#define __ABCTYPES_H_

typedef enum {
	__add 		= 0xa0,
	__add_i		= 0xc5,
	__astype	= 0x86,
	__astypelate	= 0x87,
	__bitand	= 0xa8,
	__bitnot	= 0x97,
	__bitor 	= 0xa9,
	__bitxor	= 0xaa,
	__call		= 0x41,
	__callmethod	= 0x43,
	__callproperty	= 0x46,
	__callproplex	= 0x4c,
	__callpropvoid	= 0x4f,
	__callstatic	= 0x44,
	__callsuper	= 0x45,
	__callsupervoid = 0x4e,
	__checkfilter	= 0x78,
	__coerce	= 0x80,
	__coerce_a	= 0x82,
	__coerce_s	= 0x85,
	__construct	= 0x42,
	__constructprop	= 0x4a,
	__constructsuper= 0x49,
	__convert_b	= 0x76,
	__convert_i	= 0x73,
	__convert_d	= 0x75,
	__convert_o	= 0x77,
	__convert_u	= 0x74,
	__convert_s	= 0x70,
	__debug		= 0xef,
	__debugfile	= 0xf1,
	__debugline	= 0xf0,
	__declocal	= 0x94,
	__declocal_i	= 0xc3,
	__decrement	= 0x93,
	__decrement_i	= 0xc1,
	__deleteproperty= 0x6a,
	__divide 	= 0xa3,
	__dup		= 0x2a,
	__dxns		= 0x06,
	__dxnslate	= 0x07,
	__equals	= 0xab,
	__esc_xattr	= 0x72,
	__esc_xelem	= 0x71,
	__findproperty	= 0x5d,
	__getdescendants= 0x59,
	__getglobalscope= 0x64,
	__getglobalslot = 0x6e,
	__getlex	= 0x60,
	__getlocal	= 0x62,
	__getlocal_1	= 0xd0,
	__getlocal_2	= 0xd1,
	__getlocal_3	= 0xd2,
	__getlocal_4	= 0xd3,
	__getproperty	= 0x66,
	__getscopeobject= 0x65,
	__getslot	= 0x6c,
	__getsuper	= 0x04,
	__greaterequals = 0x00, /* unknow */
	__greaterthan	= 0xaf, 
	__hasnext	= 0x1f,
	__hasnext2	= 0x32,
	__ifeq		= 0x13,
	__iffalse	= 0x12,
	__ifge		= 0x18,
	__ifgt		= 0x17,
	__ifle		= 0x16,
	__iflt		= 0x15,
	__ifnge		= 0x0f,
	__ifngt		= 0x0e,
	__ifnle		= 0x0d,
	__ifnlt		= 0x0c,
	__ifne		= 0x14,
	__ifstricteq 	= 0x19,
	__ifstrictne	= 0x1a,
	__iftrue	= 0x11,
	__in		= 0xb4,
	__inclocal	= 0x92,
	__inclocal_i	= 0xc2,
	__increment	= 0x91,
	__increment_i	= 0xc0,
	__initproperty	= 0x68,
	__instanceof	= 0xb1,
	__istype	= 0xb2,
	__istypelate	= 0xb3,
	__jump		= 0x10,
	__kill		= 0x08,
	__label 	= 0x09,
	__lessequals	= 0xae,
	__lessthan 	= 0xad,
	__lookupswitch	= 0x1b,
	__lsshift	= 0xa5,
	__modulo	= 0xa4,
	__multiply	= 0xa2,
	__multiply_i 	= 0xc7,
	__negate	= 0x90,
	__negate_i	= 0xc4,
	__newactivation	= 0x57,
	__newarray	= 0x56,
	__newcatch	= 0x5a,
	__newclass	= 0x58,
	__newfunction 	= 0x40,
	__newobject	= 0x55,
	__nextname	= 0x1e,
	__nextvalue	= 0x23,
	__nop		= 0x02,
	__not		= 0x96,
	__pop		= 0x29,
	__popscope	= 0x1d,
	__pushbyte	= 0x24,
	__pushdouble	= 0x2f,
	__pushfalse	= 0x27,
	__pushint	= 0x2d,
	__pushnamespace	= 0x31,
	__pushnan	= 0x28,
	__pushnull	= 0x20,
	__pushscope	= 0x30,
	__pushshort	= 0x25,
	__pushstring	= 0x2c,
	__pushtrue	= 0x26,
	__pushuint	= 0x2e,
	__pushundefined	= 0x21,
	__pushwith	= 0x1c,
	__returnvalue	= 0x48,
	__returnvoid	= 0x47,
	__rshift	= 0xa6,
	__setlocal	= 0x63,
	__setlocal_0	= 0xd4,
	__setlocal_1	= 0xd5,
	__setlocal_2	= 0xd6,
	__setlocal_3	= 0xd7,
	__setglobalslot	= 0x6f,
	__setproperty	= 0x61,
	__setslot	= 0x6d,
	__setsuper	= 0x05,
	__strictequals	= 0xac,
	__subtract	= 0xa1,
	__subtract_i	= 0xc6,
	__swap		= 0x2b,
	__throw		= 0x03,
	__abc_typeof	= 0x95,
	__urshift	= 0xa7,
} SWFAbcOpCode;

/* ABC Constants */
#define ABC_CONST_QNAME		0x07
#define ABC_CONST_QNAME_A	0x0D
#define ABC_CONST_RTQNAME	0x0F
#define ABC_CONST_RTQNAME_A	0x10
#define ABC_CONST_RTQNAME_L	0x11
#define ABC_CONST_RTQNAME_LA	0x12
#define ABC_CONST_MULTINAME	0x09
#define ABC_CONST_MULTINAME_A	0x0E
#define ABC_CONST_MULTINAME_L	0x1B
#define ABC_CONST_MULTINAME_LA	0x1C
#define ABC_INT			0x03
#define ABC_UINT		0x04
#define ABC_DOUBLE		0x06
#define ABC_UTF8		0x01
#define ABC_TRUE		0x0B
#define ABC_FALSE		0x0A
#define ABC_NULL		0x0C
#define ABC_UNDEF		0x00
#define ABC_NAMESPACE		0x08
#define ABC_PACKAGE_NS		0x16
#define ABC_PACKAGE_INTERNAL_NS	0x17
#define ABC_PROTECTED_NS	0x18
#define ABC_EXPLICIT_NS		0x19
#define ABC_STATIC_PROTECTED_NS	0x1A
#define ABC_PRIVATE_NS		0x05

/* method_info flags */
#define ABC_METHOD_NEED_ARGS		0x01
#define ABC_METHOD_NEED_ACTIVATION	0x02
#define ABC_METHOD_NEED_REST		0x04
#define ABC_METHOD_HAS_OPTIONAL		0x08
#define ABC_METHOD_SET_DXNS		0x40
#define ABC_METHOD_HAS_PARAM_NAMES	0x80

/* instance_info flags */
#define ABC_CLASS_SEALED 		0x01
#define ABC_CLASS_FINAL			0x02
#define ABC_CLASS_INTERFACE		0x04
#define ABC_CLASS_PROTECTED_NS		0x08

/* Trait Constants */
#define ABC_CONST_TRAIT_SLOT		0x00
#define ABC_CONST_TRAIT_METHOD		0x01
#define ABC_CONST_TRAIT_GETTER		0x02
#define ABC_CONST_TRAIT_SETTER		0x03
#define ABC_CONST_TRAIT_CLASS		0x04
#define ABC_CONST_TRAIT_FUNCTION	0x05
#define ABC_CONST_TRAIT_CONST		0x06

#define ABC_TRAIT_ATTR_FINAL		0x01
#define ABC_TRAIT_ATTR_OVERRIDE		0x02
#define ABC_TRAIT_ATTR_METADATA		0x04

#endif
