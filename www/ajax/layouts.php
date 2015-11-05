<?php

class layouts extends Controller {

    public function __construct(){
        parent::__construct();
		$this->chAccess('viewer');
    }

    public function postData()
    {
		database::escapeString($_POST['layout']);
		switch (Inp::post('mode')) {
			case 'edit': $this->editLayout(); break;
			case 'new' : $this->newLayout(); break;
			case 'load': $this->loadLayout(); break;
			case 'delete': $this->deleteLayout(); break;
			case 'clear': $_SESSION['load_layout']='null:null;'; break;
		}
    }

	private function deleteLayout(){
		data::query("DELETE FROM userLayouts WHERE user_id={$_SESSION['id']} AND layout_name='{$_POST['layout']}'"); 
	}
	private function editLayout(){
		$value = $this->cookieToString();
		data::query("UPDATE userLayouts SET layout='{$value}' WHERE user_id={$_SESSION['id']} AND layout_name='{$_POST['layout']}'"); 
	}
	private function newLayout(){
		$value = $this->cookieToString();
		data::query("INSERT INTO userLayouts VALUES ('', {$_SESSION['id']}, '{$_POST['layout']}', '{$value}')");
	}
	private function loadLayout(){
		$layout = data::query("SELECT layout_name, layout FROM userLayouts WHERE layout_name='{$_POST['layout']}' AND user_id='{$_SESSION['id']}'");
		$_SESSION['load_layout'] = $layout[0]['layout'];
		$_SESSION['load_layout_name'] = $layout[0]['layout_name'];
	}
	private function cookieToString($unset = false){
		$r = '';
		foreach($_COOKIE as $key => $value){
			if (preg_match("/imgSrcy[0-9]*x[0-9]*|lvRows|lvCols/", $key)) {
				$r .= "{$key}:{$value};";
				if ($unset) setcookie($key, '', 5);
				
			};
		}
		return $r;
	}
}

