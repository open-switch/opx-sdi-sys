/*
 * Copyright (c) 2019 Dell Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/*
 * filename: sdi_sfp_plus_aq.h
 */

#ifndef _SDI_SFP_PLUS_AQ_H_ 
#define _SDI_SFP_PLUS_AQ_H_

#include "sdi_media_ext_mod_ctrl.h"
#include "sdi_media.h"


t_std_error sdi_media_aq_set_rate (sdi_resource_hdl_t resource_hdl, sdi_media_speed_t rate);
t_std_error sdi_media_aq_get_system_side_status (sdi_resource_hdl_t resource_hdl, bool *rx_if_link_status, bool *tx_if_link_status,
                    sdi_media_speed_t *if_speed_rate);
t_std_error sdi_media_aq_set_tx_state (sdi_resource_hdl_t resource_hdl, bool state);
t_std_error sdi_media_aq_get_tx_state (sdi_resource_hdl_t resource_hdl, bool *state);
t_std_error sdi_media_aq_get_line_side_status (sdi_resource_hdl_t resource_hdl, bool *link_status, sdi_media_speed_t *link_speed_rate);
t_std_error sdi_media_aq_get_link_status(sdi_resource_hdl_t resource_hdl, bool* status);

#endif
