<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check



Class MotionMap{
	public $data;
	
	public function __construct($id){
		$this->data = data::getObject('Devices', 'id', $id);
	}
}

$motion_map = new MotionMap(intval($_GET['id']));

#require template to show data
include_once('../template/ajax/motionmap.php');

?>
