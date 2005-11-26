/*
 *  TikiTest.cpp
 *  TikiTest
 *
 *  Created by Brian Peek on 1/23/05.
 *  Copyright 2005 Cryptic Allusion, LLC. All rights reserved.
 *
 */

#include "pch.h"

// Windows globals
static char szAppName[] = "TikiTest";

////////////////////////////////////
// WinMain
// initialization
////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return Tiki::DoMain(szAppName, hInst, hPrevInstance, lpCmdLine, nCmdShow);
}
