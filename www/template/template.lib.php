<?php

//makes an options dropdown
function arrayToSelect($options, $selected='', $id='', $class = '', $disabled=false){
	$tmp = "<SELECT id='{$id}' class='{$class}' name='{$id}' ".(($disabled) ? 'disabled' : '').">";
	foreach($options as $i => $value){
		if (is_array($value))$value = current($value);
		$tmp .= "<OPTION value='$value' ".(($value==$selected) ? 'selected' : '').">$value";
	}
	$tmp .= "</SELECT>";
	return $tmp;
}



?>