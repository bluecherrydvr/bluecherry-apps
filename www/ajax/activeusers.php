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

class activeusers extends Controller {
    public $users;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.activeusers');

		$tmp = data::query("SELECT * FROM ActiveUsers");
		foreach($tmp as $k => $user){
			$this->users[$k] = new user('id', $user['id']);
			$this->users[$k]->info['ip'] = $user['ip'];
			$this->users[$k]->info['kick'] = $user['kick'];
			$this->users[$k]->info['from_client'] = $user['from_client'];
        }

        $this->view->this_users = $this->users;

		//var_dump_pre($this->users);
        if ($this->reqXML()) {
            $this->MakeXML(); 
            die();
        }
    }

    private function MakeXML()
    {
		$xml = '<?xml version="1.0" encoding="UTF-8" ?><activeusers>';
		foreach($this->users as $k => $user){
			$xml .= "<user id='{$user->info['id']}'>";
				$xml .= "<name>{$user->info['name']}</name>
						<ip>{$user->info['ip']}</ip>
						<kick>{$user->info['kick']}</kick>";
			$xml .= "</user>";
		}
		$xml .='</activeusers>';
		header('Content-type: text/xml');
		print_r($xml);
	}
}

