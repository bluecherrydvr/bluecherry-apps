<?php

//makes an options dropdown
function arrayToSelect($options, $selected='', $id='', $class = '', $disabled=false){
	$tmp = "<select id='{$id}' class='{$class}' name='{$id}'".(($disabled) ? ' disabled="disabled"' : '').">";
	foreach($options as $i => $value){
		if (is_array($value))$value = current($value);
		$tmp .= "<option value='$value'".(($value==$selected) ? ' selected="selected"' : '').">$value</option>";
	}
	$tmp .= "</select>";
	return $tmp;
}



?>
