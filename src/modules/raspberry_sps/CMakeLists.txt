#*******************************************************************************
# * Copyright (c) 2015 fortiss GmbH
# * All rights reserved. This program and the accompanying materials
# * are made available under the terms of the Eclipse Public License v1.0
# * which accompanies this distribution, and is available at
# * http://www.eclipse.org/legal/epl-v10.html
# *
# * Contributors:
# *    Gerd Kainz, Alois Zoitl - initial API and implementation and/or initial documentation
# *******************************************************************************/ 
 
forte_add_module(Raspberry-SPS "Interacting with GPIOs Raspberry SPS")

forte_add_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#######################################################
# i2c-dev
#######################################################


forte_add_sourcefile_hcpp(../../stdfblib/io/IX)
forte_add_sourcefile_hcpp( ../../stdfblib/io/QX)
forte_add_sourcefile_hcpp(../../stdfblib/io/IW)
forte_add_sourcefile_hcpp( ../../stdfblib/io/QW)

forte_add_sourcefile_hcpp(../i2c_dev/i2cprocessinterface)

forte_add_sourcefile_hcpp(processinterface)

