<?php

//makes an options dropdown
function arrayToSelect($options, $selected='', $id='', $class = ''){
	$tmp = "<SELECT id={$id} class='{$class}'>";
	foreach($options as $i => $value){
		$tmp .= "<OPTION value='$value' ".(($value==$selected) ? 'selected' : '').">$value";
	}
	$tmp .= "</SELECT>";
	return $tmp;
}



?>