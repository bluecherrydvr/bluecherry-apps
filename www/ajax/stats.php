<?php DEFINE(INDVR, true);
#lib
include("../lib/lib.php");  #common functions


#auth check
$current_user = new DVRUser();
$current_user->CheckStatus();
$current_user->StatusAction('admin');
#/auth check

session_write_close();

#server stats: CPU/memusage

class ServerStats{
	public function __construct(){
		$this->MakeXML();
	}
		function GetLoadAvg(){
			for ($i=0; $i<=1; $i++){
				$file = fopen("/proc/stat", "r");
				$tmp = fgets($file, 4096);				
				$tmp = explode(" ", $tmp);
				for ($j=2; $j<=5; $j++){
					$reading[$i][$j-1]=$tmp[$j];	
				};
				sleep(2);
			}
			$tmp = ($reading[2][1]-$reading[1][1]+$reading[2][2]-$reading[1][2]+$reading[2][3]-$reading[1][3]);
			return round($tmp/($tmp+$reading[2][4]-$reading[1][4])*100, 2);
		}
		function GetMemUse(){
			$file = fopen("/proc/meminfo", "r");
			for ($i=1; $i<=4; $i++){
				$tmp = fgets($file, 4096); $tmp = preg_replace("/[^(0-9)]/", "", $tmp);
				$memstat[$i] = $tmp;
			};
			$memstat[2] = $memstat[1] - ($memstat[2]+$memstat[3]+$memstat[4]);
			$memstat[3] = round($memstat[2]/$memstat[1]*100, 2);
			return $memstat;
		}
		function GetUpTime(){
			$file = file_get_contents("/proc/uptime", "r");
			$tmp = explode(" ", $file);
			$dd = (round($tmp[0])<86400 || round($tmp[0])>= 172800) ? " \d\a\y\s " : " \d\a\y ";
			//return date("j ".trim($dd)."\, H:i:s",-64800 + round($tmp[0]));
			$d = intval($tmp[0]/86400); $tmp[0] -= $d*86400;
			$h = str_pad(intval($tmp[0]/3600), 2, '0', STR_PAD_LEFT); $tmp[0] -= $h*3600;
 			$m = str_pad(intval($tmp[0]/60), 2, '0', STR_PAD_LEFT); $tmp[0] -= $m*60;
  			return $d.' day(s), '.$h.':'.$m.':'.str_pad(intval($tmp[0]), 2, '0', STR_PAD_LEFT);
		}
		function MakeXML(){
			//getdata
			$loadavg = $this->GetLoadAvg();
			$memstat = $this->GetMemUse();
			$uptime  = $this->GetUpTime();
			//simple XML output
			header('Content-type: text/xml');
			echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
				  	<server-status>
						<cpu-usage>$loadavg</cpu-usage>
						<memory-total>".$memstat['1']."</memory-total>
						<memory-inuse>".$memstat['2']."</memory-inuse>
						<memory-used-percentage>".$memstat['3']."</memory-used-percentage>
						<server-uptime>$uptime</server-uptime>
				  	</server-status>
			";
		}
}

$stats = new ServerStats;

?>