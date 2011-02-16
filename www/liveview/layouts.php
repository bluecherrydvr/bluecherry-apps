<?php
DEFINE('INDVR', true);

#libs
include("../lib/lib.php");  #common functions

#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('viewer');
#/auth check

class editLayout{
	public function __construct($action){
		$db = DVRDatabase::getInstance();
		$db->DBEscapeString($_POST['layout']);
		switch ($action){
			case 'edit': $this->editLayout(); break;
			case 'new' : $this->newLayout(); break;
			case 'load': $this->loadLayout(); break;
			case 'delete': $this->deleteLayout(); break;
			case 'clear': $_SESSION['load_layout']='null:null;'; break;
		}
	}
	private function deleteLayout(){
		$db = DVRDatabase::getInstance();
		$db->DBQuery("DELETE FROM userLayouts WHERE user_id={$_SESSION['id']} AND layout_name='{$_POST['layout']}'"); 
	}
	private function editLayout(){
		$value = $this->cookieToString();
		$db = DVRDatabase::getInstance();
		$db->DBQuery("UPDATE userLayouts SET layout='{$value}' WHERE user_id={$_SESSION['id']} AND layout_name='{$_POST['layout']}'"); 
	}
	private function newLayout(){
		$value = $this->cookieToString();
		$db = DVRDatabase::getInstance();
		$db->DBQuery("INSERT INTO userLayouts VALUES ('', {$_SESSION['id']}, '{$_POST['layout']}', '{$value}')");
	}
	private function loadLayout(){
		$db = DVRDatabase::getInstance();
		$layout = $db->DBFetchAll("SELECT layout_name, layout FROM userLayouts WHERE layout_name='{$_POST['layout']}' AND user_id='{$_SESSION['id']}'");
		$_SESSION['load_layout'] = $layout[0]['layout'];
		$_SESSION['load_layout_name'] = $layout[0]['layout_name'];
	}
	private function cookieToString($unset = false){
		foreach($_COOKIE as $key => $value){
			if (preg_match("/imgSrcy[0-9]*x[0-9]*|lvRows|lvCols/", $key)) {
				$r .= "{$key}:{$value};";
				if ($unset) setcookie($key, '', 5);
				
			};
		}
		return $r;
	}
}

$l = new editLayout($_POST['mode']);
?>