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

class ipcameracheck extends Controller {
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->initProc();
    }

    public function postData()
    {
        $this->initProc();
    }

    private function initProc()
    {
        init();

        $id = (!empty($_GET['id'])) ? intval($_GET['id']) : false;

        $camera = new ipCamera($id);

        $camera->checkConnection();

        $status_message = '';
        foreach($camera->info['connection_status'] as $type => $status){
        	if ($status!='OK'){
        		$status_message .= str_replace('%TYPE%', $type, constant('IP_ACCESS_STATUS_'.$status)).'<br /><br />';
        	}
        };

        echo $status_message;
    }
}

