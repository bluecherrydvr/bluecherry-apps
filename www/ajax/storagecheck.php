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
        $change_status = $this->change_directory_permissions($_GET['path']);
        data::responseJSON($change_status[0], $change_status[1]);
    }

    // Function to call the bash script to change permissions of a directory

public function change_directory_permissions($path)
{
    // Call the bash script to change directory permissions and ownership
    $output = shell_exec("sudo /usr/share/bluecherry/scripts/check_dir_permission.sh " . escapeshellarg($path));

    // Interpret the output from the script to form the response
    if (strpos($output, 'Error') !== false) {
        return array('F', $output);
    } elseif (strpos($output, 'Changing permissions') !== false || strpos($output, 'Modifying permissions and ownership') !== false) {
        return array('OK', 'Permissions and ownership changed for ' . $path);
    } elseif (strpos($output, 'already has the correct permissions and ownership') !== false) {
        return array('OK', '');
    } elseif (strpos($output, 'does not exist. Creating it now') !== false) {
        // Handle the case where the directory is being created
        return array('OK', 'Directory created and permissions set for ' . $path);
    } else {
        return array('F', 'Unexpected output from shell script: ' . $output);
    }
}


}
