# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Include python install macros
########################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

########################################################################
# Install python sources
########################################################################
GR_PYTHON_INSTALL(
    PROGRAMS
    benchmark_ofdm.py
    rx.py
    tx.py
    DESTINATION bin
)

GR_PYTHON_INSTALL(
    FILES
    configparse.py
    DESTINATION ${GR_PYTHON_DIR}
)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    autocorrelator.py
    channel.py
    common_options.py
    delaylines.py
    file_psd.py
    fusb_options.py
    gr_tools.py
    moms_general.py
    moms.py
    morelli.py
    ofdm_receiver2.py
    ofdm_receiver.py
    preambles.py
    receive_path.py
    schmidl.py
    snr_estimator.py
    station_configuration.py
    transmit_path2.py
    transmit_path.py
    uhd_interface.py
    vector_equalizer.py
    DESTINATION ${GR_PYTHON_DIR}/ofdm
)