<?php 


class storagecheck extends Controller {
	
    public function __construct()
    {
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
        $check_dir = $this->directory_staus($_GET['path'], 'new');

        data::responseJSON($check_dir[0], $check_dir[1]);
    }

    // path -- path to storage dir, type -- new or existing, existing are not checked for existing files
    private function directory_staus($path, $type)
    {
    	#if file 
    	if (!file_exists($path)){
    		return array('F', str_replace('%PATH%', $path, DIR_DOES_NOT_EXIST_OR_NOT_READABLE));
    	}
    	if (!is_readable($path)){
    		return array('F', str_replace('%PATH%', $path, DIR_NOT_READABLE));
    	}
    	if ($type == 'new'){
    		return (count(scandir($path)) == 2) ? array('OK', DIR_OK) : array('INFO', str_replace('%PATH%', $path, DIR_NOT_EMPTY));
    	}
    }
}

