/*=============================================================================================*/
/* Name        :                                                                               */
/* Author      :                                                                               */
/* Revision    : $Revision: #5 $                                                                    */
/*                                                                                             */
/* Copyright 2009-2020 ECMWF.                                                                  */
/* This software is licensed under the terms of the Apache Licence version 2.0                 */
/* which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.                        */
/* In applying this licence, ECMWF does not waive the privileges and immunities                */
/* granted to it by virtue of its status as an intergovernmental organisation                  */
/* nor does it submit to any jurisdiction.                                                     */
/*                                                                                             */
/* Description :                                                                               */
/*=============================================================================================*/

#include "pixmap.h"
/* XPM */
static const char * bits[] = {
"16 16 14 1",
" 	c #E7E7E7E7E7E7",
".	c #FFFFFFFFFFFF",
"X	c #FFFFCECE9C9C",
"o	c #FFFF73739494",
"O	c #FFFF29292929",
"+	c #FFFF00000000",
"@	c #8C8C5A5A3939",
"#	c #CECECECECECE",
"$	c #8C8C8C8C8C8C",
"%	c #F7F7EFEFDEDE",
"&	c #000000000000",
"*	c #4A4A42423939",
"=	c #BDBDB5B5B5B5",
"-	c #7B7B7B7B7B7B",
"                ",
"    .XoX        ",
"   XO++@#       ",
"  X+++++$       ",
" %++++O&$       ",
" o+++**$   .o%  ",
"%+++**     o+O  ",
" +++*#    ++++o ",
"%O++*    +++++++",
".+++@.    X+++*$",
" o+++=    O++@* ",
" %O+++ooo++++&$ ",
"  X+++++++++**  ",
"   oO++++++**=  ",
"    =@@+****#   ",
"     .=$-$=.    "};
static pixmap p("Status",(const char**)bits);
