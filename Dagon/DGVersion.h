/*
 *  DAGON
 *  Copyright (c) 2011 Senscape s.r.l.
 *	All rights reserved.
 *
 *  NOTICE: Senscape permits you to use, modify, and distribute this
 *  file in accordance with the terms of the license agreement accompanying it.
 *
 */

#ifndef DG_VERSION_H
#define DG_VERSION_H

#define DG_VERSION_MAJOR 0
#define DG_VERSION_MINOR 6
#define DG_VERSION_RELEASE 0 // TODO: Put this in the bundle version
#define DG_VERSION_BUILD 1000

// This assumes that there will never be more than 9999 builds

#define DG_VERSIONNUM ((DG_VERSION_MAJOR)*100000 + (DG_VERSION_MINOR)*10000 + \
                        (DG_VERSION_RELEASE)*1000 + (DG_VERSION_BUILD))

#endif // DG_VERSION_H
