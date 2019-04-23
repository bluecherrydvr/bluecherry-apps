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

class cameraperms extends Controller {
	public $camera_list;
	public $user;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.cameraperms');

		$id = intval($_GET['id']);
		$this->user = new user('id', $id);
		$type = (Inp::get('t') == 'PTZ') ? 'access_ptz_list' : 'access_device_list';
		$this->camera_list = $this->matchCams($this->user->info[$type]);


        $p = new StdClass();
        $p->camera_list = $this->camera_list;
        $p->user = $this->user;

        $this->view->p = $p;
    }

    private function matchCams($access_list)
    {
		$camera_list = data::query("SELECT id, device_name FROM Devices");
		foreach($camera_list as $key => $camera){
			$camera_list[$key]['allowed'] = (in_array($camera_list[$key]['id'], $access_list)) ? false : true;
		}
		return $camera_list;
	}

}

