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

class ptzpresetedit extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.ptzpresetedit');

        $camera_id = (!empty($_GET['ref'])) ? intval($_GET['ref']) : false;
        $this->view->camera = new ipCamera($camera_id);

        $preset_id = (!empty($_GET['id'])) ? intval($_GET['id']) : 'new';
        $this->view->preset = new ipPtzPreset($preset_id);
    }

    public function postData()
    {
	    $id = $_POST['presetId']; unset($_POST['presetId']); unset($_POST['ref']);
    	$type = ($id!='new') ? 'update' : 'insert';
    	$_POST['custom'] = ($id == 'new' || $id >=9000) ? 1 : 0;
    	$query = data::formQueryFromArray($type, 'ipPtzCommandPresets', $_POST, 'id', $id);
    	$result = data::query($query, true);
    	data::responseJSON($result);
    	exit();
    }
}

