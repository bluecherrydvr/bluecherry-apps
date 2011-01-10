<?php DEFINE('INDVR', true);

#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check



Class MotionMap extends DVRData{
	public $data;
	
	public function __construct($id){
		$this->data = $this->GetObjectData('Devices', 'id', $id);
	}
}

$motion_map = new MotionMap(intval($_GET['id']));

#require template to show data
include_once('../template/ajax/motionmap.php');

?>
