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
	public function __construct(){
		$this->makeXML();
	}
		function getLoadAvg(){
			/* DISABLED UNTIL MOVED TO SERVER 
			for ($i=0; $i<=1; $i++){
				$file = fopen("/proc/stat", "r");
				$tmp = fgets($file, 4096);				
				$tmp = preg_split("/[\s]+/", $tmp);

				for ($j=1; $j<=4; $j++){
					$reading[$i][$j-1]=$tmp[$j];	
				};
				sleep(1);
			}
			$tmp = ($reading[1][0]-$reading[0][0]+$reading[1][1]-$reading[0][1]+$reading[1][2]-$reading[0][2]);
			return round($tmp/($tmp+$reading[1][3]-$reading[0][3])*100, 2);
			*/ 
			return 0;
		}
		function getMemUse(){
			$file = fopen("/proc/meminfo", "r");
			for ($i=1; $i<=4; $i++){
				$tmp = fgets($file, 4096); $tmp = preg_replace("/[^(0-9)]/", "", $tmp);
				$memstat[$i] = $tmp;
			};
			$memstat[2] = $memstat[1] - ($memstat[2]+$memstat[3]+$memstat[4]);
			$memstat[3] = round($memstat[2]/$memstat[1]*100, 2);
			return $memstat;
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
			if (file_exists("/var/run/bluecherry/db-writable")){
				return date("F d Y H:i:s.", filemtime("/var/run/bluecherry/db-writable"));
			} else {
				return 'false';
			}
		}
		function makeXML(){
			$loadavg = $this->getLoadAvg();
			$memstat = $this->getMemUse();
			$uptime  = $this->getUpTime();
			$server  = $this->isServerRunning();
			$writefail = $this->writeFailCheck();
			//simple XML output
			header('Content-type: text/xml');
			echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
				  	<server-status>
						<cpu-usage>$loadavg</cpu-usage>
						<memory-total>".$memstat['1']."</memory-total>
						<memory-inuse>".$memstat['2']."</memory-inuse>
						<memory-used-percentage>".$memstat['3']."</memory-used-percentage>
						<bc-server-running>".(($server)? 'up' : 'down')."</bc-server-running>
						<server-uptime>$uptime</server-uptime>
						<failed-write>$writefail</failed-write>
				  	</server-status>
			";
		}
}

$stats = new ServerStats;

?>
