<?php

class StandaloneDatabase {
    private $dblink;
    private $dbname;
    private $dbuser;
    private $dbpassword;
    private $dbhost;
    private $connection_timeout = 10; // 10 second timeout
    private $max_retries = 3;

    public function __construct() {
        $this->connect();
    }

    public function read_config() {
        $config_file = fopen("/etc/bluecherry.conf", 'r') or die(LANG_DIE_COULDNOTOPENCONF);
        $config_text = fread($config_file, filesize("/etc/bluecherry.conf"));
        fclose($config_file);

        $matches = array();

        preg_match("/dbname[[:blank:]]*=[[:blank:]]*\"(.*)\";/", $config_text, $matches) or die(LANG_DIE_COULDNOTOPENCONF);
        $dbname = $matches[1];

        preg_match("/user[[:blank:]]*=[[:blank:]]*\"(.*)\";/", $config_text, $matches) or die(LANG_DIE_COULDNOTOPENCONF);
        $dbuser = $matches[1];

        preg_match("/password[[:blank:]]*=[[:blank:]]*\"(.*)\";/", $config_text, $matches) or die(LANG_DIE_COULDNOTOPENCONF);
        $dbpassword = $matches[1];

        if (!preg_match("/host[[:blank:]]*=[[:blank:]]*\"(.*)\";/", $config_text, $matches)) {
            $dbhost = "localhost";
        }
        else {
            $dbhost = $matches[1];
        }

        return array($dbname, $dbuser, $dbpassword, $dbhost);
    }

    private function load_config(): void
    {
        list($dbname, $dbuser, $dbpassword, $dbhost) = $this->read_config();
        $this->dbname = stripslashes($dbname);
        $this->dbuser = stripslashes($dbuser);
        $this->dbpassword = stripslashes($dbpassword);
        $this->dbhost = stripslashes($dbhost);
    }

    // CRITICAL FIX: Improved connection with retry logic and health checking
    private function connect(): void
    {
        $this->load_config();
        
        $retries = 0;
        while ($retries < $this->max_retries) {
            // CRITICAL FIX: Set connection timeout
            $this->dblink = mysqli_init();
            if (!$this->dblink) {
                $retries++;
                continue;
            }
            
            // CRITICAL FIX: Set connection options for better reliability
            mysqli_options($this->dblink, MYSQLI_OPT_CONNECT_TIMEOUT, $this->connection_timeout);
            // Use constants only if they exist (PHP version compatibility)
            if (defined('MYSQLI_OPT_READ_TIMEOUT')) {
                mysqli_options($this->dblink, MYSQLI_OPT_READ_TIMEOUT, 30);
            }
            if (defined('MYSQLI_OPT_WRITE_TIMEOUT')) {
                mysqli_options($this->dblink, MYSQLI_OPT_WRITE_TIMEOUT, 30);
            }
            
            $this->dblink = mysqli_real_connect($this->dblink, $this->dbhost, $this->dbuser, $this->dbpassword, $this->dbname);
            
            if ($this->dblink) {
                // CRITICAL FIX: Set charset and other options
                mysqli_real_query($this->dblink, "set names utf8;");
                mysqli_real_query($this->dblink, "SET SESSION wait_timeout=300;");
                mysqli_real_query($this->dblink, "SET SESSION interactive_timeout=300;");
                return;
            }
            
            $retries++;
            if ($retries < $this->max_retries) {
                usleep(100000); // 100ms delay before retry
            }
        }

        if (!$this->dblink) {
            header('HTTP/1.1 503 Service Unavailable');
            Reply::ajaxDie('unavailable', LANG_DIE_COULDNOTCONNECT);
        }
    }
    
    // CRITICAL FIX: Check connection health
    private function check_connection() {
        if (!$this->dblink || !mysqli_ping($this->dblink)) {
            error_log("Database connection lost, attempting to reconnect");
            $this->connect();
        }
    }

    public function query($query) {
        $this->check_connection();
        
        $ret = false;
        $retries = 0;
        
        while ($retries < $this->max_retries) {
            $ret = mysqli_real_query($this->dblink, $query);
            
            if ($ret) {
                break;
            }
            
            $error = mysqli_error($this->dblink);
            if (strpos($error, 'MySQL server has gone away') !== false || 
                strpos($error, 'Lost connection') !== false) {
                $retries++;
                $this->connect();
                continue;
            }
            
            break;
        }

        if (!$ret)
            trigger_error(mysqli_error($this->dblink)." query:".$query, E_USER_ERROR);

        return $ret;
    }

    public function fetchAll($query) {
        $this->check_connection();
        
        $fetchedTable = array();
        $retries = 0;
        
        while ($retries < $this->max_retries) {
            $qresult = mysqli_query($this->dblink, $query, MYSQLI_STORE_RESULT);

            if ($qresult) {
                $fetchedTable = mysqli_fetch_all($qresult, MYSQLI_ASSOC);
                mysqli_free_result($qresult);
                break;
            }
            
            $error = mysqli_error($this->dblink);
            if (strpos($error, 'MySQL server has gone away') !== false || 
                strpos($error, 'Lost connection') !== false) {
                $retries++;
                $this->connect();
                continue;
            }
            
            trigger_error(mysqli_error($this->dblink)." query:".$query, E_USER_ERROR);
            break;
        }

        return $fetchedTable;
    }

    public function last_id() {
        $this->check_connection();
        return mysqli_insert_id($this->dblink);
    }
}
