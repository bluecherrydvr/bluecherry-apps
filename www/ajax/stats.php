<?php DEFINE('INDVR', true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new user('id', $_SESSION['id']);
$current_user->checkAccessPermissions('admin');
#/auth check

session_write_close();

#server stats: CPU/memusage

class ServerStats{
	public function __construct() {
		$this->makeXML();
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
		for ($i = 0; $str = fgets($f); $i++) {
			$stat[$i] = intval(preg_replace("/.*\s(\d+)\s.*/", "$1",
					   $str));
		}
		fclose($f);

		$total = $stat[0];
		$used = $total - ($stat[1] + $stat[2] + $stat[3]);
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
	private function writeFailCheck(){
		$time = file_exists(DB_WRITABLE) ? filemtime(DB_WRITABLE) : 0;
		if ($time>43200){
			return date("F d Y H:i:s.", $time);
		} else {
			return 'false';
		};
	}
	function makeXML(){
		$cpu = $this->getCPUUsage();
		$mem = $this->getMemUse();
		$uptime  = $this->getUpTime();
		$server  = $this->isServerRunning();
		$writefail = $this->writeFailCheck();

		header('Content-type: text/xml');
		echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
<server-status>
	<cpu-usage>$cpu</cpu-usage>
	<memory-total>{$mem[0]}</memory-total>
	<memory-inuse>{$mem[1]}</memory-inuse>
	<memory-used-percentage>{$mem[2]}</memory-used-percentage>
	<bc-server-running>".(($server)? 'up' : 'down')."</bc-server-running>
	<server-uptime>$uptime</server-uptime>
	<failed-write>$writefail</failed-write>
</server-status>\n";
	}
}

$stats = new ServerStats;

?>
