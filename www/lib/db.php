<?php

class StandaloneDatabase {
    private $dblink;
    private $dbname;
    private $dbuser;
    private $dbpassword;
    private $dbhost;

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

    private function connect(): void
    {
        $this->load_config();
        $this->dblink = mysqli_connect($this->dbhost, $this->dbuser, $this->dbpassword, $this->dbname);

        if (!$this->dblink) {
            header('HTTP/1.1 503 Service Unavailable');
            Reply::ajaxDie('unavailable', LANG_DIE_COULDNOTCONNECT);
        }

        mysqli_real_query($this->dblink, "set names utf8;");
    }

    public function escapeString($string): string
    {
        $string = mysqli_real_escape_string($this->dblink, $string);
        return $string;
    }

    public function query($query): bool
    {
        $ret = mysqli_real_query($this->dblink, $query);

        if (!$ret)
            trigger_error(mysqli_error($this->dblink)." query:".$query, E_USER_ERROR);

        return $ret;
    }

    public function fetchAll($query): array
    {
        $fetchedTable = array();
        $qresult = mysqli_query($this->dblink, $query, MYSQLI_STORE_RESULT);

        if ($qresult) {
            $fetchedTable = mysqli_fetch_all($qresult, MYSQLI_ASSOC);
        }
        else {
            trigger_error(mysqli_error($this->dblink)." query:".$query, E_USER_ERROR);
        }

        return $fetchedTable;
    }

    public function last_id(): int|string
    {
        return mysqli_insert_id($this->dblink);
    }
}
