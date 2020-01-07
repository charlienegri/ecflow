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
"16 16 35 1",
" 	c None",
".	c #E7E7E7",
"+	c #C1C1C1",
"@	c #959595",
"#	c #CECECE",
"$	c #808080",
"%	c #D6D6D6",
"&	c #7E7E7E",
"*	c #C5C5C5",
"=	c #D4D4D4",
"-	c #7A7A7A",
";	c #DDDDDD",
">	c #C6C6C6",
",	c #E5E5E5",
"'	c #A0A0A0",
")	c #AEAEAE",
"!	c #B3B3B3",
"~	c #939393",
"{	c #D2D2D2",
"]	c #9E9E9E",
"^	c #DBDBDB",
"/	c #DFDFDF",
"(	c #8B8B8B",
"_	c #B5B5B5",
":	c #999999",
"<	c #B7B7B7",
"[	c #DADADA",
"}	c #E1E1E1",
"|	c #767676",
"1	c #D8D8D8",
"2	c #C3C3C3",
"3	c #B1B1B1",
"4	c #AAAAAA",
"5	c #828282",
"6	c #B9B9B9",
"................",
".............+@.",
"............#$%.",
"...........#&*..",
"..........=-*...",
".........;$>....",
"........,').....",
"........!~,.....",
"{]*....=&^......",
"/+(_...:<.......",
"..[$>.;&}.......",
"...#|12'........",
"....345#........",
"..../$(.........",
".....6>.........",
"................"};
static pixmap p("Check",(const char**)bits);
