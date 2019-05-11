//------------------------------------------------------------------------------
// Copyright (C) 2010, Raditex AB
// All rights reserved.
//
// rSCADA
// http://www.rSCADA.se
// info@rscada.se
//
//------------------------------------------------------------------------------

#include "mbus-protocol.h"
#include "../config.h"

//
//
//
int ICACHE_FLASH_ATTR mbus_init() {return 0;}

///
/// Return current version of the library
///
const char*
ICACHE_FLASH_ATTR
mbus_get_current_version() {return VERSION;}
