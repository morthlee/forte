#*******************************************************************************
# * Copyright (c) 2010 - 2018 ACIN
# * All rights reserved. This program and the accompanying materials
# * are made available under the terms of the Eclipse Public License v1.0
# * which accompanies this distribution, and is available at
# * http://www.eclipse.org/legal/epl-v10.html
# *
# * Contributors:
# *   Alois Zoitl, Patrick Smejkal, Martin Melik-Merkumians
# *    - initial API and implementation and/or initial documentation
# *    - added utils directory
# *******************************************************************************/
forte_add_include_directories(${CMAKE_CURRENT_SOURCE_DIR})
SET(SOURCE_GROUP ${SOURCE_GROUP}\\arch)

add_subdirectory(utils)

add_subdirectory(ecos)
add_subdirectory(netos)
add_subdirectory(posix)
add_subdirectory(win32)
add_subdirectory(be_m1)
add_subdirectory(macos)
add_subdirectory(rcX)
add_subdirectory(vxworks)

forte_add_sourcefile_hcpp(timerha devlog)

