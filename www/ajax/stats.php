<?php 

#server stats: CPU/memusage

class stats extends Controller {
	public function __construct() {
        parent::__construct();

		$this->chAccess('admin');

	}

    public function getData()
    {
        $this->makeXML();

        die();
    }

    public function postData()
    {
        $this->makeXML();

        die();
    }


	function getCPUUsage() {
		$p = @popen("/usr/bin/mpstat", "r");
		if (!$p)
			return 0;
		while ($str = fgets($p)) {
			if (!strstr($str, "all"))
				continue;
			$cpu = preg_split("/[\s,]+/", $str);
			break;
		}
		pclose($p);
		$id = count($cpu) - 2;
		return round(100 - $cpu[$id], 2);
	}
	function getMemUse() {
                $f = @fopen("/proc/meminfo", "r");

		if (!$f)
			return array(0, 0, 0);

                for ($i = 0; $str = fgets($f); $i++) {
                        $str_el = preg_split("/[\s:]+/", $str, 3);
                        $stat[$str_el[0]] = intval($str_el[1]);
                }
                fclose($f);
		
		// fix for linux kernel version < 3.14
		if( !isset( $stat["MemAvailable"] ) )
		{
			$stat["MemAvailable"] = $stat["Cached"] + $stat["MemFree"];
		}

                $total = $stat["MemTotal"];
                $used = $total - $stat["MemAvailable"];
                $percent = round($used / $total * 100, 2);

                return array($total, $used, $percent);
        }
	function getUpTime(){
		$file = file_get_contents("/proc/uptime", "r");
		$tmp = explode(" ", $file);
		$dd = (round($tmp[0])<86400 || round($tmp[0])>= 172800) ? " \d\a\y\s " : " \d\a\y ";
		//return date("j ".trim($dd)."\, H:i:s",-64800 + round($tmp[0]));
		$d = intval($tmp[0]/86400); $tmp[0] -= $d*86400;
		$h = str_pad(intval($tmp[0]/3600), 2, '0', STR_PAD_LEFT); $tmp[0] -= $h*3600;
		$m = str_pad(intval($tmp[0]/60), 2, '0', STR_PAD_LEFT); $tmp[0] -= $m*60;
		return $d.' day(s), '.$h.':'.$m.':'.str_pad(intval($tmp[0]), 2, '0', STR_PAD_LEFT);
	}
	private function isServerRunning(){
		return (shell_exec('pidof bc-server')) ? true : false;
	}
	private function serverStatus(){
		$status = data::getObject('ServerStatus');
		if (!$status){
			return false;
		} else {
			return array($status[0]['pid'], $status[0]['timestamp'], $status[0]['message']);
		}

	}
	function makeXML(){
		$cpu = $this->getCPUUsage();
		$mem = $this->getMemUse();
		$uptime  = $this->getUpTime();
		$server  = $this->isServerRunning();
		$serverstatus = $this->serverStatus();

		header('Content-type: text/xml');
		echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
<stats>
	<cpu-usage>$cpu</cpu-usage>
	<memory-total>{$mem[0]}</memory-total>
	<memory-inuse>{$mem[1]}</memory-inuse>
	<memory-used-percentage>{$mem[2]}</memory-used-percentage>
	<bc-server-running>".(($server)? 'up' : 'down')."</bc-server-running>
	<server-uptime>$uptime</server-uptime>
	<server-status>
		<pid>{$serverstatus[0]}</pid>
		<timestamp>{$serverstatus[1]}</timestamp>
		<message>{$serverstatus[2]}</message>
	</server-status>
</stats>\n";
	}
}

