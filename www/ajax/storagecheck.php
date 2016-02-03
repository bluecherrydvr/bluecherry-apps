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
    public function directory_staus($path, $type = '')
    {
    	#if file 
    	if (!file_exists($path)){
    		return array('F', str_replace('%PATH%', $path, DIR_DOES_NOT_EXIST_OR_NOT_READABLE));
    	}

        $file_group = posix_getpwuid(filegroup($path));
        if ((!isset($file_group['name'])) || (isset($file_group['name']) && ($file_group['name'] != 'bluecherry'))) {
            return array('F', str_replace('%PATH%', $path, DIR_NOT_READABLE));
        }

        $file_owner = posix_getpwuid(fileowner($path));
        if ((!isset($file_owner['name'])) || (isset($file_owner['name']) && ($file_owner['name'] != 'bluecherry'))) {
            return array('F', str_replace('%PATH%', $path, DIR_NOT_READABLE));
        }

        $file_perms = substr(decoct(fileperms($path)), 2);
        if (($file_perms != '750')) {
            return array('F', str_replace('%PATH%', $path, DIR_NOT_READABLE));
        }

    	if ($type == 'new'){
    		return (count(scandir($path)) == 2) ? array('OK', DIR_OK) : array('INFO', str_replace('%PATH%', $path, DIR_NOT_EMPTY));
    	}

        return false;
    }
}

