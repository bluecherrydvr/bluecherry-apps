<?php DEFINE(INDVR, true);

#lib
include("../lib/lib.php");  #common functions
	#fake fake fake :p
	
	Class MotionMap extends DVRData{
	public $data;
	
	public function __construct(){
		$id = intval($_GET['id']);
		$this->data = $this->GetObjectData('devices', 'id', $id);
	}
}

$motion_map = new MotionMap;

header('Content-type: image/png');
$file = ($_GET['id']!='none') ? 'http://localhost/_TMP/'.(($motion_map->data[0]['signal_type']=='NTSC') ? 'NTSC' : 'PAL').'.png' : 'http://localhost/img/icons/layouts/none.png';
//echo $file;
readfile($file);

?>