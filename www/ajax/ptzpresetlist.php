<?php

/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

class ptzpresetlist extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $onvif_rec = data::query("SELECT * FROM ipPtzCommandPresets WHERE name='ONVIF' AND protocol='onvif' LIMIT 1");
        if (empty($onvif_rec)) {
            data::query("INSERT INTO ipPtzCommandPresets (`name`, `driver`, `mright`, `mleft`, `up`, `down`, `up_right`, `up_left`, `down_right`, `down_left`, `wide`, `tight`, `focus_in`, `focus_out`, `preset_save`, `preset_go`, `stop`, `stop_zoom`, `needs_stop`, `http_auth`, `custom`, `port`, `protocol`) VALUES ('ONVIF', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', 0, 0, 0, '', 'onvif')", true);
        }

        $this->setView('ajax.ptzpresetlist');

        $this->view->presets = data::getObject('ipPtzCommandPresets');

        $camera_id = (!empty($_GET['id'])) ? intval($_GET['id']) : false;
        $this->view->camera = new ipCamera($camera_id);
    }

    public function postData()
    {

    }
}

